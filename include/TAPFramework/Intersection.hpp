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


#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <list>
#include <boost/graph/adjacency_list.hpp>

namespace TAPFramework {

class GraphImporter;

/**
 * TAPFramework's storage class for origin-destination data. This class
 * simply stores a list of paired destination node ids and flow demands.
 */

class Intersection
{
	public:
		Intersection() {}
		Intersection(const std::list<std::pair<unsigned,double> >& destinations) :
			destinations(destinations) {}
		const std::list<std::pair<unsigned ,double> >& dests() const { return destinations; }
		friend class GraphImporter;
		//FIXME: Shouldn't need friends.
		
	protected:
		std::list<std::pair<unsigned,double> > destinations;
};

}

#endif
