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

#ifdef _MSC_VER
 #include <functional>
#elif defined __PATHCC__
 #include <boost/tr1/functional.hpp>
#else
 #include <tr1/functional>
#endif

class ABGraph;
class BushNode;
class ForwardGraphEdge;
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
		BushEdge(BackwardGraphEdge* arc) :
			realEdge(arc), ownFlow(0) {}

		/**
		 * TODO
		 */
		BushEdge() :
			realEdge(0), ownFlow(0) {}

		/**
		 * TODO
		 */
		double length() const { return realEdge->distance(); }

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
		 * Turns the arc around. We assume a topological sort will
		 * occur after this, and no other bush data is affected.
		 * If the bush-specific flow is not zero the solution loses
		 * feasibility.
		 */
		void swapDirection(ABGraph &g);

		/**
		 * TODO
		 */
		void addFlow(double d, ForwardGraphEdge *fge) {
			ownFlow += d;
			fge->addFlow(d);
			realEdge->setDistance((*fge->costFunction())(fge->getFlow()));
		}
		
		BackwardGraphEdge* underlyingEdge() { return realEdge; }
	private:
		BackwardGraphEdge* realEdge;
		double ownFlow;
};

#endif
