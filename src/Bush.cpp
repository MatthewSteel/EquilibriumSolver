/*
    Copyright 2008, 2009 Matthew Steel.

    This file is part of EF.

    EF is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    EF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with EF.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Bush.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <cstdlib>

using namespace std;

Bush::Bush(const Origin& o, ABGraph& g, vector<unsigned>& tempStore, vector<unsigned> &reverseTS) :
origin(o), edges(g.numVertices()+1), sharedNodes(g.nodes()), tempStore(tempStore), reverseTS(reverseTS), graph(g)
{
	//Set up graph data structure:
	topologicalOrdering.reserve(g.numVertices());

	setUpGraph();
	buildTrees();//Sets up predecessors. Unnecessary if we do preds
	//in Dijkstra.
	
	sendInitialFlows();//Sends out initial flow patterns (all-or-nothing)
	
	clearChanges();
}

void Bush::setUpGraph()
{
	vector<long> distanceMap(graph.numVertices(), -1);
		//position of node i in topologicalOrdering
		//Set up in Dijkstra
	
	graph.dijkstra(origin.getOrigin(), distanceMap, topologicalOrdering);
	
	/*
	Our Dijkstra routine gives a proper topological ordering, consistent
	even in the presence of zero-length arcs. I think. Of course, the traffic
	assignment problem with zero-length arcs does not have a unique link-flow
	solution in general, so it isn't really an issue that we aren't as
	careful later on.
	*/
	
	//TEST: destination unreachable from origin.
	for(vector<pair<int, double> >::const_iterator i = origin.dests().begin(); i != origin.dests().end(); ++i) {
		if(distanceMap.at(i->first) == -1)
			std::cout << "Unreachable dest: origin " << origin.getOrigin() << ", dest " << i->first << std::endl;
	}
	
	for(unsigned i = 0; i < topologicalOrdering.size(); ++i) {
		edges[i+1] = edges[i];

		BackwardGraphEdge *j = graph.edgesFrom(topologicalOrdering.at(i));
		BackwardGraphEdge *end = graph.edgesFrom(topologicalOrdering.at(i+1));
		for(; j != end; ++j) {
			unsigned fromPosition = (unsigned)(distanceMap.at(j->fromNode()-&sharedNodes[0]));
			if(fromPosition < i) {
				++edges[i+1];
				edgeStorage.push_back(BushEdge(j));
			}
		}
	}
}

void Bush::sendInitialFlows()
{
	BushNode *root = &sharedNodes.at(origin.getOrigin());
	for(vector<pair<int, double> >::const_iterator i = origin.dests().begin(); i != origin.dests().end(); ++i) {
		//For each origin,
		BushNode* node = &sharedNodes.at(i->first);
		while(node != root) {
			//Flow back to the bush's root
			BushEdge *be = node->getMinPredecessor();
			ForwardGraphEdge* fge = graph.forward(be->underlyingEdge());
			be->addFlow(i->second, fge);
			node = be->fromNode();
		}
	}
}//Performs initial all-or-nothing flows in Bush, adding to both BushEdges and GraphEdges.

void Bush::printCrap()
{
	//Not really used, exists for debugging purposes if I really break something.
	cout << "Printing  crap:" <<endl;
	cout << "In-arcs:"<<endl;
	
	for(vector<BushNode>::iterator i = sharedNodes.begin(); i != sharedNodes.end(); ++i) {

		long nodeNum = i-sharedNodes.begin();
		cout << nodeNum << "("<< i->minDist() <<","<< i->maxDist() <<"):";

		vector<BushEdge>::iterator end = edgeStorage.begin()+edges[reverseTS[nodeNum]+1];
		
		for(vector<BushEdge>::iterator j = edgeStorage.begin()+edges[reverseTS[nodeNum]]; j!=end; ++j) {
			cout << " " << ((j->fromNode()-&sharedNodes[0])) <<
			        "(" << (j->length()) << "," << (j->flow()) << ") ";
		}
		cout << endl;
	}

	cout << "Topological Ordering:" << endl;
	for(vector<unsigned>::const_iterator i = topologicalOrdering.begin(); i != topologicalOrdering.end(); ++i)
		cout << *i << " ";
	cout << endl;
}

