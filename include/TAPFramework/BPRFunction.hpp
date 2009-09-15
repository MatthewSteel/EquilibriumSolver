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

#ifndef TAP_BPRFUNCTION_HPP
#define TAP_BPRFUNCTION_HPP

namespace TAPFramework
{

/**
 * TAPFramework's storage class for BPR Function data. This class does
 * not provide for the evaluation of the function. This class is
 * immutable.
 */
class BPRFunction
{
	public:
		/**
		 * Default constructor.
		 * @param zeroFlowTime The time to travel along a link in zero traffic
		 * @param practicalCapacity The practical capacity of the link (FIXME: - I'm not a traffic engineer!)
		 * @param alpha A fitting parameter in the BPR Function. May have some real-world significance?
		 * @param beta A fitting parameter in the BPR Function (the exponent). May have some real-world significance?
		 */
		BPRFunction(double zeroFlowTime=0, double practicalCapacity=0, double alpha = 0.15, unsigned beta = 4) :
			zeroFlowTime(zeroFlowTime),
			practicalCapacity(practicalCapacity),
			alpha(alpha),
			beta(beta) {}
		
		/**
		 * Simple accessor method for the zeroFlowTime member.
		 * @return The time to travel along a link in zero traffic
		 */
		double getZeroFlowTime() const { return zeroFlowTime; }
		/**
		 * Simple accessor method for the practicalCapacity member.
		 * @return The practical capacity of the link
		 */
		double getPracticalCapacity() const { return practicalCapacity; }
		/**
		 * Simple accessor method for the alpha member.
		 * @return The alpha parameter
		 */
		double getAlpha() const { return alpha; }
		/**
		 * Simple accessor method for the beta member.
		 * @return The beta parameter
		 */
		unsigned getBeta() const { return beta; }
	private:
		double zeroFlowTime;
		double practicalCapacity;
		double alpha;
		unsigned beta;
};

}
#endif
