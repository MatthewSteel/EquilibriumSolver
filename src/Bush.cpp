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
	
	edgeStorage.reserve(graph.numEdges());
	for(unsigned i = 0; i < topologicalOrdering.size(); ++i) {
		edges[i+1] = edges[i];
		
		BackwardGraphEdge *j = graph.edgesFrom(topologicalOrdering.at(i));
		BackwardGraphEdge *end = graph.edgesFrom(topologicalOrdering.at(i)+1);
		for(; j != end; ++j) {
			unsigned fromPosition = (unsigned)(distanceMap.at(j->fromNode()-&sharedNodes[0]));
			if(fromPosition < i) {
				++edges[i+1];
				edgeStorage.push_back(BushEdge(j));
			}
		}
//		cout << i << "\t" << edges[i] << "\t" << edges[i+1] << endl;
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
		
		vector<BushEdge>::iterator end = edgeStorage.begin()+*(esp+1);
		v.updateInDistances(evv, end);
		reverseTS[id]=topoIndex;
		
		updateEdges(evv, end, v.maxDist(), id);
	}
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
	 * advantage and lets me farm things out to the STL.
	 * Because we now sort only those bits that can change (between the
	 * ends of reversed edges) we now only spend a small amount of time
	 * in this function.
	 * 
	 * In this function we find the overlapping reversed edges in the
	 * topological ordering and then call partialTS on the regions.
	 * 
	 * Diagrams:
	 * [------------------------------------------------------------]
	 *  ^(whole ordering)              reversal1:     [i-------j]
	 *     reversal2:    [i---j]
	 *                             reversal3: [i-------j]
	 * 
	 * Final regions passed to partialTS:
	 *                 1:[-----]            2:[-----------------]
	 */
	
	typedef vector<pair<unsigned, BushEdge*> >::reverse_iterator it;
	it start = deletions.rbegin();
	
	for(it i = deletions.rbegin(); i != deletions.rend(); start=i) {
		
		unsigned upperLimit = reverseTS[i->first];
		unsigned lowerLimit = upperLimit;
		
		for(; i != deletions.rend() && reverseTS[i->first] >= lowerLimit; ++i) {
			unsigned fromIndex = reverseTS[i->second->fromNode()-&sharedNodes[0]];
			if(lowerLimit > fromIndex) lowerLimit = fromIndex;
		}
		
		sort(start, i, DeletionsComparator(reverseTS, sharedNodes));
		sort(additions.rbegin()+(start-deletions.rbegin()), additions.rbegin()+(i-deletions.rbegin()), AdditionsComparator(reverseTS, sharedNodes));
		partialTS(lowerLimit, upperLimit+1);
	}
}

void Bush::partialTS(unsigned lower, unsigned upper)
{
	/*
	 * This is where the actual topological re-ordering happens. The
	 * basic idea: we take region to be sorted and make a copy of the
	 * topological order. We then do a stable-sort by the nodes'
	 * maximum distances (as per buildTrees). We apply this new order
	 * at the end.
	 * Using this new order and the relevant additions and deltions
	 * we also re-order the edge storage. We essentially make a copy
	 * of the edgeStorage bits to reorder, fill it up in the new order
	 * and then copy its contents back into the "real" edge storage
	 * vector.
	 */
	
	typedef vector<unsigned>::iterator vi;
	
	tempStore.clear();
	
	vi begin = topologicalOrdering.begin()+lower;
	vi end = topologicalOrdering.begin()+upper;
	
	for(vi i = begin; i != end; ++i) {
		tempStore.push_back(*i);
	}
	
	stable_sort(tempStore.begin(), tempStore.end(), NodeIndexComparator(sharedNodes));
	//tempStore is now a sorted list of [distance, id]
	
	updateEdgeStorage(upper, lower);

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

void Bush::updateEdgeStorage(unsigned upper, unsigned lower)
{
	/*
	 * TODO: Make this pretty. It is probably the ugliest function in the
	 * program.
	 */
	vector<BushEdge> vb;
	vector<unsigned> edgeIndices(upper-lower);
	unsigned edgeIndicesIndex=0;
	
	for(
		vector<unsigned>::reverse_iterator i=tempStore.rbegin();
		i != tempStore.rend();
		++i, ++edgeIndicesIndex
	) {
		unsigned id = *i;
		unsigned tIndex = reverseTS[id];
		
		vector<BushEdge>::iterator edgeIt = edgeStorage.begin()+edges[tIndex+1]-1;
		for(vector<BushEdge>::iterator edgesEnd = edgeStorage.begin()+edges[tIndex]; edgeIt >= edgesEnd; --edgeIt) {
			for(; additions.size() && additions.back().first == id && additions.back().second->fromNode() > edgeIt->fromNode(); additions.pop_back()) {
				vb.push_back(BushEdge(additions.back().second));
				++edgeIndices[edgeIndicesIndex];
			}
			if (deletions.size() && &(*edgeIt) == deletions.back().second) {
				deletions.pop_back();
			} else {
				vb.push_back(*edgeIt);
				++edgeIndices[edgeIndicesIndex];
			}
		}
		for(; additions.size() && additions.back().first == id; additions.pop_back()) {//TODO: do this properly
			vb.push_back(BushEdge(additions.back().second));
			++edgeIndices[edgeIndicesIndex];
		}
	}
	copy(vb.rbegin(), vb.rend(), edgeStorage.begin()+edges[lower]);
	edgeIndicesIndex = upper-lower;
	for(vector<unsigned>::iterator i=edges.begin()+lower; i != edges.begin()+upper; ++i) {
		*(i+1) = *i + edgeIndices[--edgeIndicesIndex];
	}
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
