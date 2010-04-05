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
#include "ABGraph.hpp"
#include "EdgeVector.hpp"

class BushNode
{
	public:
		BushNode(unsigned);
		void equilibriate(ABGraph&);
		void updateOutDistances(EdgeVector&);
		double minDist() const { return minDistance; }
		double maxDist() const { return maxDistance; }
		unsigned getId() const { return id; }
		double getDifference() const { return (maxDistance-minDistance); }
		BushEdge* getMinPredecessor() { return minPredecessor; }
		void reset() {
			realFlow = false;
			maxDistance = std::numeric_limits<double>::infinity();
			//trust me
			minDistance = std::numeric_limits<double>::infinity();
			minPredecessor = maxPredecessor = 0;
		}
		void setDistance(double d) { minDistance = maxDistance = d; }
	private:
		bool moreSeparatePaths(BushNode*&, BushNode*&, ABGraph&);
		void fixDifferentPaths(std::vector<std::pair<BushEdge*, BackwardGraphEdge*> >&,
				       std::vector<std::pair<BushEdge*, BackwardGraphEdge*> >&, double);
		
		BushEdge* minPredecessor;
		BushEdge* maxPredecessor;
		double minDistance;
		double maxDistance;
		unsigned id;
		bool realFlow;
};

//This function inlined because we spend 60% of our total execution time in it.
//Will be marginally sped up by inverting arcs?
//Also try threading min/max? Could gain 30 odd % if we're lucky, but they only run for 3ms on our biggest graph...
inline void BushNode::updateOutDistances(EdgeVector& outEdges)
{
	BushEdge* end = outEdges.end();
	for(BushEdge* i = outEdges.begin(); i != end; ++i) {
		//Function only gets called when !outEdges.empty()
		BushEdge& outEdge = (*i);
		BushNode* toNode = outEdge.toNode();
		double edgeLength = outEdge.length();
		if(minDistance + edgeLength <= toNode->minDistance) {
			toNode->minDistance = minDistance + edgeLength;
			toNode->minPredecessor = &outEdge;
		}
		if(/*realFlow &&*/outEdge.used()) {//BUG, TODO: This should fix a bug? Test it later.
			if(!toNode->realFlow || maxDistance + edgeLength > toNode->maxDistance) {
				toNode->maxDistance = maxDistance + edgeLength;
				toNode->maxPredecessor = &outEdge;
			}
			toNode->realFlow = true;
		} else if (!outEdge.toNode()->realFlow) {
			if(maxDistance + edgeLength < toNode->maxDistance) {
				toNode->maxDistance = maxDistance + edgeLength;
				toNode->maxPredecessor = &outEdge;
			}
		}
	}
}

#endif
