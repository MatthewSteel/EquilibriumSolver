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
using namespace std;

ABGraph::DijkstraHeap::DijkstraHeap(unsigned origin, vector<double>& distances) : positions(distances.size(), -1), distances(distances)
{
	distances.at(origin) = 0;
	positions.at(origin) = 0;
	heap.push_back(origin);
}

unsigned ABGraph::DijkstraHeap::pop()
{
	unsigned top = heap.front();
	swap(heap.front(), heap.back());
	positions.at(heap.front()) = 0;
	heap.erase(heap.end()-1);
	bubbleDown(0);
	return top;
}

void ABGraph::DijkstraHeap::bubbleDown(unsigned i)
{

	while(true) {
		if(heap.size() <= 2*i+1) return;
		
		unsigned m = 2*i + ((heap.size()==2*i+2 || distances.at(heap.at(2*i+1)) < distances.at(heap.at(2*i+2)))? 1:2);
		if(distances.at(heap.at(m)) >= distances.at(heap.at(i))) return;
		positions.at(heap.at(m)) = i;
		positions.at(heap.at(i)) = m;
		swap(heap.at(i), heap.at(m));
		i = m;
		//cout << "pos to " << m << endl;
	}
}

void ABGraph::DijkstraHeap::bubbleUp(unsigned pos)
{
	while(pos > 0 && distances.at(heap.at((pos-1)/2)) > distances.at(heap.at(pos))) {
		positions.at(heap.at((pos-1)/2)) = pos;
		positions.at(heap.at(pos)) = (pos-1)/2;
		swap(heap.at((pos-1)/2), heap.at(pos));
		pos = (pos-1)/2;
	}
}

void ABGraph::DijkstraHeap::maybePush(unsigned id, vector<GraphEdge*>& neighbours)
{
	for(vector<GraphEdge*>::iterator i = neighbours.begin(); i != neighbours.end(); ++i) {
		int toId = (*i)->getToId();
		int& position = positions.at(toId);
		if(position == -1) {
			//New to the list
			position = heap.size();
			heap.push_back(toId);
			distances.at(toId) = (*i)->distance + distances.at(id);
			bubbleUp(heap.size()-1);
		} else if (distances.at(toId) > (*i)->distance + distances.at(id)) {
			distances.at(toId) = (*i)->distance + distances.at(id);
			bubbleUp(position);
			bubbleDown(position);
		}
	}
}

ABGraph::ABGraph(const InputGraph& g) : edgeStructure(g.numNodes()), numberOfEdges(0)
{
	//Set up contiguous node storage
	nodeStorage.reserve(g.numNodes());
	for(unsigned i = 0; i < g.numNodes(); ++i) {
		nodeStorage.push_back(BushNode(i));
	}
	typedef map<unsigned,InputGraph::VDF> EdgeMap;
	typedef map<unsigned,EdgeMap> GraphMap;
	
	//Set up contiguous edge storage
	//Count edges first (so we don't get a vector reallocation when we add them in)
	for(GraphMap::const_iterator i = g.graph().begin(); i != g.graph().end(); ++i) {
		unsigned fromNode = i->first;
		for(EdgeMap::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			++numberOfEdges;
			unsigned toNode = j->first;
			GraphMap::const_iterator k = g.graph().find(toNode);
			if(k == g.graph().end() || k->second.find(fromNode)==k->second.end()) {
				++numberOfEdges;
			}
			//Reverse edge not found
		}
	}
	edgeStorage.reserve(numberOfEdges);

	//Mirror the basic graph structure (with imaginary back-arcs when we need them):
	for(GraphMap::const_iterator i = g.graph().begin(); i != g.graph().end(); ++i) {
		unsigned fromNode = i->first;
		for(EdgeMap::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			unsigned toNode = j->first;
			add(GraphEdge(j->second, nodeStorage.at(fromNode), nodeStorage.at(toNode)));
			GraphMap::const_iterator k = g.graph().find(toNode);
			if(k == g.graph().end() || k->second.find(fromNode)==k->second.end()) {
				//Imaginary arc setup
				add(GraphEdge(nodeStorage.at(toNode), nodeStorage.at(fromNode)));
			}
		}
	}

	//Set up edge inverses
	for(vector<GraphEdge>::iterator i = edgeStorage.begin(); i != edgeStorage.end(); ++i) {
		edge(i->toNode()->getId(), i->fromNode()->getId())->setInverse(&(*i));
	}
}
