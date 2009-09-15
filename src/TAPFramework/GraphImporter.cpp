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


#include "TAPFramework/GraphImporter.hpp"
#include <iostream>
#include <limits>
#include <list>
#include <utility>
#include "TAPFramework/NetworkProperties.hpp"
#include "TAPFramework/Intersection.hpp"
#include "TAPFramework/Road.hpp"
#include <sstream>

using namespace std;
using namespace boost;
using namespace TAPFramework;

typedef boost::adjacency_list<
	boost::vecS,
	boost::vecS,
	boost::bidirectionalS,
	TAPFramework::Intersection,
	TAPFramework::Road> Graph;
typedef Graph::vertex_descriptor Vertex;

shared_ptr<Graph> GraphImporter::readInGraph(istream& graphStream, istream& originStream)
{
	shared_ptr<Graph> graph;
	readInNetwork(graph, graphStream);
	readInTrips(graph, originStream);
	return graph;
}

void GraphImporter::readInNetwork(shared_ptr<Graph>& graph, istream& is)
{
	/*
	TODO: format is:
	
	<NUMBER OF ZONES> Z
	<NUMBER OF NODES> N
	<FIRST THRU NODE> F
	<NUMBER OF LINKS> E
	<END OF METADATA>
	
	We should also deal with comments properly.
	*/
	if (!is) {
		cerr << "Guys, bad times with the network file" << endl;
		return;
	}
	is.ignore(numeric_limits<streamsize>::max(),'\n');
	is.ignore(numeric_limits<streamsize>::max(),'>');
	
	is >> nodes;

	is.ignore(numeric_limits<streamsize>::max(),'>');
	unsigned firstThroughNode;
	is >> firstThroughNode;
	zones = firstThroughNode - 1;
	is.ignore(numeric_limits<streamsize>::max(),'\n');
	is.ignore(numeric_limits<streamsize>::max(),'>');
	
	unsigned arcs;
	is >> arcs;

	graph.reset(new Graph(nodes+zones));
	is.ignore(numeric_limits<streamsize>::max(),';');
	while (arcs --> 0) {
		unsigned to, from;
		is >> from >> to;
		
		if(to <= zones) to += nodes;
		
		double capacity, length, speed, toll, zeroFlowTime, alpha;
		unsigned beta;
		
		is >> capacity >> length >> zeroFlowTime >> alpha >> beta >> speed >> toll;
		//Don't use speed, type?
		Road r(BPRFunction(zeroFlowTime, capacity, alpha, beta), length, toll);
		add_edge(from-1, to-1, r, *graph);
//		cout << "e " << from << " " << to << endl;

		is.ignore(numeric_limits<streamsize>::max(),';');
	}
}

void GraphImporter::readInTrips(boost::shared_ptr<Graph>& graph, istream& is)
{
	if (!is) {
		cerr << "Guys, bad times with the trips file" << endl;
		return;
	}
	string s;
	getline(is, s);
	unsigned currentNode = 500;
	while(!is.eof() && s.find("Origin") == string::npos)
		getline(is, s);

	stringstream ss(s);
	ss.ignore(numeric_limits<streamsize>::max(),'n');
	ss >> currentNode;
	list<pair<unsigned, double> > currentDestinations;

	string::size_type index;
	getline(is,s);
	ss.clear();
	for(ss.str(s); !is.eof(); getline(is,s), ss.clear(), ss.str(s)) {
		if((index = s.find("Origin")) != string::npos) {
			if (!currentDestinations.empty()) {
				(*graph)[vertex(currentNode-1, *graph)].destinations = currentDestinations;
				currentDestinations.clear();
//				cout << "origin: " << currentNode << ":" << endl;
//				for(list<pair<unsigned, double> >::iterator i = (*graph)[vertex(currentNode-1, *graph)].destinations.begin(); i != (*graph)[vertex(currentNode-1, *graph)].destinations.end(); ++i) {
//					cout << i->first << "(" << i->second << ")" << endl;
//				}
			}
			ss.ignore(index+6);
			ss >> currentNode;
			//cout << "found origin " << currentNode << ", index was " << index <<endl;
		} else {
			for(index = s.find(';'); index != string::npos; ss.ignore(numeric_limits<streamsize>::max(),';'), index = s.find(';', index+1)) {
				unsigned dest= 0;
				ss >> dest;
				if(dest <= zones) dest += nodes;
				ss.ignore(numeric_limits<streamsize>::max(),':');
				double flow=0.0;
				ss >> flow;
				if (flow > 0 && dest != currentNode)
					currentDestinations.push_back(pair<unsigned, double>(vertex(dest-1, *graph), flow));
			}
		}
	}
	if (!currentDestinations.empty())
		(*graph)[vertex(currentNode-1, *graph)].destinations = currentDestinations;
/*	is.ignore(numeric_limits<streamsize>::max(),'n');
	for (int i = 0; i < nodes; ++i) {
		Vertex v = vertex(i, *graph);
		list<pair<unsigned, double> > destinations;
		for (unsigned j = 0; j < nodes; ++j) {
			unsigned k = j;
			if(j+1 < zones) k += nodes;
			is.ignore(numeric_limits<streamsize>::max(),':');
			double flow;
			is >> flow;

			if (flow > 0)
				destinations.push_back(pair<unsigned, double>(vertex(j, *graph), flow));
		}
		if (!destinations.empty()) (*graph)[v].destinations = destinations;
	}
*/
}
