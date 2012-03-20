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


#include "HornerPolynomial.hpp"
#include <algorithm>
#include <functional>

double HornerPolynomial::operator()(double x) const
{
	double result = 0;
	for(std::vector<double>::const_reverse_iterator i = coeffs.rbegin(); i != coeffs.rend(); ++i) {
		result *= x;
		result += (*i);
	}
	return result;
}

void HornerPolynomial::operator+=(const HornerPolynomial& p)
{
	if (p.coeffs.size() > coeffs.size())
		coeffs.resize(p.coeffs.size());
	std::transform(coeffs.begin(), coeffs.end(), p.coeffs.begin(), coeffs.begin(), std::plus<double>());
}

void HornerPolynomial::operator-=(const HornerPolynomial& p)
{
	if (p.coeffs.size() > coeffs.size())
		coeffs.resize(p.coeffs.size());
	std::transform(coeffs.begin(), coeffs.end(), p.coeffs.begin(), coeffs.begin(), std::minus<double>());
}

void HornerPolynomial::multiplyX(double d)
{
	double multiple = 1.0;
	for(std::vector<double>::iterator i = coeffs.begin(); i != coeffs.end(); ++i) {
		*i *= multiple;
		multiple *= d;
	}
}

void HornerPolynomial::operator*=(double d)
{
	for(std::vector<double>::iterator i = coeffs.begin(); i != coeffs.end(); ++i)
		*i *= d;
}
void HornerPolynomial::operator+=(double d)
{
	if(coeffs.size() < 1) coeffs.resize(1);
	coeffs.at(0) += d;
}

void HornerPolynomial::shiftXInc(double d)
{
	//There are some clever ways to do this.
	//Literature review should be done.
	//Specifically: "Fast algorithms for Taylor shifts and certain difference equations" by Joachim von zur Gathen and Jurgen Gerhard
	
	//Note: This does ax^n --> (ax + d)^n, not a(x+d)^n.
	if(d == 0) return;
	std::vector<double> dPowers(coeffs.size(), 1.0);
	for(std::vector<double>::iterator i = dPowers.begin()+1; i < dPowers.end(); ++i)
		(*i)=(*(i-1))*d;
	
	for(unsigned i = 1; i < coeffs.size(); ++i) {
		//Dealing with power i
		double currentCoeff = coeffs.at(i);
		double rollingCoeff = 1.0;
		if(currentCoeff==0) continue;//Most of our polynomials likely have few terms in x.
		unsigned pascalsCoeff = 1;
		for(unsigned j = 0; j < i; ++j) {
			//modifying coefficient in place j
			coeffs.at(j) += rollingCoeff * pascalsCoeff * dPowers.at(i-j);
			pascalsCoeff = (pascalsCoeff*(i-j))/(j+1);
			rollingCoeff *= currentCoeff;
			//See http://en.wikipedia.org/wiki/Pascal's_Triangle#Calculating_an_individual_row
		}
		coeffs.at(i) = rollingCoeff;
	}
}
void HornerPolynomial::shiftX(double d)
{
	//There are some clever ways to do this.
	//Literature review should be done.
	//Specifically: "Fast algorithms for Taylor shifts and certain difference equations" by Joachim von zur Gathen and Jurgen Gerhard
	
	//Note: This does ax^n --> a(x+d)^n.
	if(d == 0) return;
	std::vector<double> dPowers(coeffs.size(), 1.0);
	for(std::vector<double>::iterator i = dPowers.begin()+1; i < dPowers.end(); ++i)
		(*i)=(*(i-1))*d;
	
	for(unsigned i = 1; i < coeffs.size(); ++i) {
		//Dealing with power i
		double currentCoeff = coeffs.at(i);
		if(currentCoeff==0) continue;//Most of our polynomials likely have few terms in x.
		unsigned pascalsCoeff = 1;
		for(unsigned j = 0; j < i; ++j) {
			//modifying coefficient in place j
			coeffs.at(j) += currentCoeff * pascalsCoeff * dPowers.at(i-j);
			pascalsCoeff = (pascalsCoeff*(i-j))/(j+1);
			//See http://en.wikipedia.org/wiki/Pascal's_Triangle#Calculating_an_individual_row
		}
	}
}
