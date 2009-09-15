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


#include "BPRFunction.hpp"
#include <cmath>
#include <iostream>

BPRFunction::BPRFunction(const TAPFramework::BPRFunction& b):
	zeroFlowTime(b.getZeroFlowTime()),
	practicalCapacity(b.getPracticalCapacity()),
	alpha(b.getAlpha()),
	beta(b.getBeta()),
	extraCost(0)
{
	std::vector<double> polynomial(beta+1);
	polynomial.at(beta) = 1;
	hp = HornerPolynomial(polynomial);
	hp.multiplyX(1/practicalCapacity);
	hp *= alpha;
	hp += 1;
	hp *= zeroFlowTime;
}

BPRFunction::BPRFunction(const BPRFunction& b) :
		zeroFlowTime(b.zeroFlowTime),
		practicalCapacity(b.practicalCapacity),
		alpha(b.alpha),
		previousFlow(b.previousFlow),
		previousDistance(b.previousDistance),
		beta(b.beta),
		hp(b.hp),
		extraCost(b.extraCost)
{}

/*
BPRFunc(flow) = T_0 * (1 + alpha(flow/capacity)^beta)

Z = integral(BPRFunc(flow), flow)

Z = T_0 * (flow + alpha*capacity/(beta+1)(flow/capacity)^(beta+1)

Z = T_0 * (lambda*new + (1-lambda)*old) + (alpha*capacity)/(beta+1)*((lambda*new + (1-lambda)*old)/capacity)^(beta+1)

dZ/dlambda = T_0 * ((new - old) + (new-old)*alpha((lambda*new + (1-lambda)*old)/capacity)^beta)
dZ/dlambda = T_0 * (new-old) * (1 + alpha*((lambda*new + (1-lambda)*old)/capacity)^beta)


(zft * (n-o)) + (zft * (n-o))*alpha*((o + x(n-o))/p)^4
= a + b*(c+dx)^4
= a + b*(c^2 + 2cdx + d^2x^2)^2
= a + b*(c^4 + 4c^3dx + 6c^2d^2x^2 + 4cd^3x^3 + d^4x^4)

= ((a + bc^4) + x(4c^3d + x(6c^2d^2 + x(4cd^3 + x(d^4)))))

 a = (zft + (n-o));
 b = a*alpha
 c = o/p
 d = (n-o)/p
*/
std::auto_ptr<HornerPolynomial> BPRFunction::integralFunction(double oldFlow, double newFlow) const
{
	double a = zeroFlowTime * (newFlow-oldFlow);
	double b = a * alpha;
	double c = oldFlow / practicalCapacity;
	double d = (newFlow-oldFlow)/practicalCapacity;
	
	std::vector<double> polynomial(beta+1);
	//polynomial is: a + b*(dx+c)^beta.
	
	polynomial.at(beta) = d;//hp = dx^beta
	std::auto_ptr<HornerPolynomial> hp(new HornerPolynomial(polynomial));
	hp->shiftXInc(c);//hp = (dx+c)^beta
	(*hp)*=b;//hp = b(dx+c)^beta
	(*hp)+=(a+(newFlow-oldFlow)*extraCost);//hp = a+b(dx+c)^beta
	
	return hp;
}

std::auto_ptr<HornerPolynomial> BPRFunction::costFunction() const
{
	return std::auto_ptr<HornerPolynomial>(new HornerPolynomial(hp));
}
