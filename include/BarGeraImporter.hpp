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


#ifndef BAR_GERA_IMPORTER_HPP
#define BAR_GERA_IMPORTER_HPP

#include "InputGraph.hpp"
#include <istream>

/**
 * A class to facilitate the importation of network files.
 * Network files are interpreted in the format as specified in Hillel
 * Bar-Gera's page at http://www.bgu.ac.il/~bargera/tntp/
 */

class BarGeraImporter {

	public:
		BarGeraImporter(double distanceCost, double tollCost) :
			distanceCost(distanceCost), tollCost(tollCost) {}
		
		void readInGraph(InputGraph& graph, std::istream& networkFile, std::istream& tripsFile);
	private:
		
		void readInNetwork(InputGraph& graph, std::istream& networkFile);
		
		void readInTrips(InputGraph& graph, std::istream& tripsFile);
		
		
		void skipComments(std::istream&);
		
		void endMetadata(std::istream&);
		
		double distanceCost;
		double tollCost;
		unsigned nodes, zones;
};


#endif
