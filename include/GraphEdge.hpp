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

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <ostream>
#include <vector>
#include <limits>
#include "BPRFunction.hpp"
#include "HornerPolynomial.hpp"
#include "TAPFramework/Road.hpp"
#include "TAPFramework/NetworkProperties.hpp"

class BushNode;

class GraphEdge
{
	public:
		GraphEdge(const TAPFramework::Road&, const TAPFramework::NetworkProperties&, BushNode&, BushNode&, unsigned);
		GraphEdge(const GraphEdge& e);
		GraphEdge(BushNode& from, BushNode& to, unsigned toId) : distance(std::numeric_limits<double>::infinity()), to(&to), inverse(0), flow(0), distanceFunction(HornerPolynomial(std::vector<double>(1,std::numeric_limits<double>::infinity()))), from(&from), toId(toId) {}
		//Ugh, think distance needs to be public for BGL, will put in a request maybe.
		
		double distance;
		
		void addFlow(double d) { flow += d; distance = distanceFunction(flow); }
		double getFlow() const { return flow; }
		std::auto_ptr<HornerPolynomial> costFunction() const { return std::auto_ptr<HornerPolynomial>(new HornerPolynomial(distanceFunction)); }

		BushNode* toNode() { return to; }
		BushNode* fromNode() { return from; }
		unsigned getToId() { return toId; }

		friend std::ostream& operator<<(std::ostream& o, GraphEdge& e) {
			o << "EdgeStuff(flow:"<< e.flow << ",func:" << e.distanceFunction << ", cost:"<<e.distance<<")";
			return o;
		}

		void setInverse(GraphEdge* ge) { inverse = ge; }
		GraphEdge* getInverse() { return inverse; }
	private:
		BushNode* to;
		GraphEdge* inverse;
		double flow;
		HornerPolynomial distanceFunction;
		BushNode* from;
		unsigned toId;
};

#endif
