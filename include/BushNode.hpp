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


#ifndef BUSH_NODE_HPP
#define BUSH_NODE_HPP

#include <vector>
#include <limits>
#include <iostream>
#include <utility>

#include "BushEdge.hpp"

class ABGraph;

class BushNode
{
	public:
		BushNode();
		void equilibriate(ABGraph&);
		void updateInDistances(std::vector<BushEdge>::iterator, std::vector<BushEdge>::iterator);
		double minDist() const { return minDistance; }
		double maxDist() const { return maxDistance; }
		double getDifference() const { return (maxDistance-minDistance); }
		BushEdge* getMinPredecessor() { return minPredecessor; }
		void setDistance(double d) { minDistance = maxDistance = d; }
	private:
		bool moreSeparatePaths(BushNode*&, BushNode*&, ABGraph&);
		void fixDifferentPaths(std::vector<std::pair<BushEdge*, ForwardGraphEdge*> >&,
				       std::vector<std::pair<BushEdge*, ForwardGraphEdge*> >&, double);
		
		BushEdge* minPredecessor;
		BushEdge* maxPredecessor;
		//TODO: store predecessors elsewhere.
		
		double minDistance;
		double maxDistance;
};

/*
This function inlined because it's called many, many times, and we spend quite
a bit of our time in it. Around 60%, last measured.
Could think about threading it, not interleaving min- and max- predecessor
calculations etc. Only scales to 2 threads, though, and we'd have to thread
updateEdges (and possibly topo-sort) as well to make it worthwhile. Still, for
two threads that beats some other ideas. Don't sacrifice any convergence per
iteration.
*/
inline void BushNode::updateInDistances(std::vector<BushEdge>::iterator it, std::vector<BushEdge>::iterator end)
{
	/*
	Our rules are as follows:
	Min-dist is the min of in-arc lengths + in-arc origin-node min-dists.
	If we have in-flows, max-dist is the max of in-arc lengths + in-arc
	origin-node max-dists (only across in-arcs with flow)
	If we don't have in-flows, max-dist is the min of in-arc length +
	in-arc origin-node max dists. (Seems silly, seems to work)
	*/
	
	
	/*
	NOTE: We won't begin by assuming that all nodes have in-arcs.
	
	This is far too branchy for my tastes. Maybe we should order these
	edges by flow? Could get rid of a few branches. Maintaining order
	could be costly, though.
	*/
	
	std::vector<BushEdge>::iterator minPred;
	std::vector<BushEdge>::iterator maxPred;
	double minDist = std::numeric_limits<double>::infinity();
	double maxDist = std::numeric_limits<double>::infinity();

	for(; it != end; ++it) {
		
		//No flow to date.
		BushNode *fromNode = it->fromNode();
		double edgeLength = it->length();
		
		double fromMinDist = fromNode->minDistance + edgeLength;
		double fromMaxDist = fromNode->maxDistance + edgeLength;
		
		if(minDist > fromMinDist) {
			minPred = it;
			minDist = fromMinDist;
		}
		if(it->used()) {
			//Flow!
			maxPred = it;
			maxDist = fromMaxDist;
			++it;
			break;
		} else if (maxDist > fromMaxDist) {
			maxPred = it;
			maxDist = fromMaxDist;
		}
	}
	for(; it != end; ++it) {
		//For when we know we have flow.
		BushNode *fromNode = it->fromNode();
		double edgeLength = it->length();

		double fromMinDist = fromNode->minDistance + edgeLength;
		double fromMaxDist = fromNode->maxDistance + edgeLength;
		
		if(minDist > fromMinDist) {
			minPred = it;
			minDist = fromMinDist;
		}
		if (it->used() && maxDist < fromMaxDist) {
			maxPred = it;
			maxDist = fromMaxDist;
		}
	}

	maxDistance = maxDist;
	minDistance = minDist;
	maxPredecessor = &*maxPred;
	minPredecessor = &*minPred;
	//save our results
}

#endif
