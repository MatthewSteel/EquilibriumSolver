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

class Bush
{
	public:
		Bush(const Origin&, ABGraph&, std::vector<std::pair<double, unsigned> >&);//Inits bush, sends initial flows
		bool fix(double);
		void printCrap();
		int getOrigin() { return origin.getOrigin(); }
		int giveCount()/* const*/;
		double allOrNothingCost();
		double maxDifference();
		~Bush();
	private:
		bool updateEdges();
		bool equilibriateFlows(double);//Equilibriates, tells graph what's going on
		bool updateEdges(EdgeVector&, double, unsigned);
		void buildTrees();
		void sendInitialFlows();
		//Makes sure all our edges are pointing in the right direction, and we're sorted well.
		void setUpGraph();
		void topologicalSort();
		
		const Origin& origin;
		std::vector<EdgeVector> bush;//Stores BushEdges, half the memory req of regular vectors.
		//Not storing pointers because these are only 16 bytes each (can cut to 12).
		
		std::vector<unsigned> topologicalOrdering;
		
		//Shared with other bushes so we don't deallocate/reallocate data uselessly between bush iterations
		std::vector<BushNode>& sharedNodes;
		std::vector<std::pair<double, unsigned> >& tempStore;//Used in topo sort, don't want to waste the alloc/dealloc time.
		
		ABGraph& graph;
};

//Inlined because we call this once per node per iteration, and spend 35% of our time in here.
inline bool Bush::updateEdges(EdgeVector& inEdges, double maxDist, unsigned id)
{
	BushEdge* changeBegin = inEdges.begin(), *last = inEdges.end();
	
	//Partition outEdges into good, bad. std::partition isn't inlining.
	//As seen in quicksort etc.
	while (true) {
		while (changeBegin != last && changeBegin->fromNode()->maxDist() <= maxDist) ++changeBegin;
		if (changeBegin == last--) break;
		while (changeBegin != last && last->fromNode()->maxDist() > maxDist) --last;
		if (changeBegin == last) break;
		std::swap (*changeBegin++, *last);
	}
	
	if(changeBegin == inEdges.end()) return true;
	//No bad ones, we can  exit early.
	
	for(BushEdge* edge = changeBegin; edge != inEdges.end(); ++edge) {
		
		unsigned fromId = edge->fromNode()-&sharedNodes[0];
		edge.swapDirection(graph);
		bush[fromId].push_back(edge);
	}
	inEdges.resize(outEdges.end()-changeBegin);
	//For all of the bad ones, switch their directions and remove them from the out-edge list.
	return false;
}

#endif
