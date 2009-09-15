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


#include "TAPFramework/Tester.hpp"
#include "TAPFramework/GraphImporter.hpp"

#include "FrankWolfeSolver.hpp"
#include "AlgorithmBSolver.hpp"
#include <fstream>
#include <iostream>
using namespace std;
using namespace boost;
using namespace TAPFramework;

double Tester::setGraph(string networkFile, string TripsFile, NetworkProperties np)
{
	GraphImporter gi;
	ifstream network(networkFile.c_str()), trips(TripsFile.c_str());
	
	graph = gi.readInGraph(network, trips);
	properties = np;
	boost::timer t;
	tas.setUp(graph, properties);
	return t.elapsed();
}

void Tester::test(unsigned iterationLimit, double accuracy)
{
	timer t;
	tas.solve(iterationLimit, accuracy);
	timeToSolve += t.elapsed();
}

double Tester::relativeGap()
{
	FrankWolfeSolver fw(graph, properties);
	return fw.relativeGap();
}
double Tester::averageExcessCosts()
{
	FrankWolfeSolver fw(graph, properties);
	return fw.averageExcessCost();
}
