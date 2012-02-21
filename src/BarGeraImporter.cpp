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

#include "BarGeraImporter.hpp"
#include "BarGeraBPRFunction.hpp"
#include "InputGraph.hpp"

#include <limits>
#include <vector>
#include <cctype>//isspace()

using namespace std;

void BarGeraImporter::readInGraph(InputGraph& graph, std::istream& networkStream, std::istream& tripsStream)
{
	if(!networkStream) throw "Network file does not exist";
	if(!tripsStream) throw "Trips file does not exist";
	readInNetwork(graph, networkStream);
	readInTrips(graph, tripsStream);
}

void BarGeraImporter::readInNetwork(InputGraph& graph, std::istream& is)
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
	graph.setNodes(nodes+zones);
	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'>');//Skip to #edges
	unsigned arcs;
	is >> arcs;
	
	endMetadata(is);
	
	while (arcs --> 0) {
		skipComments(is);
		unsigned to, from;
		is >> from >> to;
		
		if(to <= zones) to += nodes;
		
		double capacity, length, speed, toll, zeroFlowTime, alpha;
		double beta;
		
		is >> capacity >> length >> zeroFlowTime >> alpha >> beta >> speed >> toll;
		//Don't use speed, type?
		BarGeraBPRFunction func(zeroFlowTime, capacity, alpha, beta, length*distanceCost+toll*tollCost);
		graph.addEdge(from-1, to-1, func.costFunction());
		
		is.ignore(numeric_limits<streamsize>::max(),';');//Skip to end of row
	}
}

void BarGeraImporter::readInTrips(InputGraph& graph, std::istream& is)
{
	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'>');//Skip to #zones
	unsigned z;
	is >> z;
	
	skipComments(is);
	is.ignore(numeric_limits<streamsize>::max(),'\n');//Skip next line
	skipComments(is);
	endMetadata(is);
	
	int currentNode = -1;
	vector<pair<unsigned, double> > currentDestinations;
	while(true) {
		skipComments(is);
		if(!is.good()) break;
		if(is.peek() == 'O') {
			is.ignore(6);
			//New origin. Add the old one to the graph if it has destinations.
			if (!currentDestinations.empty()) {
				for(vector<pair<unsigned,double> >::iterator i = currentDestinations.begin(); i != currentDestinations.end(); ++i)
					graph.addDemand(currentNode-1, i->first, i->second);
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
				currentDestinations.push_back(pair<unsigned, double>(toNode-1, amount));
			is.ignore(numeric_limits<streamsize>::max(),';');
		}
	}
	if (!currentDestinations.empty()) {
		for(vector<pair<unsigned,double> >::iterator i = currentDestinations.begin(); i != currentDestinations.end(); ++i)
			graph.addDemand(currentNode-1, i->first, i->second);
	}
}

void BarGeraImporter::skipComments(std::istream& i)
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

void BarGeraImporter::endMetadata(std::istream& is)
{
	//Sometimes there is misc metadata. Ugh. Skip that stuff.
	while (true) {
		is.ignore(numeric_limits<streamsize>::max(),'<');//Skip to "end of metadata" hopefully	
		bool out = true;
		for(int i = 0; i < 15; ++i) {
			out = out && (is.get() == "END OF METADATA"[i]);
		}
		if(out) break;
	}
	is.ignore(numeric_limits<streamsize>::max(),'>');
	
}