bool Bush::fix(double accuracy)
{
	bool localFlowChanged = false;
	do {
		localFlowChanged = equilibriateFlows(accuracy) | localFlowChanged;
	} while (updateEdges());
	return localFlowChanged;
}

bool Bush::equilibriateFlows(double accuracy)
{
	bool flowsChanged = false;//returns whether we've done any updates on our bush flows.
	buildTrees();
	while (true) {
		bool thisTime = false;
		for(vector<pair<int, double> >::const_iterator i = origin.dests().begin(); i != origin.dests().end(); ++i) {
			if (sharedNodes[i->first].getDifference() > accuracy) {
				sharedNodes[i->first].equilibriate(graph);
				//makes it better
				thisTime = true;
			}
		}//Find worst difference.
		flowsChanged = flowsChanged | thisTime;
		if(!thisTime) return flowsChanged;
		buildTrees();
	}
	return flowsChanged;
}

void Bush::buildTrees()
{
	sharedNodes[origin.getOrigin()].setDistance(0.0);
	reverseTS[origin.getOrigin()]=0;
	
	clearChanges();//Just in case, forget any edges need turning around
	
	unsigned topoIndex = 1;
	vector<BushEdge>::iterator evv = edgeStorage.begin()+edges[1];
	vector<unsigned>::iterator esp = edges.begin()+1;
	for(
		vector<unsigned>::const_iterator i = topologicalOrdering.begin()+1;
		i < topologicalOrdering.end();
		++i, ++topoIndex, ++esp
	) {
		unsigned id = *i;
		BushNode &v = sharedNodes[id];
		
		vector<BushEdge>::iterator end = edgeStorage.begin()+*esp;
		v.updateInDistances(evv, end);
		reverseTS[id]=topoIndex;
		
		updateEdges(evv, end, v.maxDist(), id);
	}
	//TODO: Sort additions
}//Resets min, max distances, builds min/max trees.

bool Bush::updateEdges()
{
	if(anyChanges()) {
		topologicalSort();//Applies changes to be made
		clearChanges();
		return true;
	}
	return false;
}//Switches direction of things that need it.

void Bush::topologicalSort()
{
	/* Used to be a zero in-degree algorithm, O(V+E). Now just a simple
	 * O(V log V) sort on nodes' max distance. Note that in real world
	 * problems E is O(V), but sorting has a huge constant-factor
	 * advantage.
	 * Because we now sort only those bits that can change (between the
	 * ends of reversed edges) we now spend a negligible amount of time
	 * in this function. Used to be around 10%.
	 */
	
	/* Preconditions: Edges in outEdges define a strict partial order
	 * on nodes, and an edge e=(v1,v2) exists only if 
	 *   maxDist(v1)< maxDist(v2).
	 * Indexes of all nodes ending up in the order need to begin in it,
	 * too.
	 * NOTE: If we deal with adjacent nodes with potential two-way zero-
	 * length connecting arcs we may need to think harder. Bar-Gera (2002)
	 * says, "strictly positive", though.
	 * 
	 * Postcondition: reverseTS etc must be valid.
	 */
	
	//NOTE: must remember to sort additions and deletions... somehow.
		//Stable sort on distance? Somehow?
	
	typedef vector<pair<unsigned, BushEdge*> >::reverse_iterator it;
	
	for(it i = deletions.rbegin(); i != deletions.rend();) {
		unsigned upperLimit = reverseTS[i->first];
		unsigned lowerLimit = upperLimit;
		
		for(; i != deletions.rend() && reverseTS[i->first] >= lowerLimit; ++i) {
			unsigned fromIndex = reverseTS[i->second->fromNode()-&sharedNodes[0]];
			
			if(lowerLimit > fromIndex) lowerLimit = fromIndex;
		}
		//TODO: Sort part of the deletions
		partialTS(lowerLimit, upperLimit+1);
	}
}

