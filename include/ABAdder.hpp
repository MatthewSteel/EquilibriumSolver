#ifndef AB_ADDER_HPP
#define AB_ADDER_HPP

#include <utility>
#ifdef _MSC_VER
 #include <functional>
#elif defined __PATHCC__
 #include <boost/tr1/functional.hpp>
#else
 #include <tr1/functional>
#endif

class ABAdder
{
	typedef const std::tr1::function<double(double)>* func;
	public:
		ABAdder(unsigned long addNum, unsigned long subtractNum) {
			add.reserve(addNum);
			subtract.reserve(subtractNum);
		}
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
