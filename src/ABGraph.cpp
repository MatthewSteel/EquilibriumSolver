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


#include "ABGraph.hpp"
#include <iostream>
#include <queue>
#include <utility>

using namespace std;

ABGraph::ABGraph(const InputGraph& g) : forwardStructure(g.numNodes()), edgeStructure(g.numNodes()+1), numberOfEdges(0)
{
	//Set up contiguous node storage
	nodeStorage.reserve(g.numNodes());
	for(unsigned i = 0; i < g.numNodes(); ++i) {
		nodeStorage.push_back(BushNode());
	}
	typedef map<unsigned,InputGraph::VDF> EdgeMap;
	typedef map<unsigned,EdgeMap> GraphMap;
	
	vector<unsigned> edgeNums(g.numNodes());
	
	//Set up contiguous edge storage
	//Count edges first (so we don't get a vector reallocation when we add them in)
	for(GraphMap::const_iterator i = g.graph().begin(); i != g.graph().end(); ++i) {
		unsigned fromNode = i->first;
		for(EdgeMap::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			++numberOfEdges;
			unsigned toNode = j->first;
			++edgeNums[toNode];//Log number of edges into this node
			GraphMap::const_iterator k = g.graph().find(toNode);
			if(k == g.graph().end() || k->second.find(fromNode)==k->second.end()) {
				++numberOfEdges;
				++edgeNums[fromNode];
			}//Reverse edge not found
		}
	}
	
	forwardStorage.resize(numberOfEdges);
	backwardStorage.resize(numberOfEdges);
	
	vector<unsigned>::iterator it = edgeNums.begin();
	for(vector<unsigned>::iterator i = edgeStructure.begin()+1; i != edgeStructure.end(); ++i, ++it) {
		*i = *(i-1)+*it;
	}
	
	
	forwardStorage.reserve(numberOfEdges);
	backwardStorage.reserve(numberOfEdges);

	//Mirror the basic graph structure (with imaginary back-arcs when we need them):
	for(GraphMap::const_iterator i = g.graph().begin(); i != g.graph().end(); ++i) {
		unsigned fromNode = i->first;
		for(EdgeMap::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			unsigned toNode = j->first;

			addEdge(
				fromNode,
				toNode,
				edgeStructure[toNode+1]-edgeNums[toNode],
				j->second
			);
			edgeNums[toNode]--;
			
			GraphMap::const_iterator k = g.graph().find(toNode);
			if(k == g.graph().end() || k->second.find(fromNode)==k->second.end()) {
				//Imaginary arc setup
				addEdge(
					toNode,
					fromNode,
					edgeStructure[fromNode+1]-edgeNums[fromNode]
					
				);
				--edgeNums[fromNode];
			}
		}
	}

	//Set up edge inverses
	vector<BackwardGraphEdge>::iterator j = backwardStorage.begin();
	for(vector<ForwardGraphEdge>::iterator i = forwardStorage.begin(); i != forwardStorage.end(); ++i, ++j) {
		long toNodeId = i->toNode() - &nodeStorage[0];
		long fromNodeId = j->fromNode() - &nodeStorage[0];
		i->setInverse(&backwardStorage[edge(toNodeId, fromNodeId)]);
	}
}

void ABGraph::dijkstra(unsigned origin, vector<long>& distances, vector<unsigned>& order)
{
	const long unvisited = -1;//Ugly, dumb
	
	priority_queue<pair<double, long> > queue;
	queue.push(make_pair(0.0, origin));
	
	while(!queue.empty()) {
		double distance = queue.top().first;
		long id = queue.top().second;
		
		if(distance == -std::numeric_limits<double>::infinity()) break;
		//Unnecessary, but saves putting unreachable nodes in the topo sort.
		//No real time benefit here, but saves a bit in the main algorithm.
		
		if(distances[id] == unvisited) {//first hit on this node
			
			distances[id] = order.size();//final index
			order.push_back((unsigned)id);//out-topological sort.
			
			for(vector<unsigned>::iterator i = forwardStructure[id].begin(); i != forwardStructure[id].end(); ++i) {
				ForwardGraphEdge& fge = forwardStorage[*i];
				long toNodeId = fge.toNode()-&nodeStorage[0];
				if(distances[toNodeId] == unvisited) {
					//If statement unnecessary, but cuts runtime by 1/3...
					
					BackwardGraphEdge& bge = backwardStorage[*i];
					queue.push(make_pair(distance - bge.distance(), toNodeId));
					//(dist - len) instead of (dist + len) because it's a max-queue (we want the min)
				}
			}
		}
		queue.pop();
	}
}
