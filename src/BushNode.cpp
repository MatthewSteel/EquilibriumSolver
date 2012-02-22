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


#include "BushNode.hpp"

#include <algorithm> //For min
#include "SecantSolver.hpp"
#include <iostream>
#include "ABAdder.hpp"
#include "ABGraph.hpp"

using namespace std;

BushNode::BushNode() :
	minDistance(numeric_limits<double>::infinity()),
	maxDistance(numeric_limits<double>::infinity()) {}

bool BushNode::moreSeparatePaths(BushNode*& minNode, BushNode*& maxNode, ABGraph &graph)
{
	//Precondition: minNode == maxNode?
	while(true) {
		if(minNode->minDistance == minNode->maxDistance) return false;//path joins to root
		else if(minNode->minPredecessor == maxNode->maxPredecessor) {
			minNode = minNode->minPredecessor->fromNode();
			maxNode = maxNode->maxPredecessor->fromNode();
		} else return true;//New segments to equilibriate: min/max predecessors are different.
	}
}//Ignore min/max paths that coincide


void BushNode::fixDifferentPaths(
               vector<pair<BushEdge*, ForwardGraphEdge*> >& minEdges,
               vector<pair<BushEdge*, ForwardGraphEdge*> >& maxEdges,
               double maxChange)
{

	ABAdder hp(minEdges.size(), maxEdges.size());
	for(vector<pair<BushEdge*, ForwardGraphEdge*> >::iterator i = maxEdges.begin(); i != maxEdges.end(); ++i) {
		hp -= make_pair(i->second->costFunction(), i->second->getFlow());
	}
	
	for(vector<pair<BushEdge*, ForwardGraphEdge*> >::iterator i = minEdges.begin(); i != minEdges.end(); ++i) {
		hp += make_pair(i->second->costFunction(), i->second->getFlow());
	}
	SecantSolver<ABAdder> solver;
	double newFlow = solver.solve(hp, maxChange, 0);//Change in flow
	
	if(newFlow == 0) return;//No change

	if(newFlow > maxChange) newFlow = maxChange;
	//Wait, is this done in the solver now?
	
	for(vector<pair<BushEdge*, ForwardGraphEdge*> >::iterator i = minEdges.begin(); i != minEdges.end(); ++i) {
		i->first->addFlow(newFlow, i->second);
	}
	for(vector<pair<BushEdge*, ForwardGraphEdge*> >::iterator i = maxEdges.begin(); i != maxEdges.end(); ++i) {
		i->first->addFlow(-newFlow, i->second);
	}
}

void BushNode::equilibriate(ABGraph& graph)
{
	/*
	NOTE: It is very important to equilibriate the different distinct segments
	of the min/max paths separately, or we get worst-case behaviour a'la
	Ford-Fulkerson
	*/
	
	BushNode* minNode = this;
	BushNode* maxNode = this;

	while (true) {
		vector<pair<BushEdge*, ForwardGraphEdge*> > minEdges;
		vector<pair<BushEdge*, ForwardGraphEdge*> > maxEdges;
		double maxChange = numeric_limits<double>::infinity();
		
		
		if(!moreSeparatePaths(minNode, maxNode, graph)) return;
		//Indicates we're done or sets node positions to start of next segment
		
		do { //Trace separate paths back, adding arcs to lists
			if(minNode->maxDistance >= maxNode->maxDistance) {
				BushEdge* pred = minNode->minPredecessor;
				ForwardGraphEdge* fge = graph.forward(pred->underlyingEdge());
				minEdges.push_back(make_pair(pred, fge));
				minNode = pred->fromNode();
			} else {
				BushEdge* pred = maxNode->maxPredecessor;
				ForwardGraphEdge* fge = graph.forward(pred->underlyingEdge());
				maxChange = min(maxChange, pred->flow());
				maxEdges.push_back(make_pair(pred, fge));
				maxNode = pred->fromNode();
			}
		} while(minNode != maxNode);
		if(maxChange > 1e-12) fixDifferentPaths(minEdges, maxEdges, maxChange);
	}
	//Probably the ugliest function in the program now.
}
