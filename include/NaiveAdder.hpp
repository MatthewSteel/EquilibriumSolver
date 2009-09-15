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


#ifndef NAIVE_ADDER_HPP
#define NAIVE_ADDER_HPP

#include <vector>

#include "Equation.hpp"
#include "Adder.hpp"

template<class T>
class NaiveAdder : public Adder<T>
{
	private:
		std::vector<T> eqs;
	public:
		double operator()(double) const;
		void operator+=(const T&);
};

template <class T>
double NaiveAdder<T>::operator()(double x) const
{
	double ret = 0;
	for(typename std::vector<T>::const_iterator i = eqs.begin(); i != eqs.end(); ++i)
		ret += (*i)(x);
	//Ehm, find out why I need to use "typename" here...
	return ret;
}

template <class T>
void NaiveAdder<T>::operator+=(const T& t)
{
	eqs.push_back(t);
}

#endif
