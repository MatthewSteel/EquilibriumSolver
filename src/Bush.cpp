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
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <iostream>
#include <limits>
#include <cstdlib>

using namespace std;
using namespace boost;

Bush::Bush(const Origin& o, ABGraph& g, vector<pair<double,unsigned> >& tempStore) :
origin(o), bush(g.numVertices()), sharedNodes(g.nodes()), tempStore(tempStore)
{
	//Set up graph data structure:
	topologicalOrdering.reserve(g.numVertices());
	setUpGraph(g);
	topologicalSort();
	//Sends out initial flow patterns (all-or-nothing):
	buildTrees();
	sendInitialFlows();
}

void Bush::setUpGraph(ABGraph& g)
{
	vector<double> distanceMap(g.numVertices(), numeric_limits<double>::infinity());

	g.dijkstra(origin.getOrigin(), distanceMap);

	vector<GraphEdge>::iterator begin = g.begin(), end=g.end();
	/*
	Partial order to ensure acyclicity: Order on distance, then node id if
	we get a tie. Remembering, some of these arcs will be imaginary, and
	that nodes at infinite distance aren't called for. TODO: Fail if a
	destination is at infinite distance.
	
	BUG (maybe): nodes at equal distance because of zero-length connecting
	arcs aren't handled correctly. The partial order criteria don't ensure
	connectivity. Breaks Philadelphia. Bar-Gera (2002) says arcs must have
	strictly positive lengths, though.
	*/
	for(vector<GraphEdge>::iterator iter = begin; iter != end; ++iter) {
		if(distanceMap.at(iter->toNode()->getId()) != numeric_limits<double>::infinity()&&distanceMap.at(iter->fromNode()->getId()) != numeric_limits<double>::infinity() && (distanceMap.at(iter->fromNode()->getId()) < distanceMap.at(iter->toNode()->getId()) ||
			(distanceMap.at(iter->fromNode()->getId()) == distanceMap.at(iter->toNode()->getId()) &&
			iter->fromNode()->getId() < iter->toNode()->getId()))) {
			bush.at(iter->fromNode()->getId()).push_back(BushEdge(&(*iter)));
		}
	}

	//So topo sort works ok the first time:
	for(unsigned i = 0; i < sharedNodes.size(); ++i) {
		if(distanceMap.at(i) != numeric_limits<double>::infinity()) {
			sharedNodes.at(i).setDistance(distanceMap.at(i));
			topologicalOrdering.push_back(i);
		}
	}
}

void Bush::sendInitialFlows()
{
	for(vector<pair<int, double> >::const_iterator i = origin.dests().begin(); i != origin.dests().end(); ++i) {
		//For each origin,
		BushNode* node = &sharedNodes.at(i->first);
		while(node != &sharedNodes.at(origin.getOrigin())) {
			//Flow back to the bush's root
			node->getMinPredecessor()->addFlow(i->second);
			node = node->getMinPredecessor()->fromNode();
		}
	}
}//Performs initial all-or-nothing flows in Bush, adding to both BushEdges and GraphEdges.

void Bush::printCrap() const
{
	cout << "Printing  crap:" <<endl;
	cout << "Out arcs:"<<endl;
	for(vector<BushNode>::const_iterator i = sharedNodes.begin(); i != sharedNodes.end(); ++i) {
		cout << i-sharedNodes.begin()+1 << "("<< (*i).minDist() <<","<<(*i).maxDist()<<"):";
		for(vector<BushEdge>::const_iterator j = bush.at(i-sharedNodes.begin()).begin(); j!=bush.at(i-sharedNodes.begin()).end(); ++j) {
			cout << " " << (j->toNodeId()+1) << "(" << (j->length()) << "," << (j->flow())<<") ";
		}
		cout << endl;
	}
	cout << "Topological Ordering:" << endl;
	for(vector<unsigned>::const_iterator i = topologicalOrdering.begin(); i != topologicalOrdering.end(); ++i)
		cout << *i+1 << " ";
	cout << endl;
}

