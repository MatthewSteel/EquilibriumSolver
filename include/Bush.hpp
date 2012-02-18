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


#ifndef BUSH_HPP
#define BUSH_HPP

#include "GraphEdge.hpp"
#include "Origin.hpp"
#include "BushNode.hpp"
#include "ABGraph.hpp"
#include "EdgeVector.hpp"

#include <vector>
#include <utility>
#include <iostream>

class Bush
{
	public:
		Bush(const Origin&, ABGraph&, std::vector<std::pair<double, unsigned> >&, std::vector<unsigned>&);//Inits bush, sends initial flows
		bool fix(double);
		void printCrap();
		int getOrigin() { return origin.getOrigin(); }
		long giveCount()/* const*/;
		double allOrNothingCost();
		double maxDifference();
		~Bush();
	private:
		bool updateEdges();
		bool equilibriateFlows(double);//Equilibriates, tells graph what's going on
		void updateEdges(EdgeVector&, double, unsigned);
		void buildTrees();
		void sendInitialFlows();
		//Makes sure all our edges are pointing in the right direction, and we're sorted well.
		void setUpGraph();
		void topologicalSort();
		void applyBushEdgeChanges();
		void partialTS(unsigned, unsigned);
		
		const Origin& origin;
		std::vector<EdgeVector> bush;//Stores BushEdges, half the memory req of regular vectors.
		//Not storing pointers because these are only 16 bytes each (can cut to 12).
		
		std::vector<unsigned> topologicalOrdering;
		
		//Shared with other bushes so we don't deallocate/reallocate data uselessly between bush iterations
		std::vector<BushNode>& sharedNodes;
		std::vector<std::pair<double, unsigned> >& tempStore;//Used in topo sort, don't want to waste the alloc/dealloc time.
		std::vector<unsigned> &reverseTS;
		
		ABGraph& graph;
		
		std::vector<std::pair<unsigned, unsigned> > changes;//BushEdges to reverse
		
		static bool pairComparator(const std::pair<double,unsigned>& first, const std::pair<double,unsigned>& second) {
			return first.first < second.first;
		}
		
};

//Inlined because we call this once per node per iteration, and spend 35% of our time in here. FIXME
inline void Bush::updateEdges(EdgeVector& inEdges, double maxDist, unsigned id)
{
	BushEdge *last = inEdges.end();
	
	for(BushEdge* changeBegin = inEdges.begin(); changeBegin != last; ++changeBegin) {
		if(changeBegin->fromNode()->maxDist() > maxDist) {
			changes.push_back(std::make_pair(id, changeBegin - inEdges.begin()));
		
		//	if(origin.getOrigin()==0 && debug) std::cout << "Edge to turn around: (" << id << "," << changeBegin->fromNode()-&sharedNodes[0] << ")" << std::endl;
		}
	}
}

#endif
