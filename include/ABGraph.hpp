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
		std::vector<std::vector<unsigned> > forwardStructure;
		std::vector<unsigned> edgeStructure;
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
				
		class EdgeHolder {
		public:
			EdgeHolder(
				unsigned to, unsigned from,
				bool real=false,
				InputGraph::VDF func=HornerPolynomial(
					std::vector<double>(1, std::numeric_limits<double>::infinity())
				)
			) : first(to), second(from), real(real), func(func) {}
			bool operator<(const EdgeHolder &e) const {
				if(first != e.first) return first < e.first;
				if(second != e.second) return second < e.second;
				return real;
			}
			bool operator==(const EdgeHolder &e) const {
				return (first==e.first && second==e.second);
			}
			unsigned first, second;
			bool real;
			InputGraph::VDF func;
		};
		
		void addEdge(EdgeHolder &e) {
			forwardStructure.at(e.second).push_back(static_cast<unsigned>(forwardStorage.size()));
			backwardStorage.push_back(BackwardGraphEdge(e.func(0.0), &nodeStorage.at(e.second)));
			forwardStorage.push_back(ForwardGraphEdge(e.func, &nodeStorage.at(e.first)));
		}
		
		unsigned edge(long from, long to) {
			//std::cout << "Looking for edge (" << from << ", " << to << ")" << std::endl;
			
			//std::cout << "indices: " << edgeStructure.at(to) << ", " << edgeStructure.at(to+1) << std::endl;
			
			for(unsigned i = edgeStructure.at(to); i != edgeStructure.at(to+1); ++i) {
			//	std::cout << "\t have edge (" << (backwardStorage.at(i).fromNode()-&nodeStorage[0]) << ", " << to << ")" << std::endl;
				if(backwardStorage.at(i).fromNode()-&nodeStorage[0] == from) {
					return (i);
				}
			}
			std::cerr << "Can't find edge (" << from << ", " << to << ")" << std::endl;
			throw "Edge does not exist";
		}//Not worth doing a binary search because traffic networks are so sparse
		
		void getEdgeList(std::vector<EdgeHolder>&, const InputGraph &);

	public:
		/**
		 * ABGraph constructor. Does some minor heavy lifting, setting
		 * up storage and edge inverses from the InputGraph.
		 */
		ABGraph(const InputGraph& g);

		/**
		 * Simple structure query, returns index of edges between two nodes.
		 */
		ForwardGraphEdge& forwardEdge(unsigned index) {
			return forwardStorage[index];
		}
		BackwardGraphEdge& backwardEdge(unsigned index) {
			return backwardStorage[index];
		}
		
		std::vector<BackwardGraphEdge>::iterator edgesFrom(unsigned index) {
			return backwardStorage.begin()+edgeStructure.at(index);
		}
		
		/**
		 * Gets the number of vertices in the graph.
		 */
		std::size_t numVertices() const { return forwardStructure.size(); }
		/**
		 * Gets the number of edges in the graph.
		 */
		unsigned numEdges() const { return numberOfEdges; }

		/**
		 * Returns an EdgeIterator pointing to the "first" out-edge
		 * of the "first" vertex of the ABGraph.
		 */
		std::vector<ForwardGraphEdge>::iterator begin() {
			return forwardStorage.begin();
		}

		/**
		 * Returns an EdgeIterator pointing just past the "last"
		 * out-edge of the "last" vertex of the ABGraph.
		 */
		std::vector<ForwardGraphEdge>::iterator end() {
			return forwardStorage.end();
		}
		
		BackwardGraphEdge* backward(const ForwardGraphEdge* f) {
			return &backwardStorage[f-&forwardStorage[0]];
		}
		ForwardGraphEdge* forward(const BackwardGraphEdge* b) {
			return &forwardStorage[b-&backwardStorage[0]];
		}
		
		//TODO: Add param info, have it return a good topo order (visited).
		void dijkstra(unsigned origin, std::vector<long>& distances, std::vector<unsigned>& order);
		
		/**
		 * Returns the total user travel time in the current solution.
		 */
		double currentCost() const {
			double cost = 0.0;
			std::vector<ForwardGraphEdge>::const_iterator i = forwardStorage.begin();
			std::vector<BackwardGraphEdge>::const_iterator j = backwardStorage.begin();
			for(; i != forwardStorage.end(); ++i, ++j)
				if(i->getFlow() != 0) cost += i->getFlow()*j->distance();
				//0 flow could mean imaginary arc, in which case 0*infinity = NaN.
			return cost;
		}
		
		/**
		 * Node storage access.
		 */
		//FIXME: Is this used? It seems kinda bad for it to be public, non-const.
		std::vector<BushNode>& nodes() { return nodeStorage; }
		
		friend std::ostream& operator<<(std::ostream& o, ABGraph & g) {
			o << "<NUMBER OF NODES> \t" << g.nodeStorage.size()<<std::endl;
			o << "<NUMBER OF LINKS> \t" << g.backwardStorage.size()<<std::endl;
			o << "<END OF METADATA>\t\t\n\n\n";
			o << "~ \tTail \tHead \t: \tVolume \tCost \t; \n";
			
			for(unsigned i = 0; i < g.forwardStructure.size(); ++i) {
				for(unsigned j=0; j < g.forwardStructure[i].size(); ++j) {
					BackwardGraphEdge& bEdge = g.backwardStorage[g.forwardStructure[i][j]];
					ForwardGraphEdge& fEdge = g.forwardStorage[g.forwardStructure[i][j]];
					
					o << "\t" <<i+1<<" \t"<<fEdge.toNode()-&g.nodeStorage.front()+1<<" \t: \t"<<fEdge.getFlow()<<" \t" << bEdge.distance() <<" \t; \n";
				}
			}
			o.flush();
			return o;
		}
};

#endif
