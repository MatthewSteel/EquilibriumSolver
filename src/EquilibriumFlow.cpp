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
#include <fstream>
#include <cstdlib> //For EXIT_SUCCESS
#include <boost/timer.hpp>
#include <boost/shared_ptr.hpp>

#include "AlgorithmBSolver.hpp"
#include "TAPFramework/GraphImporter.hpp"

using namespace std;

typedef boost::adjacency_list<
		boost::vecS,
		boost::vecS,
		boost::bidirectionalS,
		TAPFramework::Intersection,
		TAPFramework::Road> Graph;

int main (int argc, char **argv)
{
	TAPFramework::GraphImporter gi;

//	TAPFramework::NetworkProperties p;
	//cout << (t.setGraph("networks/Auckland_net2.txt", "networks/Auckland_trips.txt", p)) << endl;
//	cout << (t.setGraph("networks/SiouxFalls_net.txt", "networks/SiouxFalls_trips.txt", p)) << endl;
//	cout << (t.setGraph("networks/Anaheim_net.txt", "networks/Anaheim_trips.txt", p)) << endl;
//	TAPFramework::NetworkProperties p(0.04, 0.02);
/*	ifstream network("networks/Anaheim_net.txt"), trips("networks/Anaheim_trips.txt");
	boost::shared_ptr<Graph> g = gi.readInGraph(network, trips);

	boost::timer timer;

	AlgorithmBSolver abs(g, p);
	double time=0.0;
	cout << (time += timer.elapsed()) << endl;//*/
	//	cout << (t.setGraph("networks/TestFW1.txt", "networks/TestFW2.txt", p)) << endl;
	TAPFramework::NetworkProperties p(0.25, 0.1);
	ifstream network("networks/ChicagoRegional_net.txt"), trips("networks/ChicagoRegional_trips.txt");
	boost::shared_ptr<Graph> g = gi.readInGraph(network, trips);
	double time = 0, tempTime = 0;

	boost::timer timer;
	AlgorithmBSolver abs(g, p);
	time += timer.elapsed() - tempTime;
	
	cout.precision(10);

	tempTime = timer.elapsed();
	abs.solve(100000, 10);
	time += timer.elapsed() - tempTime;
	
	cout << "10 Done, testing"<<endl;
	cout << abs.relativeGap() << " " << time << " " << abs.getCount() << endl;

	tempTime = timer.elapsed();
	abs.solve(100000, 1);
	time += timer.elapsed() - tempTime;

	cout << "1 Done, testing"<<endl;
	cout << abs.relativeGap() << " " << time << " " << abs.getCount() << endl;

	tempTime = timer.elapsed();
	abs.solve(100000, 0.1);
	time += timer.elapsed() - tempTime;

	cout << "0.1 Done, testing"<<endl;
	cout << abs.relativeGap() << " " << time << " " << abs.getCount() << endl;
	exit(0);//*/

	double d = 0.1;
	for(int i = 0; i < 6; ++i) {
		boost::timer t2;
		cout << d << endl;
		abs.solve(100000, d);
		time += t2.elapsed();
		cout << time << ' ' <<abs.relativeGap() << endl;
		d/=100;
	}
	//abs.printBushes();
	//cout << abs << endl;
}
