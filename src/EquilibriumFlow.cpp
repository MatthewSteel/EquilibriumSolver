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


#include <utility> //For Pair
#include <iostream>
#include <cstdlib> //For EXIT_SUCCESS

#include "AlgorithmBSolver.hpp"
#include "FrankWolfeSolver.hpp"
#include "TAPFramework/GraphImporter.hpp"
#include "TAPFramework/Tester.hpp"


using namespace std;

int main (int argc, char **argv)
{
	AlgorithmBSolver abs;
	TAPFramework::Tester t(abs);

//	TAPFramework::NetworkProperties p;
	//cout << (t.setGraph("networks/Auckland_net2.txt", "networks/Auckland_trips.txt", p)) << endl;
//	cout << (t.setGraph("networks/SiouxFalls_net.txt", "networks/SiouxFalls_trips.txt", p)) << endl;
//	cout << (t.setGraph("networks/Anaheim_net.txt", "networks/Anaheim_trips.txt", p)) << endl;

	TAPFramework::NetworkProperties p(0.04, 0.02);
	cout << (t.setGraph("networks/ChicagoSketch_net.txt", "networks/ChicagoSketch_trips.txt", p)) << endl;//*/
	//	cout << (t.setGraph("networks/TestFW1.txt", "networks/TestFW2.txt", p)) << endl;
/*	TAPFramework::NetworkProperties p(0.25, 0.1);
	t.setGraph("networks/ChicagoRegional_net.txt", "networks/ChicagoRegional_trips.txt", p);
	cout.precision(10);
	t.test(100000, 10);
	cout << "10 Done, testing"<<endl;
	cout << t.getRelativeGap() << " " << t.getTime() << " " << abs.getCount() << endl;
	t.test(100000, 1);
	cout << "1 Done, testing"<<endl;
	cout << t.getRelativeGap() << " " << t.getTime() << " " << abs.getCount() << endl;
	t.test(100000, 0.1);
	cout << "0.1 Done, testing"<<endl;
	cout << t.getRelativeGap() << " " << t.getTime() << " " << abs.getCount() << endl;
	exit(0);//*/

	double d = 0.1;
	for(int i = 0; i < 6; ++i) {
		cout << d << endl;
		t.test(100000, d);
		cout << t.getRelativeGap() << " " << t.getTime() << " " << abs.getCount() << endl;
		d/=100;
	}
	//abs.printBushes();
	//cout << abs << endl;
}
