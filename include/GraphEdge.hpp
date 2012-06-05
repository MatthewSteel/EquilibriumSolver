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

class BackwardGraphEdge
{
	public:
		BackwardGraphEdge() : from(0), _distance(0.0) {}
		BackwardGraphEdge(double, BushNode *from);
		BackwardGraphEdge(const BackwardGraphEdge & e);

		double distance() const { return _distance; }
		void setDistance(double d) { _distance = d; }
		BushNode* fromNode() { return from; }
	private:
		friend class ForwardGraphEdge;
		BushNode* from;
		double _distance;
};

class ForwardGraphEdge
{
	public:
		ForwardGraphEdge() : inverse(0), to(0), flow(0.0) {}

		ForwardGraphEdge(InputGraph::VDF, BushNode* to, BackwardGraphEdge* inverse=0);
		ForwardGraphEdge(const ForwardGraphEdge& e);
		
		const InputGraph::VDF* costFunction() const { return &distanceFunction; }

		BushNode* toNode() { return to; }

		void setInverse(BackwardGraphEdge* ge) { inverse = ge; }
		BackwardGraphEdge* getInverse() { return inverse; }
		void addFlow(double d) {
			flow += d;
		}
		double getFlow() const { return flow; }
	private:
		InputGraph::VDF distanceFunction;//Dang, 32 bytes in GCC!?
		BackwardGraphEdge* inverse;
		BushNode* to;
		double flow;
};

#endif
