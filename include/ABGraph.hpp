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


/* An old bug in the BGGraph input library led to the Boost graph library
eating all of my RAM. I made this to get around it before noticing my mistake.
Still, this seems to be faster, still uses less RAM and it makes the
executable somewhat smaller. As a bonus, we can remove Boost as a dep soon.*/

#ifndef AB_GRAPH_HPP
#define AB_GRAPH_HPP

#include <vector>
#include "GraphEdge.hpp"
#include "BushNode.hpp"
#include <iostream>

#include "InputGraph.hpp"

/**
 * Graph class providing some nice, simple storage for bush-specific data.
 * Contains a reasonable Dijkstra's algorithm implementation.
 */

class ABGraph
{
	private:
		std::vector<std::vector<GraphEdge*> > edgeStructure;
		std::vector<GraphEdge> edgeStorage;
		std::vector<BushNode> nodeStorage;
		//Better idea: Store these things in a row, as now, but ordered specially so we can store structure as 2 iterators.
		unsigned numberOfEdges;
		//NOTE: Is there any reason to store this member?
		//Is it ever used?

		/**
		 * Adds an edge. To-node and from-node are retrieved from the
		 * parameter's data if they're necessary.
		 */
		void add(GraphEdge e) {
			edgeStorage.push_back(e);
			this->edgeStructure[e.fromNode()->getId()].push_back(&edgeStorage.back());
		}

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
		//TODO: redo heap operations to use the STL
		class DijkstraHeap
		{
			//Unfortunately, without the BGL I had to do this myself too...
			public:
				/**
				 * Constructs a DijkstraHeap for graph search
				 * from a given node, with an in/out reference
				 * parameter for node distances.
				 */
				//TODO: Fix order for equal distances
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
				void maybePush(unsigned, std::vector<GraphEdge*>&);
			private:
				void bubbleUp(unsigned);
				void bubbleDown(unsigned);
				std::vector<unsigned> heap;
				std::vector<int> positions;
				std::vector<double>& distances;
		};
//		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, TAPFramework::Intersection, TAPFramework::Road> InputGraph;
	public:
		/**
		 * ABGraph constructor. Does some minor heavy lifting, setting
		 * up storage and edge inverses from the InputGraph.
		 */
		ABGraph(const InputGraph& g);

		/**
		 * Simple structure querying to get edges between two nodes
		 */
		GraphEdge* edge(unsigned from, unsigned to) {
			for(std::vector<GraphEdge*>::iterator i = edgeStructure[from].begin(); i != edgeStructure[from].end(); ++i) {
				if((*i)->getToId() == to) return (*i);
			}
			return 0;
		}//Not worth doing a binary search because traffic networks are so sparse
		
		/**
		 * Simple structure querying to get edges between two nodes (const version)
		 */
		GraphEdge* const edge(unsigned from, unsigned to) const {
			for(std::vector<GraphEdge*>::const_iterator i = edgeStructure[from].begin(); i != edgeStructure[from].end(); ++i) {
				if((*i)->getToId() == to) return (*i);
			}
			return 0;
		}//Not worth doing a binary search because traffic networks are so sparse
		
		/**
		 * Gets the number of vertices in the graph.
		 */
		std::size_t numVertices() const { return edgeStructure.size(); }
		/**
		 * Gets the number of edges in the graph.
		 */
		unsigned numEdges() const { return numberOfEdges; }

		/**
		 * Returns an EdgeIterator pointing to the "first" out-edge
		 * of the "first" vertex of the ABGraph.
		 */
		std::vector<GraphEdge>::iterator begin() {
			return edgeStorage.begin();
		}

		/**
		 * Returns an EdgeIterator pointing just past the "last"
		 * out-edge of the "last" vertex of the ABGraph.
		 */
		std::vector<GraphEdge>::iterator end() {
			return edgeStorage.end();
		}
		//NOTE: Hope we never have to deal with 0 node graphs? Size 0 vectors don't have a back().
		//NOTE: Possibly buggy even without considering 0 node graphs. Have a look over this some time.
		
		/**
		 * Our lovely Dijkstra's algorithm. You love it. I love it.
		 *
		 * "It's not horribly slow!"
		 */
		//TODO: Add param info
		void dijkstra(unsigned origin, std::vector<double>& distances) {
			DijkstraHeap d(origin, distances);
			while(!d.empty()) {
				unsigned top = d.pop();
				d.maybePush(top, edgeStructure.at(top));
			}
			
		}
		
		/**
		 * Returns the total user travel time in the current solution.
		 */
		double currentCost() const {
			double cost = 0.0;
			for(std::vector<GraphEdge>::const_iterator i = edgeStorage.begin(); i != edgeStorage.end(); ++i)
				if(i->getFlow() != 0) cost += i->getFlow()*i->distance;
				//0 flow could mean imaginary arc, in which case 0*infinity = NaN.
			return cost;
		}
		
		/**
		 * Node storage access.
		 */
		//FIXME: Is this used? It seems kinda bad for it to be public, non-const.
		std::vector<BushNode>& nodes() { return nodeStorage; }
};

#endif
