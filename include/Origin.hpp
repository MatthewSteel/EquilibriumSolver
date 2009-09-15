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


//ODPair.hpp

#ifndef ORIGIN_HPP
#define ORIGIN_HPP

#include <utility>
#include <ostream>
#include <vector>

class Origin
{
	public:
		Origin(int);
		
		int getOrigin() const { return origin; }
		void addDestination(int, double);
		
		friend std::ostream& operator<<(std::ostream& o, Origin & p) {
			o << "Origin["<<p.origin<<",some destinations]";
			return o;
		}
		const std::vector<std::pair<int,double> >& dests() const { return destinations; }
	protected:
		int origin;
		std::vector<std::pair<int,double> > destinations;
};

#endif