void Bush::partialTS(unsigned lower, unsigned upper)
{
	typedef vector<unsigned>::iterator vi;
	
	tempStore.clear();
	
	vi begin = topologicalOrdering.begin()+lower;
	vi end = topologicalOrdering.begin()+upper;
	
	for(vi i = begin; i != end; ++i) {
		tempStore.push_back(*i);
	}
	stable_sort(tempStore.begin(), tempStore.end(), NodeIndexComparator(sharedNodes));
	//tempStore is now a sorted list of [distance, id]
	
	//TODO: get the edges in order
	vector<BushEdge> vb(upper-lower);
	for(
		vector<unsigned>::reverse_iterator i=tempStore.rbegin();
		i != tempStore.rend();
		++i
	) {
		unsigned id = *i;
		unsigned tIndex = reverseTS[id];
		vector<BushEdge>::iterator edgesEnd = edgeStorage.begin()+edges[tIndex+1];
		for(vector<BushEdge>::iterator edgeIt = edgeStorage.begin()+edges[tIndex]; edgeIt != edgesEnd; ++edgeIt) {
			if (&(*edgeIt) == deletions.back().second) {
				deletions.pop_back();
			} else {
				vb.push_back(*edgeIt);
			}
		}
		for(; additions.back().first == id; additions.pop_back()) {
			vb.push_back(BushEdge(additions.back().second));
		}
	}
	copy(vb.begin(), vb.end(), edgeStorage.begin()+lower);
	
	unsigned numIndex=lower;
	vi tsIndex = tempStore.begin();
	for(vi i = begin; i != end; ++i, ++tsIndex, ++numIndex) {
		
		//Last one: update the topological index of the other one?
		topologicalOrdering[reverseTS[*tsIndex]] = *i;
		//set the reverseTS of the old one here to our old location
		reverseTS[*i] = reverseTS[*tsIndex];
		//set our reverseTS to our new location
		reverseTS[*tsIndex] = numIndex;
		
		*i = *tsIndex;
	}
}

long Bush::giveCount()// const
{
	//NOTE: now returns number of nodes with the same distance.
	buildTrees();
	long count = 0;
	for(vector<unsigned>::iterator i = topologicalOrdering.begin(); i != topologicalOrdering.end();) {
		double dist1 = sharedNodes.at(*i).maxDist();
		vector<unsigned>::iterator j;
		for(j = i+1; j != topologicalOrdering.end() && dist1 == sharedNodes.at(*j).maxDist(); ++j);
		count += (j-i-1);
		i = j;
	}
	return count;
	
	/*
	//Returns the number of arcs with non-zero flow from this bush. Tends to
	//reach about a fifth of all arcs in typical road networks.
	int count = 0;
	for(vector<vector<BushEdge> >::const_iterator i = bush.begin(); i != bush.end(); ++i) {
		for(vector<BushEdge>::const_iterator j = i->begin(); j != i->end(); ++j) {
			if(j->used()) ++count;
		}
	}
	return count;
	*/
}

double Bush::allOrNothingCost()
{
	/*
	NOTE: Probably not too slow (5ms or so on largest test case for all
	bushes once) but if we find a use-case where we call it all the time
	we can make it linear time by iterating in reverse topological order
	and storing flows in linear space.
	TODO: Maybe merge with sendInitialFlows() a bit - looks kinda copy and
	paste at the moment.
	*/

	buildTrees();// NOTE: Breaks constness. Grr. Make sharedNodes mutable?
	
	double cost = 0.0;
	BushNode* root = &sharedNodes.at(origin.getOrigin());
	for(vector<pair<int, double> >::const_iterator i = origin.dests().begin(); i != origin.dests().end(); ++i) {
		//For each origin,
		BushNode* node = &sharedNodes.at(i->first);
		while(node != root) {
			//Flow back to the bush's root
			BushEdge *be = node->getMinPredecessor();
			cost += i->second * be->length();
			
			node = be->fromNode();
		}
	}
	return cost;
}

Bush::~Bush()
{}

double Bush::maxDifference() {
	buildTrees();
	double ret = 0.0;
	for(std::vector<std::pair<int, double> >::const_iterator i = origin.dests().begin(); i != origin.dests().end(); ++i) {
		ret = max(ret, sharedNodes[i->first].getDifference());
	}
	return ret;
}
