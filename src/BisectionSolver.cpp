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


#include "BisectionSolver.hpp"
#include <iostream>
#include "HornerPolynomial.hpp"
BisectionSolver::BisectionSolver(unsigned iterations):iterationLimit(iterations) {}

double BisectionSolver::solve(const Equation& p, double lower, double upper)
{
	double lowerEval = p(lower), upperEval = p(upper);
	
	if ((upperEval >= 0) == (lowerEval >= 0)) {
		std::cerr << (HornerPolynomial&)p << std::endl;
		return (lowerEval < upperEval)?lower:upper;
	}
	
	double lambda=lower, lambdaEval;
	
	for(unsigned i = 0; i < iterationLimit; ++i) {
		lambda = (lower + upper)/2;
		lambdaEval = p(lambda);
		if ((upperEval >= 0) == (lambdaEval >= 0)) {
			upper = lambda;
			upperEval = lambdaEval;
		} else {
			lower = lambda;
			lowerEval = lambdaEval;
		}
	}
	return lambda;
}
