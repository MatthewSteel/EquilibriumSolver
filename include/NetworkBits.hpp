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


#ifndef NETWORK_BITS_HPP
#define NETWORK_BITS_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <ostream>
#include "BPRFunction.hpp"
#include "HornerPolynomial.hpp"
#include "TAPFramework/Road.hpp"
#include "TAPFramework/NetworkProperties.hpp"

class EdgeStuff
{
	public:
		EdgeStuff(const TAPFramework::Road&, const TAPFramework::NetworkProperties&);
		EdgeStuff(const EdgeStuff& e);

		double distance;
		//Ugh, think distance needs to be public for BGL, will put in a request maybe.
		
		void tempFlow(double);
		void update(double=1.0);
		double getFlow() { return flow; }
		double getAugmentingFlow() { return secondFlow; }
		std::auto_ptr<HornerPolynomial> integralFunction() const { return distanceFunction.integralFunction(flow, secondFlow); }
		std::auto_ptr<HornerPolynomial> costFunction() const { return distanceFunction.costFunction(); }
		friend std::ostream& operator<<(std::ostream& o, EdgeStuff& e) {
			o << "EdgeStuff(flow:"<< e.flow << ",func:" << e.distanceFunction << ", cost:"<<e.distance<<")";
			return o;
		}
	private:
		double flow;
		double secondFlow;
		BPRFunction distanceFunction;
};

struct EdgeHolder
{
	//A bit of a hack..
	//Makes predecessor recording in Dijkstra's algorithm faster.
	mutable boost::graph_traits<boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> >::edge_descriptor previousEdge;
	//Dijkstra visitor wants to be const, so we wrap this up and make it mutable...
};

#endif
