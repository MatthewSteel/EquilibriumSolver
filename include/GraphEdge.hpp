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
#include "HornerPolynomial.hpp"
#include "InputGraph.hpp"

class BushNode;

/*
Split edges for cache gains. Should bring GraphEdges from 4.32MB total in CR's
BuildTrees to ~1.5MB
*/

class ForwardGraphEdge
{
	friend class BackwardGraphEdge;
	public:
		ForwardGraphEdge(InputGraph::VDF, BushNode& to);
		ForwardGraphEdge(const ForwardGraphEdge & e);
		ForwardGraphEdge(BushNode& to);

		unsigned getToId() { return to->getId(); }
		double distance() { return _distance; }
		double getFlow() const { return flow; }
		BushNode* toNode() { return to; }
	private:
		void addFlow(double d) { flow += d; _distance = distanceFunction(flow); }
		
		BushNode* to;
		double _distance;
		double flow;
};

class BackwardGraphEdge
{
	public:
		BackwardGraphEdge(InputGraph::VDF, BushNode& from);
		BackwardGraphEdge(const BackwardGraphEdge& e);
		BackwardGraphEdge(BushNode& from);

		//Forward to appropriate handler
		void addFlow(double d) { forwardData.addFlow(d); }
		double getFlow() const { return forwardData.getFlow(); }
		
		const InputGraph::VDF* costFunction() const { return &distanceFunction; }

		BushNode* fromNode() { return from; }
		unsigned getToId() { return toId; }

		friend std::ostream& operator<<(std::ostream& o, GraphEdge& e) {
			o << "EdgeStuff(flow:"<< e.flow << ",func:" << e.distanceFunction << ", cost:"<<e.distance<<")";
			return o;
		}

		void setInverse(ForwardGraphEdge* ge) { inverse = ge; }
		ForwardGraphEdge* getInverse() { return inverse; }
	private:
		ForwardGraphEdge & forwardData;
		GraphEdge* inverse;
		InputGraph::VDF distanceFunction;//Dang, 32 bytes in GCC!?
		BushNode* from;
		unsigned toId;
};

#endif
