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


#ifndef BPR_FUNCTION_HPP
#define BPR_FUNCTION_HPP

#ifdef _MSC_VER
 #include <functional>
#elif defined __PATHCC__
 #include <boost/tr1/functional.hpp>
#else
 #include <tr1/functional>
#endif

#include <vector>
#include <cmath>
#include "HornerPolynomial.hpp"
/**
 * Our BPR Function class. Reasonably clever, doing things with the Horner
 * scheme when available.
 */

class BarGeraBPRFunction {

	public:
		/**
		 * TODO
		 */
		BarGeraBPRFunction(double zeroFlowTime, double practicalCapacity, double alpha, double beta, double extraCost);
		
		/**
		 * TODO
		 */
		const std::tr1::function<double(double)> costFunction() const { return _costFunction; }

	private:
		std::tr1::function<double(double)> _costFunction;
		class BPREval {
			public:
				BPREval(double zeroFlowTime, double practicalCapacity, double alpha, double beta, double extraCost) :
				zeroFlowTime(zeroFlowTime), practicalCapacity(practicalCapacity), alpha(alpha), beta(beta), extraCost(extraCost) {}
				double operator()(double d) {
					return extraCost + zeroFlowTime*(1+alpha*std::pow(d/practicalCapacity,beta));
				}
			private:
				double zeroFlowTime, practicalCapacity, alpha, beta, extraCost;
		};
};

BarGeraBPRFunction::BarGeraBPRFunction(double zeroFlowTime, double practicalCapacity, double alpha, double beta, double extraCost)
{
	if(std::floor(beta) == beta && beta >= 0) {
		std::vector<double> polynomial((long)(beta+1.0));
		polynomial.at(static_cast<unsigned>(beta)) = 1;
		HornerPolynomial hp = HornerPolynomial(polynomial);
		hp.multiplyX(1/practicalCapacity);
		hp *= alpha;
		hp += 1;
		hp *= zeroFlowTime;
		hp += extraCost;
		_costFunction = hp;
	} else {
		_costFunction = BPREval(zeroFlowTime, practicalCapacity, alpha, beta, extraCost);
	}
}

#endif
