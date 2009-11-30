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
#include "BarGeraImporter.hpp"
#include "InputGraph.hpp"

using namespace std;

void general(const char* netString, const char* tripString, double distanceFactor=0.0, double tollFactor=0.0, double gap = 1e-14)
{
//	TAPFramework::GraphImporter gi;
//	TAPFramework::NetworkProperties p(distanceFactor, tollFactor);
	ifstream network(netString), trips(tripString);
	BarGeraImporter bgi(distanceFactor, tollFactor);
	InputGraph ig;
	boost::timer timer3;
	bgi.readInGraph(ig, network, trips);
	cout << timer3.elapsed() << endl;
	//boost::shared_ptr<Graph> g = gi.readInGraph(network, trips);

	boost::timer timer;

	AlgorithmBSolver abs(ig);
	double time=0.0;
	cout << (time += timer.elapsed()) << endl;//*/
/*	double d = 0.1;
	for(int i = 0; i < 6; ++i) {
		boost::timer t2;
		cout << d << endl;
		abs.solve(10000, d);
		time += t2.elapsed();
		cout << time << ' ' <<abs.relativeGap() << endl;
		d/=100;
	}
*/
	double thisGap;
	for(thisGap = abs.relativeGap(); thisGap > gap; thisGap = abs.relativeGap()) {
		cout << time << ' ' << thisGap << endl;
		boost::timer t2;
		//cout << i << endl;
		abs.solve(12);
		time += t2.elapsed();
	}
	cout << time << ' ' << thisGap << endl;
//	cout << abs << endl;
}

int main (int argc, char **argv)
{
//	general("networks/ChicagoSketch_net.txt", "networks/ChicagoSketch_trips.txt", 0.04, 0.02);
	general("networks/Braess_net.txt", "networks/Braess_trips.txt");
//	general("networks/Auckland_net2.txt", "networks/Auckland_trips.txt");
	general("networks/SiouxFalls_net.txt", "networks/SiouxFalls_trips.txt");
	general("networks/Anaheim_net.txt", "networks/Anaheim_trips.txt");
//	general("networks/ChicagoRegional_net.txt", "networks/ChicagoRegional_trips.txt", 0.25, 0.1, 1e-5);
//	general("networks/Philadelphia_network.txt", "networks/Philadelphia_trips.txt", 0.0, 0.055, 1e-4);//Known BUG: Doesn't quite work just yet.
}
