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


#ifndef HORNER_POLYNOMIAL_HPP
#define HORNER_POLYNOMIAL_HPP

#include <vector>
#include <ostream>

class HornerPolynomial
{
	private:
		std::vector<double> coeffs;
	public:
		HornerPolynomial() {}
		HornerPolynomial(const std::vector<double>& coeffs) : coeffs(coeffs) {}
		HornerPolynomial(const HornerPolynomial& h) : coeffs(h.coeffs) {}//Maybe remove? Can't remember why I did before...
		double operator()(double x) const;
		void operator+=(const HornerPolynomial&);
		void operator-=(const HornerPolynomial&);
		void operator*=(double);
		void operator+=(double);
		void shiftX(double);
		void shiftXInc(double);
		void multiplyX(double);
		friend std::ostream& operator<<(std::ostream& o, HornerPolynomial & e)
		{
			o << "Horner Polynomial:";
			unsigned u = 0;
			for(std::vector<double>::iterator i = e.coeffs.begin(); i != e.coeffs.end(); ++i)
				o << " + " << (*i) << "x^" << u++;
			return o;
		}//FIXME
		
};

#endif
