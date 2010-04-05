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


#ifndef GRAPH_EDGE_HPP
#define GRAPH_EDGE_HPP

#include <ostream>
#include <vector>
#include <limits>
#include "InputGraph.hpp"

class BushNode;

/*
Split edges for cache gains. Should bring GraphEdges from 4.32MB total in CR's
BuildTrees to ~1.5MB
*/

class ForwardGraphEdge
{
	public:
		ForwardGraphEdge(InputGraph::VDF, BushNode *to);
		ForwardGraphEdge(const ForwardGraphEdge & e);

		unsigned getToId();
		double distance() const { return _distance; }
		void setDistance(double d) { _distance = d; }
		BushNode* toNode() { return to; }
	private:
		friend class BackwardGraphEdge;
		BushNode* to;
		double _distance;
};

class BackwardGraphEdge
{
	public:
		BackwardGraphEdge(InputGraph::VDF, BushNode* from, ForwardGraphEdge* inverse);
		BackwardGraphEdge(const BackwardGraphEdge& e);
		
		const InputGraph::VDF* costFunction() const { return &distanceFunction; }

		BushNode* fromNode() { return from; }
		unsigned getToId() const { return toId; }

		void setInverse(ForwardGraphEdge* ge) { inverse = ge; }
		ForwardGraphEdge* getInverse() { return inverse; }
		void addFlow(double d) {
			flow += d;
		}//TODO: add dist to FGE?
		double getFlow() const { return flow; }
		friend std::ostream& operator<<(std::ostream& o, BackwardGraphEdge& e);
	private:
		InputGraph::VDF distanceFunction;//Dang, 32 bytes in GCC!?
		ForwardGraphEdge* inverse;
		BushNode* from;
		double flow;
		unsigned toId;
		
};

#endif
