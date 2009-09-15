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

#include <iostream>
#include <memory>

#include "HornerPolynomial.hpp"
#include "TAPFramework/BPRFunction.hpp"

/**
 * Our BPR Function class. Gives out polynomials when you need them, provides
 * some nice variants on the standard BPR function because Frank-Wolfe is dumb.
 * TODO: Document all of this.
 */

class BPRFunction {

	public:
		/**
		 * TODO
		 */
		BPRFunction(const TAPFramework::BPRFunction&);

		/**
		 * TODO
		 */
		BPRFunction(const BPRFunction&);
		
		/**
		 * TODO
		 */
		friend std::ostream& operator<<(std::ostream& o, BPRFunction& e)
		{
			o << "BPRFunction(" << e.previousFlow << ','
					<< e.previousDistance<<','
					<< e.zeroFlowTime << ","
					<< e.practicalCapacity << ","
					<< e.alpha << ","
					<< e.beta
					<< ")";
			return o;
		}

		/**
		 * TODO
		 */
		std::auto_ptr<HornerPolynomial> integralFunction(double, double) const;

		/**
		 * TODO
		 */
		std::auto_ptr<HornerPolynomial> costFunction() const;

		/**
		 * TODO
		 */
		double operator()(double x) const { return hp(x); }

		/**
		 * TODO
		 */
		void addCost(double d) { hp += d; extraCost += d; }
	private:
		double zeroFlowTime, practicalCapacity, alpha, previousFlow, previousDistance;
		unsigned beta;
		HornerPolynomial hp;
		double extraCost;
		//Prev flow, distance so we can save some time if it doesn't change in an iteration.
};

#endif
