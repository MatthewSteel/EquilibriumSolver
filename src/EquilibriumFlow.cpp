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

void general(const char* netString, const char* tripString, double distanceFactor=0.0, double tollFactor=0.0, double gap = 1e-14)
{
	TAPFramework::GraphImporter gi;
	TAPFramework::NetworkProperties p(distanceFactor, tollFactor);
	ifstream network(netString), trips(tripString);
	boost::shared_ptr<Graph> g = gi.readInGraph(network, trips);

	boost::timer timer;

	AlgorithmBSolver abs(g, p);
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
	while(abs.relativeGap() > gap) {
		boost::timer t2;
		//cout << i << endl;
		abs.solve(12);
		time += t2.elapsed();
		cout << time << ' ' <<abs.relativeGap() << endl;
	}
	
	cout << time << endl;
}

int main (int argc, char **argv)
{
	general("networks/ChicagoSketch_net.txt", "networks/ChicagoSketch_trips.txt", 0.04, 0.02);
//	general("networks/Auckland_net2.txt", "networks/Auckland_trips.txt");
//	general("networks/SiouxFalls_net.txt", "networks/SiouxFalls_trips.txt");
//	general("networks/Anaheim_net.txt", "networks/Anaheim_trips.txt");
//	general("networks/ChicagoRegional_net.txt", "networks/ChicagoRegional_trips.txt", 0.25, 0.1, 1e-4);
//	general("networks/Philadelphia_network.txt", "networks/Philadelphia_trips.txt", 0.0, 0.055, 1e-4);//Known BUG: Doesn't quite work just yet.
}
