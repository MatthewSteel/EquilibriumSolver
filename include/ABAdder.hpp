#ifndef AB_ADDER_HPP
#define AB_ADDER_HPP

#include <utility>
#include <tr1/functional>

class ABAdder
{
	typedef const std::tr1::function<double(double)>* func;
	public:
		double operator()(double d) const {
			double ret = 0;
			for(std::vector<std::pair<func, double> >::const_iterator i = add.begin(); i != add.end(); ++i)
				ret += (*(i->first))(i->second + d);
			for(std::vector<std::pair<func, double> >::const_iterator i = subtract.begin(); i != subtract.end(); ++i)
				ret -= (*(i->first))(i->second - d);
			return ret;
		}
		void operator+=(std::pair<func, double> p) {
			add.push_back(p);
		}
		void operator-=(std::pair<func, double> p) {
			subtract.push_back(p);
		}
	private:
		std::vector<std::pair<func, double> > add;
		std::vector<std::pair<func, double> > subtract;
};

#endif
