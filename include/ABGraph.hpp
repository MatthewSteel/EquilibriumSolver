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


//An old bug in the input parsing library led to the Boost graph library was eating all my RAM.
//I made this to get around it before noticing my mistake. Still, this seems to be faster,
//still uses less RAM and it shaves 500KB off the executable size to boot.

#ifndef AB_GRAPH_HPP
#define AB_GRAPH_HPP

#include <vector>
#include "GraphEdge.hpp"
#include "BushNode.hpp"
#include <iostream>

/**
 * Graph class when I misdiagnosed some ridiculous memory usage as a library
 * problem. Provides some nice, simple storage for Graph data, and has an
 * implementation of Dijkstra's algorithm that works reasonably well.
 * Resulted in some savings in my executable size, which is nice.
 */

class ABGraph
{
	private:
		std::vector<std::vector<GraphEdge> > graphStorage;
		unsigned numberOfEdges;
		//NOTE: Is there any reason to store this member?
		//Is it ever used?

		/**
		 * A simple heap for Dijkstra's algorithm. Functionality will
		 * be replaced soon with the STL's push_heap, pop_heap and
		 * make_heap (forgot about that, used some of my old, messy
		 * code instead. Whoops.)
		 * Does some nice things by remembering where nodes are in
		 * the heap and moving them up when we revisit them, (storing
		 * a heap of nodes instead of a heap of node-observations).
		 * Of course, this doesn't really save us too much, especially
		 * when traffic networks are so sparse, but que sera sera...
		 * Works well, and I'm satisfied with it.
		 * The heap itself is a simple binary heap in a vector.
		 */
		class DijkstraHeap
		{
			//Unfortunately, without the BGL I had to do this myself too...
			public:
				/**
				 * Constructs a DijkstraHeap for graph search
				 * from a given node, with an in/out reference
				 * parameter for node distances.
				 */
				DijkstraHeap(unsigned, std::vector<double>&);

				/**
				 * Removes and returns the id of the node at
				 * the top of the heap.
				 */
				unsigned pop();

				/**
				 * Determines whether the heap is empty. (That
				 * is, determines whether Dijkstra's algorithm
				 * has terminated.)
				 */
				bool empty() { return heap.empty(); }

				/**
				 * Given a vector of edges from a node and its
				 * id, pushes/updates the heap positions of
				 * the edges' to-nodes if prudent.
				 */
				void maybePush(unsigned, std::vector<GraphEdge>&);
			private:
				void bubbleUp(unsigned);
				void bubbleDown(unsigned);
				std::vector<unsigned> heap;
				std::vector<int> positions;
				std::vector<double>& distances;
		};

	public:
		class EdgeIterator;
		
		/**
		 * ABGraph constructor. Just allocates storage for the edges,
		 * really.
		 */
		ABGraph(unsigned size) :graphStorage(size), numberOfEdges(0) {}

		/**
		 * Adds an edge. To-node and from-node are retrieved from the
		 * parameter's data if they're necessary.
		 */
		void add(GraphEdge e) {
			this->graphStorage[e.fromNode()->getId()].push_back(e);
			++numberOfEdges;
		}

		/**
		 * Simple structure querying to get edges between two nodes
		 */
		GraphEdge* edge(unsigned from, unsigned to) {
			for(std::vector<GraphEdge>::iterator i = graphStorage[from].begin(); i != graphStorage[from].end(); ++i) {
				if(i->getToId() == to) return &(*i);
			}
			return 0;
		}//Not worth doing a binary search because traffic networks are so sparse
		
		/**
		 * Gets the number of vertices in the graph.
		 */
		std::size_t numVertices() const { return graphStorage.size(); }
		/**
		 * Gets the number of edges in the graph.
		 */
		unsigned numEdges() const { return numberOfEdges; }

		/**
		 * Returns an EdgeIterator pointing to the "first" out-edge
		 * of the "first" vertex of the ABGraph.
		 */
		const EdgeIterator begin() {
			if(graphStorage.front().empty()) std::cout << "dang"<<std::endl;
			return EdgeIterator(graphStorage.begin(), graphStorage.front().begin(), graphStorage.end());
		}

		/**
		 * Returns an EdgeIterator pointing just past the "last"
		 * out-edge of the "last" vertex of the ABGraph.
		 */
		const EdgeIterator end() {
			return EdgeIterator(graphStorage.end(), graphStorage.back().end(), graphStorage.end());
		}
		//NOTE: Hope we never have to deal with 0 node graphs? Size 0 vectors don't have a back().
		//NOTE: Possibly buggy even without considering 0 node graphs. Have a look over this some time.
		
		/**
		 * Our lovely Dijkstra's algorithm. You love it. I love it.
		 *
		 * "It's not horribly slow!"
		 */
		void dijkstra(unsigned origin, std::vector<double>& distances) {
			DijkstraHeap d(origin, distances);
			while(!d.empty()) {
				unsigned top = d.pop();
				d.maybePush(top, graphStorage.at(top));
			}
			
		}
		
		/**
		 * Iterator for graph edges. Still probably a little buggy, I
		 * need to go over this a little more.
		 */
		class EdgeIterator
		{
			public:
				void operator=(const EdgeIterator& i);
				bool operator==(const EdgeIterator& i);
				bool operator!=(const EdgeIterator& i);
				EdgeIterator& operator++();
				GraphEdge& operator*() { return *second; }
				GraphEdge* operator->() { return &(*second); }
			private:
				EdgeIterator(
					std::vector<std::vector<GraphEdge> >::iterator,
					std::vector<GraphEdge>::iterator,
					std::vector<std::vector<GraphEdge> >::iterator);
				
				friend class ABGraph;
				std::vector<std::vector<GraphEdge> >::iterator first;
				std::vector<GraphEdge>::iterator second;
				std::vector<std::vector<GraphEdge> >::iterator firstLast;
		};
};

#endif
