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

#include <vector>
#include <utility>

class Bush
{
	public:
		Bush(const Origin&, ABGraph&, std::vector<std::pair<double, unsigned> >&);//Inits bush, sends initial flows
		bool fix(double);
		void printCrap() const;
		int getOrigin() { return origin.getOrigin(); }
		int giveCount()/* const*/;
		double allOrNothingCost();
		double maxDifference();
		~Bush();
	private:
		bool updateEdges();
		bool equilibriateFlows(double);//Equilibriates, tells graph what's going on
		bool updateEdges(std::vector<BushEdge>&, double, unsigned);
		void buildTrees();
		void sendInitialFlows();
		//Makes sure all our edges are pointing in the right direction, and we're sorted well.
		void setUpGraph(ABGraph& g);
		void topologicalSort();
		
		const Origin& origin;
		std::vector<std::vector<BushEdge> > bush;
		//Used to be vec<vec<Edge*>> but now edges are lightweight, rarely move and the pointer is just extra storage.
		//(We need to store ~50,000,000 of these. If we need to save space at the expense of running time we can probably cut that by 5 though.)
		
		std::vector<unsigned> topologicalOrdering;
		
		//Shared with other bushes so we don't deallocate/reallocate data uselessly between bush iterations
		std::vector<BushNode>& sharedNodes;
		std::vector<std::pair<double, unsigned> >& tempStore;//Used in topo sort, don't want to waste the alloc/dealloc time.
};

//Inlined because we call this once per node per iteration, and spend 35% of our time in here.
inline bool Bush::updateEdges(std::vector<BushEdge>& outEdges, double maxDist, unsigned id)
{
	std::vector<BushEdge>::iterator changeBegin = outEdges.begin(), last = outEdges.end();
	
	//Partition outEdges into good, bad. Pretty sure std::partition didn't inline.
	//As seen in quicksort etc.
	while (true) {
		while (changeBegin != last && changeBegin->toNode()->maxDist() >= maxDist) ++changeBegin;
		if (changeBegin == last--) break;
		while (changeBegin != last && last->toNode()->maxDist() < maxDist) --last;
		if (changeBegin == last) break;
		std::swap (*changeBegin++, *last);
	}
	
	if(changeBegin == outEdges.end()) return true;
	//No bad ones, we can  exit early.
	
	for(std::vector<BushEdge>::iterator i = changeBegin; i != outEdges.end(); ++i) {
		
		BushEdge& edge = (*i);
		unsigned toId = edge.toNodeId();
		edge.swapDirection();
		bush[toId].push_back(edge);
	}
	outEdges.erase(changeBegin,outEdges.end());
	//For all of the bad ones, switch their directions and remove them from the out-edge list.
	return false;
}

#endif
