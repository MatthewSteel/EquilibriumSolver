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

ABGraph::DijkstraHeap::DijkstraHeap(unsigned origin, vector<double>& distances) : positions(distances.size(), -1), distances(distances) {
	distances.at(origin) = 0;
	positions.at(origin) = 0;
	heap.push_back(origin);
}

unsigned ABGraph::DijkstraHeap::pop() {
	unsigned top = heap.front();
	swap(heap.front(), heap.back());
	positions.at(heap.front()) = 0;
	heap.erase(heap.end()-1);
	bubbleDown(0);
	return top;
}

void ABGraph::DijkstraHeap::bubbleDown(unsigned i) {

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

void ABGraph::DijkstraHeap::bubbleUp(unsigned pos) {
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

void ABGraph::DijkstraHeap::maybePush(unsigned id, vector<GraphEdge>& neighbours) {
	for(vector<GraphEdge>::iterator i = neighbours.begin(); i != neighbours.end(); ++i) {
		int toId = i->getToId();
//		cout << "Adding " << toId << ", distance " << i->distance + distances.at(id)<<endl;
		int& position = positions.at(toId);
		if(position == -1) {
			//New to the list
			position = heap.size();
			heap.push_back(toId);
			distances.at(toId) = i->distance + distances.at(id);
			bubbleUp(heap.size()-1);
		} else if (distances.at(toId) > i->distance + distances.at(id)) {
			distances.at(toId) = i->distance + distances.at(id);
			bubbleUp(position);
			bubbleDown(position);
		}
	}
//	for(int i = 0; i < heap.size(); ++i)
//		cout << heap[i] << "("<<distances[heap[i]]<<") ";
//	cout << endl;
}


void ABGraph::EdgeIterator::operator=(const EdgeIterator& i) {
	first = i.first;
	second = i.second;
	firstLast = i.firstLast;
}
bool ABGraph::EdgeIterator::operator==(const EdgeIterator& i) {
	return (first == i.first && second == i.second);
}
bool ABGraph::EdgeIterator::operator!=(const EdgeIterator& i) {
	return (first != i.first || second != i.second);
}
ABGraph::EdgeIterator& ABGraph::EdgeIterator::operator++() {
	++second;
	while(second == first->end()) {
		++first;
		if(first != firstLast) second = first->begin();
	}
	return *this;
}//Will crash if you're dumb
ABGraph::EdgeIterator::EdgeIterator(
	vector<vector<GraphEdge> >::iterator first,
	vector<GraphEdge>::iterator second,
	vector<vector<GraphEdge> >::iterator firstLast
) : first(first), second(second), firstLast(firstLast) {}

