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


#ifndef ROAD_HPP
#define ROAD_HPP

#include "TAPFramework/BPRFunction.hpp"

namespace TAPFramework {

/**
 * TAPFramework's storage class for arc data. This class simply stores a
 * BPR function, a length, a toll and a flow.
 */

class Road
{
	public:
		Road(const TAPFramework::BPRFunction& function = TAPFramework::BPRFunction(), double length = 0.0, double toll = 0.0, double flow = 0.0) :
			function(function), length(length), toll(toll), flow(flow) {}
		const TAPFramework::BPRFunction& getBPRFunction() const { return function; }
		double getLength() const { return length; }
		double getFlow() const { return flow; }
		double getToll() const { return toll; }
		void updateFlow(double f) { flow = f; }
		void addFlow(double f) { flow += f; }
	private:
		TAPFramework::BPRFunction function;
		double length;
		double toll;
		double flow;
};

}

#endif
