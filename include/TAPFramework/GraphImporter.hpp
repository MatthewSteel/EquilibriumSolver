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


//Exists to shorten compile times in FrankWolfeSolver.cpp.
//Also helps code re-use later on.

#ifndef GRAPH_IMPORTER_HPP
#define GRAPH_IMPORTER_HPP

#include <boost/shared_ptr.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <istream>

namespace TAPFramework {

class Intersection;
class Road;
class NetworkProperties;

/**
 * A class to facilitate the importation of network files.
 * Network files are interpreted in the format as specified in Hillel
 * Bar-Gera's page at http://www.bgu.ac.il/~bargera/tntp/
 */

//TODO: Clean this up some
class GraphImporter {

	typedef boost::adjacency_list<
		boost::vecS,
		boost::vecS,
		boost::bidirectionalS,
		TAPFramework::Intersection,
		TAPFramework::Road> Graph;

	public:
		boost::shared_ptr<Graph> readInGraph(std::istream&, std::istream&);
		void readInNetwork(boost::shared_ptr<Graph>&, std::istream&);
		void readInTrips(boost::shared_ptr<Graph>&, std::istream&);
	private:
		void skipComments(std::istream&);
		unsigned nodes, zones;
};

}

#endif
