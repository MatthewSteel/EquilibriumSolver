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
#include <tr1/tuple>

using namespace std;

ABGraph::ABGraph(const InputGraph& g) : numberOfEdges(0)
{
	unsigned nodes=g.numNodes();
	
	vector<EdgeHolder> edgesList;

	//Get a list of real and artificial arcs (sorted, contiguous etc)
	getEdgeList(edgesList, g);

	//If some nodes are over-full, move their in-edges to the end (new things).
	nodes = splitLargeNodes(edgesList, MAX_EDGES_PER_NODE, nodes);
	forwardStructure.resize(nodes);
	nodeStorage.resize(nodes);
	//CHAR_BIT/2*sizeof(unsigned) or similar later
	
	edgeStructure.reserve(nodes);
	edgeStructure.push_back(0);
	
	vector<EdgeHolder>::iterator j = edgesList.begin();
	unsigned edgesSoFar = 0;
	for(unsigned i=1; i <= nodes; ++i) {
		for(; j != edgesList.end() && j->first != i; ++j, ++edgesSoFar) {
			addEdge(*j);
		}
		edgeStructure.push_back(edgesSoFar);
	}
	edgeStructure.push_back(edgesSoFar);
	
	for(unsigned i = 0; i < edgesList.size(); ++i) {
		EdgeHolder &e = edgesList.at(i);
		forwardStorage.at(i).setInverse(&backwardStorage.at(edge(e.first, e.second)));
	}
}

void ABGraph::getEdgeList(vector<EdgeHolder>& edgesList, const InputGraph &g)
{
	typedef vector<EdgeHolder>::iterator vpit;
	typedef map<unsigned,InputGraph::VDF> EdgeMap;
	typedef map<unsigned,EdgeMap> GraphMap;
	
	//Get a list of all edges (real or imagined)
	for(GraphMap::const_iterator i = g.graph().begin(); i != g.graph().end(); ++i) {
		unsigned fromNode = i->first;
		for(EdgeMap::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			unsigned toNode = j->first;
			edgesList.push_back(EdgeHolder(fromNode, toNode));
			edgesList.push_back(EdgeHolder(toNode, fromNode, EdgeHolder::REAL, &j->second));
		}
	}
	sort(edgesList.begin(), edgesList.end());
	
	//remove duplicates
	vpit end = unique(edgesList.begin(), edgesList.end());
	edgesList.erase(end, edgesList.end());
}

unsigned ABGraph::splitLargeNodes(vector<EdgeHolder>& edgesList, unsigned MAX_EDGES_PER_NODE, unsigned nodes)
{
	for(unsigned i = 0; i < edgesList.size();) {
		unsigned j=i+1;
		unsigned node = edgesList.at(i).first;
		for(; j < edgesList.size() && node == edgesList.at(j).first; ++j);
		//(i,j) is the range of in-arcs to a given node
		
		if(j-i>MAX_EDGES_PER_NODE) { //too crowded
			
			//find out how many extra nodes,
			unsigned extraNodes = (j-i-3)/(MAX_EDGES_PER_NODE-2);
			unsigned edgeOverflow = j-i-MAX_EDGES_PER_NODE+1;
			
			//link them all up
			edgesList.push_back(EdgeHolder(node, nodes, EdgeHolder::ZERO));
			edgesList.push_back(EdgeHolder(nodes, node, EdgeHolder::ARTIFICIAL));

			dummyNodes[node]=nodes;//TODO: Check this is the right way around
			reverseDummies[nodes]=node;
			
			for(unsigned k=0; k+1<extraNodes; ++k) {
				dummyNodes[nodes+k]=nodes+k+1;
				reverseDummies[nodes+k+1]=nodes+k;
				
				edgesList.push_back(EdgeHolder(nodes+k, nodes+k+1, EdgeHolder::ZERO));
				edgesList.push_back(EdgeHolder(nodes+k+1, node, EdgeHolder::ARTIFICIAL));
			}
			
			//distribute the edges to the right places
			unsigned from = i+MAX_EDGES_PER_NODE-1;
			for(unsigned k=0; k < edgeOverflow; ++k) {
				edgesList.at(from+k).first= nodes+k/(MAX_EDGES_PER_NODE-2);
			}
			edgesList.at(j-1).first = nodes+extraNodes-1;
			//If MAX_EDGES_PER_NODE==3, node capacities are 2-1-1-...-1-2.
			
			nodes += extraNodes;
		}
		i=j;
	}
	sort(edgesList.begin(), edgesList.end());
	return nodes;
}

void ABGraph::dijkstra(unsigned origin, vector<long>& distances, vector<unsigned>& order)
{
	const long unvisited = -1;//Ugly, dumb
	
	priority_queue<tr1::tuple<double, long, long> > queue;
	queue.push(tr1::make_tuple(0.0, 0, origin));
	
	while(!queue.empty()) {
		double distance = tr1::get<0>(queue.top());
		long id = tr1::get<2>(queue.top());
		
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
					queue.push(tr1::make_tuple(distance - bge.distance(), -order.size(), toNodeId));
					
					//(dist - len) instead of (dist + len) because it's a max-queue (we want the min)
				}
			}
		}
		queue.pop();
		
	}
}
