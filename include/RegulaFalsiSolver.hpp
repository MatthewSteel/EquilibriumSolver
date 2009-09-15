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


#ifndef REGULA_FALSI_SOLVER_HPP
#define REGULA_FALSI_SOLVER_HPP

#include "Equation.hpp"

class RegulaFalsiSolver
{
	public:
		RegulaFalsiSolver(unsigned = 25);
		double solve(const Equation&, double=0.0, double=1.0);
	private:
		unsigned iterationLimit;
};

#endif

