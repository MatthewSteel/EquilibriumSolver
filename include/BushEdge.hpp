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


#ifndef BUSH_EDGE_HPP
#define BUSH_EDGE_HPP

#include <memory>
#include <algorithm>
#include <utility>

#include "HornerPolynomial.hpp"
#include "GraphEdge.hpp"
#include <tr1/functional>

class BushNode;

/**
 * A bush-specific edge structure that only exists so we can know the
 * bush-specific flow on the edge. Lots of handy functions, though...
 */
class BushEdge
{
	public:
		/**
		 * TODO
		 */
		BushEdge(ForwardGraphEdge* arc) :
			realEdge(arc), ownFlow(0) {}

		/**
		 * TODO
		 */
		BushEdge() :
			realEdge(0), ownFlow(0) {}

		/**
		 * TODO
		 */
		double length() const { return realEdge->distance; }

		/**
		 * TODO
		 */
		bool used() const { return ownFlow > 1e-10; }
		//BUG: Doesn't work on 1e-12 for known reason, need to fix.
		
		/**
		 * TODO
		 */
		double flow() const { return ownFlow; }

		/**
		 * TODO
		 */
		BushNode* fromNode() const { return realEdge->fromNode(); }

		/**
		 * TODO
		 */
		BushNode* toNode() const { return realEdge->toNode(); }

		/**
		 * TODO
		 */
		unsigned toNodeId() const { return realEdge->getToId(); }

		/**
		 * Turns the arc around. We assume a topological sort will
		 * occur after this, and no other bush data is affected.
		 * If the bush-specific flow is not zero the solution loses
		 * feasibility.
		 */
		void swapDirection(ABGraph &g) { realEdge = g.backward(realEdge)->getInverse(); }

		/**
		 * Gives the cost function of the BushEdge's corresponding edge
		 * in the underlying graph "shifted" by the edge's current
		 * flow. That is,
		 *     costFunction(0) = realEdge.costFunction(realEdge.flow)
		 */
		std::pair<const std::tr1::function<double(double)>*, double> costFunction() const {
			return std::make_pair(realEdge->costFunction(), realEdge->getFlow());
		}

		/**
		 * Returns the edge on the underlying graph from this edge's
		 * to-node to this edge's from-node.
		 */
		const ForwardGraphEdge* getInverse() const { return realEdge->getInverse(); }

		/**
		 * TODO
		 */
		void addFlow(double d) { realEdge->addFlow(d); ownFlow += d; }
	private:
		ForwardGraphEdge* realEdge;
		double ownFlow;
};

#endif
