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


#include "RegulaFalsiSolver.hpp"

RegulaFalsiSolver::RegulaFalsiSolver(unsigned iterations) : iterationLimit(iterations) {}

double RegulaFalsiSolver::solve(const Equation& p, double lower, double upper)
{
	double lambda, lambdaEval;
	double lowerEval = p(lower), upperEval = p(upper);
	
	if ((upperEval >= 0) == (lowerEval >= 0)) return lowerEval < upperEval?lower:upper;
	//Hmm. Think about what to do about the symmetry issue here and further down.
	
	for(unsigned i = 0; i < iterationLimit && (lowerEval-upperEval > 1e-18 || lowerEval-upperEval < -1e-18); ++i) {
		lambda = lower + (upper-lower)*(lowerEval/(lowerEval - upperEval));
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
