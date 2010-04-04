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
#include "HornerPolynomial.hpp"
#include <iostream>
#include "InputGraph.hpp"

/**
 * Graph class providing some nice, simple storage for bush-specific data.
 * Contains a simple Dijkstra's algorithm implementation.
 */

class ABGraph
{
	private:
		std::vector<std::vector<unsigned> > edgeStructure;
		std::vector<ForwardGraphEdge> forwardStorage;
		std::vector<BackwardGraphEdge> backwardStorage;
		
		std::vector<BushNode> nodeStorage;
		//Better idea: Store these things in a row, as now, but ordered specially so we can store structure as 2 iterators.
		unsigned numberOfEdges;
		//NOTE: Is there any reason to store this member?
		//Is it ever used?

		/**
		 * Adds an edge. To-node and from-node are retrieved from the
		 * parameter's data if they're necessary.
		 */
		void addEdge(unsigned from, unsigned to, InputGraph::VDF func = HornerPolynomial(std::vector<double>(1, std::numeric_limits<double>::infinity()))) {
			forwardStorage.push_back(ForwardGraphEdge(func, &nodeStorage.at(to)));
			backwardStorage.push_back(BackwardGraphEdge(func, &nodeStorage.at(from), &forwardStorage.back()));
			
			edgeStructure.at(from).push_back(forwardStorage.size()-1);
		}

	public:
		/**
		 * ABGraph constructor. Does some minor heavy lifting, setting
		 * up storage and edge inverses from the InputGraph.
		 */
		ABGraph(const InputGraph& g);

		/**
		 * Simple structure querying to get edges between two nodes
		 */
		unsigned edge(unsigned from, unsigned to) const {
			for(std::vector<unsigned>::const_iterator i = edgeStructure[from].begin(); i != edgeStructure[from].end(); ++i) {
				if(backwardStorage[*i].getToId() == to) return (*i);
			}
			throw "Edge does not exist";
		}//Not worth doing a binary search because traffic networks are so sparse
		
		ForwardGraphEdge& forwardEdge(unsigned index) {
			return forwardStorage[index];
		}
		BackwardGraphEdge& backwardEdge(unsigned index) {
			return backwardStorage[index];
		}
		
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
		std::vector<BackwardGraphEdge>::iterator begin() {
			return backwardStorage.begin();
		}

		/**
		 * Returns an EdgeIterator pointing just past the "last"
		 * out-edge of the "last" vertex of the ABGraph.
		 */
		std::vector<BackwardGraphEdge>::iterator end() {
			return backwardStorage.end();
		}
		
		BackwardGraphEdge* backward(const ForwardGraphEdge* f) {
			return &backwardStorage[f-&forwardStorage[0]];
		}
		ForwardGraphEdge* forward(const BackwardGraphEdge* b) {
			return &forwardStorage[b-&backwardStorage[0]];
		}
		
		//TODO: Add param info, have it return a good topo order (visited).
		void dijkstra(unsigned origin, std::vector<double>& distances);
		
		/**
		 * Returns the total user travel time in the current solution.
		 */
		double currentCost() const {
			double cost = 0.0;
			std::vector<ForwardGraphEdge>::const_iterator i = forwardStorage.begin();
			std::vector<BackwardGraphEdge>::const_iterator j = backwardStorage.begin();
			for(; i != forwardStorage.end(); ++i, ++j)
				if(j->getFlow() != 0) cost += j->getFlow()*i->distance();
				//0 flow could mean imaginary arc, in which case 0*infinity = NaN.
			return cost;
		}
		
		/**
		 * Node storage access.
		 */
		//FIXME: Is this used? It seems kinda bad for it to be public, non-const.
		std::vector<BushNode>& nodes() { return nodeStorage; }
		
		friend std::ostream& operator<<(std::ostream& o, ABGraph & g) {
			for(std::vector<BackwardGraphEdge>::iterator i = g.backwardStorage.begin(); i != g.backwardStorage.end(); ++i) {
				o << *i << std::endl;
			}
			return o;
		}
		
};

#endif
