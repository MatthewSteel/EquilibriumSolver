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


#ifndef BISECTION_SOLVER_HPP
#define BISECTION_SOLVER_HPP

#include "Equation.hpp"

/**
 * Simple little class used to find the roots of Equations by solving them
 * using the bisection method.
 */

//TODO: Make iterationLimit a template parameter.
class BisectionSolver
{
	public:
		/**
		 * TODO
		 */
		BisectionSolver(unsigned = 25);

		/**
		 * TODO
		 */
		double solve(const Equation&, double=0.0, double=1.0);
	private:
		unsigned iterationLimit;
};

#endif
