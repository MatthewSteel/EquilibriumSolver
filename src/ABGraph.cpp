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

using namespace std;
using namespace boost;

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
		//if(heap.size() > 2*i+1) cout << " against " << heap.at(2*i+1) << " distance " << distances.at(heap.at(2*i+1));
		//if(heap.size() > 2*i+2) cout << " and " << heap.at(2*i+2) << " distance " << distances.at(heap.at(2*i+2));
		//cout <<endl;
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
//	cout << "bubbling pos " << pos << " up, distance "<<distances.at(heap.at(pos));
//	if(pos > 0) { cout << " against " << distances.at(heap.at((pos-1)/2)); }
//	cout << endl;
	while(pos > 0 && distances.at(heap.at((pos-1)/2)) > distances.at(heap.at(pos))) {
//		cout <<"pos up to " << (pos-1)/2<<endl;
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
//		cout << "Adding " << toId << ", distance " << i->distance + distances.at(id)<<endl;
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
//	for(int i = 0; i < heap.size(); ++i)
//		cout << heap[i] << "("<<distances[heap[i]]<<") ";
//	cout << endl;
}

ABGraph::ABGraph(const InputGraph& g, const TAPFramework::NetworkProperties& p) : edgeStructure(num_vertices(g)), numberOfEdges(0)
{
	//Set up contiguous node storage
	nodeStorage.reserve(num_vertices(g));
	for(unsigned i = 0; i < num_vertices(g); ++i) {
		nodeStorage.push_back(BushNode(i));
	}
	
	//Set up contiguous edge storage
	//Count edges first (so we don't get a vector reallocation when we add them in)
	InputGraph::edge_iterator inBegin, inEnd;
	tie(inBegin, inEnd) = edges(g);
	for(InputGraph::edge_iterator j = inBegin; j != inEnd; ++j) {
		++numberOfEdges;
		pair<graph_traits<InputGraph>::edge_descriptor, bool> e = boost::edge(target(*j,g), source(*j,g), g);
		if(!e.second) {//Imaginary arc setup
			++numberOfEdges;
		}
	}
	edgeStorage.reserve(numberOfEdges);

	//Mirror the basic graph structure (with imaginary back-arcs when we need them):
	for(InputGraph::edge_iterator j = inBegin; j != inEnd; ++j) {
		add(GraphEdge(g[*j], p, nodeStorage.at(source(*j, g)), nodeStorage.at(target(*j, g)), target(*j,g)));
		pair<graph_traits<InputGraph>::edge_descriptor, bool> e = boost::edge(target(*j,g), source(*j,g), g);
		if(!e.second) {//Imaginary arc setup
			add(GraphEdge(nodeStorage.at(target(*j, g)), nodeStorage.at(source(*j, g)), source(*j,g)));
		}
	}

	//Set up edge inverses
	for(vector<GraphEdge>::iterator i = edgeStorage.begin(); i != edgeStorage.end(); ++i) {
		edge(i->toNode()->getId(), i->fromNode()->getId())->setInverse(&(*i));
	}

}
