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


#ifndef ADDER_HPP
#define ADDER_HPP

#include "Equation.hpp"

/**
 * Abstract class to represent Ts (Equations) that can be added to things. I'm
 * pretty sure I wrote this class in here when I didn't understand how C++
 * really worked, and it's probably not strictly necessary.
 */
template <typename T>
class Adder : public Equation
{
	public:
		virtual ~Adder() {}

		/**
		 * Adds another T to the list of things to add up.
		 */
		virtual void operator+=(const T&) = 0;
};

#endif
