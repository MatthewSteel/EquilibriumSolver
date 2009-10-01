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
#include <cctype>
#include "TAPFramework/NetworkProperties.hpp"
#include "TAPFramework/Intersection.hpp"
#include "TAPFramework/Road.hpp"

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
	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'\n');//Skip #zones - not important

	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'>');//Skip to #nodes
	is >> nodes;//Read in #nodes

	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'>');//Skip to First Through Node
	unsigned firstThroughNode;
	is >> firstThroughNode;
	zones = firstThroughNode - 1;
	
	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'>');//Skip to #edges
	unsigned arcs;
	is >> arcs;

	is.ignore(numeric_limits<streamsize>::max(),'>');//Skip to end of metadata
	graph.reset(new Graph(nodes+zones));
	while (arcs --> 0) {
		skipComments(is);
		unsigned to, from;
		is >> from >> to;
		
		if(to <= zones) to += nodes;
		
		double capacity, length, speed, toll, zeroFlowTime, alpha;
		unsigned beta;
		
		is >> capacity >> length >> zeroFlowTime >> alpha >> beta >> speed >> toll;
		//Don't use speed, type?
		Road r(BPRFunction(zeroFlowTime, capacity, alpha, beta), length, toll);
		add_edge(from-1, to-1, r, *graph);

		is.ignore(numeric_limits<streamsize>::max(),';');//Skip to end of row
	}
}

void GraphImporter::readInTrips(boost::shared_ptr<Graph>& graph, istream& is)
{
	if (!is) {
		cerr << "Guys, bad times with the trips file" << endl;
		return;
	}
	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'>');//Skip to #zones
	unsigned z;
	is >> z;
	
	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'\n');//Skip next line
	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'\n');//End of metadata
	
	int currentNode = -1;
	list<pair<unsigned, double> > currentDestinations;
	while(true) {
		skipComments(is);
		if(!is.good()) break;
		if(is.peek() == 'O') {
			is.ignore(6);
			//New origin. Add the old one to the graph if it has destinations.
			if (!currentDestinations.empty()) {
				(*graph)[vertex(currentNode-1, *graph)].destinations = currentDestinations;
				currentDestinations.clear();
			}
			is >> currentNode;
			continue;
		} else {
			//Read in the destination data
			unsigned toNode;
			double amount;
			is >> toNode;
			if(toNode <= zones) toNode += nodes;
			is.ignore(numeric_limits<streamsize>::max(),':');
			is >> amount;
			if (amount > 0 && static_cast<int>(toNode) != currentNode)
				currentDestinations.push_back(pair<unsigned, double>(vertex(toNode-1, *graph), amount));
			is.ignore(numeric_limits<streamsize>::max(),';');
		}
	}
	if (!currentDestinations.empty()) {
		(*graph)[vertex(currentNode-1, *graph)].destinations = currentDestinations;
	}
}

void GraphImporter::skipComments(std::istream& i)
{
	bool haveComment = true, haveSpace = true;
	if(i.bad()) return;
	while(true) {
		haveSpace = false;
		while(i.good() && isspace(i.peek())) {
			i.ignore();
			haveSpace = true;
		}
		if(!(haveSpace || haveComment)) return;
		haveComment = false;
		while(i.good() && i.peek() == '~') {
			i.ignore(numeric_limits<streamsize>::max(),'\n');
			haveComment = true;
		}
		if(!(haveSpace || haveComment)) return;
	}
}