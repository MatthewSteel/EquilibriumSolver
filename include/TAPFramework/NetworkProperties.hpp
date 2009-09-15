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


#ifndef NETWORK_PROPERTIES_HPP
#define NETWORK_PROPERTIES_HPP

namespace TAPFramework
{

/**
 * A simple storage class for arc cost parameters not represented in the
 * BPR Function. Specifically, this class stores costs-per-unit of road
 * tolls and lengths, where a "unit" refers to whatever unit is used in
 * the network files used.
 */

class NetworkProperties
{
	public:
		NetworkProperties(double lengthCost = 0.0, double tollCost = 0.0) : lengthCost(lengthCost), tollCost(tollCost) {}
		double getLengthCost() const { return lengthCost; }
		double getTollCost() const { return tollCost; }
	private:
		double lengthCost;
		double tollCost;
};

}

#endif