bool Bush::fix(double accuracy)
{
	
	bool localFlowChanged = false;
	do {
		localFlowChanged = equilibriateFlows(accuracy) | localFlowChanged;
	} while (!updateEdges());
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
				sharedNodes[i->first].equilibriate();
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
	//Reset shared data
	for(vector<BushNode>::iterator i = sharedNodes.begin(); i != sharedNodes.end(); ++i)
		i->reset();
	
	sharedNodes[origin.getOrigin()].setDistance(0.0);

	for(vector<unsigned>::const_iterator i = topologicalOrdering.begin(); i < topologicalOrdering.end(); ++i) {
		vector<BushEdge>& outEdges = bush[*i];
		if(!outEdges.empty()) {
			BushNode* v = outEdges.front().fromNode();
			v->updateOutDistances(outEdges);
		}
	}
}//Resets min, max distances, builds min/max trees.

bool Bush::updateEdges()
{
	bool same = true;
	
	for(vector<vector<BushEdge> >::iterator i = bush.begin(); i != bush.end(); ++i) {
		if(!i->empty() && !updateEdges(*i, i->front().fromNode()->maxDist(), i->front().fromNode()->getId())) {
			same = false;
		}
	}
	if(!same) topologicalSort();
	return same;
}//Switches direction of things that need it.

void Bush::topologicalSort()
{
	/* Used to be a zero in-degree algorithm, O(V+E). Now just a simple
	 * O(V log V) sort on nodes' max distance. Note that in real world
	 * problems E is O(V), but sorting has a huge constant-factor
	 * advantage.
	 * At the end of the day we don't spend much time in this function
	 * (5% at last count), but it's twice as fast as it was, and
	 * simpler to boot. Win-win.
	 *
	 * I'm considering storing my edge lists sorted instead of how I
	 * do things now, maybe keeping an inverse-topological sort. This
	 * would make buildTrees a somewhat faster, and this function and
	 * updateEdges(2) a little slower. Maybe later, probably worth it
	 * if I can be clever about it.
	 */
	
	/* Preconditions: Edges in outEdges define a strict partial order
	 * on nodes, and an edge e=(v1,v2) exists only if 
	 *   maxDist(v1)< maxDist(v2).
	 * Indexes of all nodes ending up in the order need to begin in it,
	 * too.
	 * NOTE: If we deal with adjacent nodes with potential two-way zero-
	 * length connecting arcs we may need to think harder. Bar-Gera (2002)
	 * says, "strictly positive", though.
	 */
	
	vector<pair<double,unsigned> >::iterator index = tempStore.begin();
	vector<pair<double,unsigned> >::iterator storeEnd = tempStore.begin() + topologicalOrdering.size();
	for(vector<unsigned>::iterator i = topologicalOrdering.begin(); index != storeEnd; ++i, ++index) {
		index->first = sharedNodes[*i].maxDist();
		index->second = sharedNodes[*i].getId();
	}
	sort(tempStore.begin(), storeEnd);
	
	index = tempStore.begin();
	for(vector<unsigned>::iterator i = topologicalOrdering.begin(); index != storeEnd; ++i, ++index) {
		(*i) = index->second;
	}
}

int Bush::giveCount() const
{
	/*
	Returns the number of arcs with non-zero flow from this bush. Tends to
	reach about a fifth of all arcs in typical road networks.
	*/
	int count = 0;
	for(vector<vector<BushEdge> >::const_iterator i = bush.begin(); i != bush.end(); ++i) {
		for(vector<BushEdge>::const_iterator j = i->begin(); j != i->end(); ++j) {
			if(j->used()) ++count;
		}
	}
	return count;
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
	for(vector<pair<int, double> >::const_iterator i = origin.dests().begin(); i != origin.dests().end(); ++i) {
		//For each origin,
		BushNode* node = &sharedNodes.at(i->first);
		while(node != &sharedNodes.at(origin.getOrigin())) {
			//Flow back to the bush's root
			cost += i->second*node->getMinPredecessor()->length();
			node = node->getMinPredecessor()->fromNode();
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
		if(ret < sharedNodes[i->first].getDifference()) ret = sharedNodes[i->first].getDifference();
	}
	return ret;
}
