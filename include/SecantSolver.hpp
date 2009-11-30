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


#ifndef SECANT_SOLVER_HPP
#define SECANT_SOLVER_HPP

/**
 * Simple little class used to find the roots of Equations by solving them
 * using the secant method. Tends to be faster than the bisection and
 * Regula-Falsi methods for what I'm doing, and doesn't seem to veer off
 * into nowhere for no reason. (Bonus!)
 */

//TODO: Make iterationLimit a template parameter.
template<typename T>
class SecantSolver
{
	public:
		SecantSolver(unsigned=25);
		double solve(const T&, double=0.0, double=1.0);
	private:
		unsigned iterationLimit;
};

template<typename T>
SecantSolver<T>::SecantSolver(unsigned iterations):iterationLimit(iterations) {}

template<typename T>
double SecantSolver<T>::solve(const T& p, double second, double first)
{
	double lambda = first;
	double firstEval = p(first);
	if(firstEval > 0) return lambda;
	double secondEval = p(second);
	if(secondEval < 0) return second;
	
	for(unsigned i = 0; i < iterationLimit && (secondEval-firstEval > 1e-18 || secondEval-firstEval < -1e-18); ++i) {
		lambda = first - firstEval*((second-first)/(secondEval-firstEval));
		first = second;
		firstEval = secondEval;
		second = lambda;
		secondEval = p(second);
	}
	return lambda;
}

#endif
