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
#include "NaiveAdder.hpp"
#include <iostream>
#include "ABAdder.hpp"

using namespace std;

BushNode::BushNode(unsigned id) : minDistance(numeric_limits<double>::infinity()), maxDistance(numeric_limits<double>::infinity()),  id(id), realFlow(false)
{}

bool BushNode::moreSeparatePaths(BushNode*& minNode, BushNode*& maxNode)
{
	//Precondition: minNode == maxNode?
	while(true) {
		if(minNode->minDistance == minNode->maxDistance) return false;//Reached root
		else if(minNode->minPredecessor == maxNode->maxPredecessor) {
			minNode = minNode->minPredecessor->fromNode();
			maxNode = maxNode->maxPredecessor->fromNode();
		} else return true;//New segments to equilibriate
	}
}//Ignore min/max paths that coincide

void BushNode::fixDifferentPaths(vector<BushEdge*>& minEdges, vector<BushEdge*>& maxEdges, double maxChange)
{
	ABAdder hp;

	for(vector<BushEdge*>::iterator i = maxEdges.begin(); i != maxEdges.end(); ++i) {
		hp -= ((*i)->costFunction());
	}
	
	for(vector<BushEdge*>::iterator i = minEdges.begin(); i != minEdges.end(); ++i) {
		hp += ((*i)->costFunction());
	}
	/*
	NOTE: The below may be faster, test on large instances.
	NaiveAdder<HornerPolynomial> hp;
	for(vector<BushEdge*>::iterator i = minEdges.begin(); i != minEdges.end(); ++i)
		hp += *((*i)->costFunction());
	for(vector<BushEdge*>::iterator i = maxEdges.begin(); i != maxEdges.end(); ++i) {
		HornerPolynomial h2 = *((*i)->costFunction());
		h2.multiplyX(-1);
		h2*=-1;
		hp += h2;
	}
	*/
	
	SecantSolver<ABAdder> solver;
	double newFlow = solver.solve(hp, maxChange, 0);//Change in flow
	
	if(newFlow == 0) return;//No change
//	double eval = hp(newFlow);

//	if(eval < -1) newFlow = maxChange;
	//cout << maxChange << ',' << newFlow << endl;
	/*
	HACK: Because the equation may have no roots, we sometimes get silly
	results from it. Unfortunately, when we consider the different distinct
	min/max path segments separately, sometimes the solver says we should
	shift flow the wrong way, giving us false positives when we just test
	newFlow < 0. -0.1 is arbitrary, but it seems to distinguish the two cases
	*/
	
	if(newFlow > maxChange) newFlow = maxChange;

	for(vector<BushEdge*>::iterator i = minEdges.begin(); i != minEdges.end(); ++i)
		(*i)->addFlow(newFlow);
	for(vector<BushEdge*>::iterator i = maxEdges.begin(); i != maxEdges.end(); ++i)
		(*i)->addFlow(-newFlow);
}

void BushNode::equilibriate()
{
	BushNode* minNode = this;
	BushNode* maxNode = this;
	/*
	NOTE: It is very important to equilibriate the different distinct segments
	of the min/max paths separately, or we get worst-case behaviour like the
	Ford-Fulkerson algorithm
	*/
	while (true) {
		vector<BushEdge*> minEdges;
		vector<BushEdge*> maxEdges;
		double maxChange = numeric_limits<double>::infinity();
		
		if(!moreSeparatePaths(minNode, maxNode)) { return; }
		//Indicates we're done or sets node positions to start of next segment
		do { //Trace paths back, adding arcs to lists
			if(minNode->minPredecessor->fromNode()->minDistance == maxNode->maxPredecessor->fromNode()->maxDistance) {
				/*
				NOTE: If we didn't have this, the elseif could occur and set minNode
				to the root (and keeping maxNode not root), and we segfault (or worse)
				when we test its predecessor next time around
				*/
				minEdges.push_back(minNode->minPredecessor);
				minNode = minNode->minPredecessor->fromNode();
				maxChange = min(maxChange, maxNode->maxPredecessor->flow());
				maxEdges.push_back(maxNode->maxPredecessor);
				maxNode = maxNode->maxPredecessor->fromNode();
			} else if(minNode->minPredecessor->fromNode()->minDistance >= maxNode->maxPredecessor->fromNode()->maxDistance) {
				minEdges.push_back(minNode->minPredecessor);
				minNode = minNode->minPredecessor->fromNode();
			} else {
				maxChange = min(maxChange, maxNode->maxPredecessor->flow());
				maxEdges.push_back(maxNode->maxPredecessor);
				maxNode = maxNode->maxPredecessor->fromNode();
			}
		} while(minNode->id != maxNode->id);
		if(maxChange > 1e-12) fixDifferentPaths(minEdges, maxEdges, maxChange);
		//Should I return otherwise? Should I return when I see it for the first time?
		//Not that it matters too much, we spend < 3% of our time here and down.
		//Benchmark it.
	}
}
