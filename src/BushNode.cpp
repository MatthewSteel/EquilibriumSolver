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

using namespace std;

BushNode::BushNode(unsigned id) : minDistance(numeric_limits<double>::infinity()), maxDistance(numeric_limits<double>::infinity()),  id(id), realFlow(false)
{}

bool BushNode::moreSeparatePaths(BushNode*& minNode, BushNode*& maxNode, ABGraph &graph)
{
	//Precondition: minNode == maxNode?
	while(true) {
		if(minNode->minDistance == minNode->maxDistance) return false;//path joins to root
		else if(minNode->minPredecessor == maxNode->maxPredecessor) {
			minNode = graph.backward(minNode->minPredecessor->underlyingEdge())->fromNode();
			maxNode = graph.backward(maxNode->maxPredecessor->underlyingEdge())->fromNode();
		} else return true;//New segments to equilibriate: min/max predecessors are different.
	}
}//Ignore min/max paths that coincide

void BushNode::fixDifferentPaths(vector<pair<BushEdge*, BackwardGraphEdge*> >& minEdges,
                                 vector<pair<BushEdge*, BackwardGraphEdge*> >& maxEdges, double maxChange)
{
//	cout << "Length of things: "<< minEdges.size() << ", " << maxEdges.size() << ", maxChange: " << maxChange << endl;
	ABAdder hp;
	for(vector<pair<BushEdge*, BackwardGraphEdge*> >::iterator i = maxEdges.begin(); i != maxEdges.end(); ++i) {
		hp -= make_pair(i->second->costFunction(), i->second->getFlow());
	}
	
	for(vector<pair<BushEdge*, BackwardGraphEdge*> >::iterator i = minEdges.begin(); i != minEdges.end(); ++i) {
		hp += make_pair(i->second->costFunction(), i->second->getFlow());
	}
	SecantSolver<ABAdder> solver;
	double newFlow = solver.solve(hp, maxChange, 0);//Change in flow
	
	if(newFlow == 0) return;//No change
//	double eval = hp(newFlow);

/*	if(hp(newFlow) < -1) {
		cout << maxChange << ',' << newFlow << endl;
		newFlow = maxChange;
	}
	/*
	HACK: Because the equation may have no roots, we sometimes get silly
	results from it. Unfortunately, when we consider the different distinct
	min/max path segments separately, sometimes the solver says we should
	shift flow the wrong way, giving us false positives when we just test
	newFlow < 0. -0.1 is arbitrary, but it seems to distinguish the two cases
	*/
	
	if(newFlow > maxChange) newFlow = maxChange;
	//This is done in the solver now?
	
//	cout << "newFlow: " << newFlow <<endl;
	
	for(vector<pair<BushEdge*, BackwardGraphEdge*> >::iterator i = minEdges.begin(); i != minEdges.end(); ++i) {
		i->first->addFlow(newFlow);
		i->second->addFlow(newFlow);
		i->first->underlyingEdge()->setDistance((*i->second->costFunction())(i->second->getFlow()));
	}
	for(vector<pair<BushEdge*, BackwardGraphEdge*> >::iterator i = maxEdges.begin(); i != maxEdges.end(); ++i) {
		i->first->addFlow(-newFlow);
		i->second->addFlow(-newFlow);
		i->first->underlyingEdge()->setDistance((*i->second->costFunction())(i->second->getFlow()));
	}
}

void BushNode::equilibriate(ABGraph& graph)
{
	BushNode* minNode = this;
	BushNode* maxNode = this;

	/*
	NOTE: It is very important to equilibriate the different distinct segments
	of the min/max paths separately, or we get worst-case behaviour a'la
	Ford-Fulkerson
	*/
	while (true) {
		vector<pair<BushEdge*, BackwardGraphEdge*> > minEdges;
		vector<pair<BushEdge*, BackwardGraphEdge*> > maxEdges;
		double maxChange = numeric_limits<double>::infinity();
		
		
		if(!moreSeparatePaths(minNode, maxNode, graph)) return;
		//Indicates we're done or sets node positions to start of next segment
		
//		cout << "minNode: " << minNode->id << ", distance " << minNode->maxDistance << endl;
//		cout << "maxNode: " << maxNode->id << ", distance " << maxNode->maxDistance << endl;
		do { //Trace paths back, adding arcs to lists
			if(minNode->maxDistance >= maxNode->maxDistance) {
				BushEdge* pred = minNode->minPredecessor;
				BackwardGraphEdge* bge = graph.backward(pred->underlyingEdge());
				minEdges.push_back(make_pair(pred, bge));
				minNode = bge->fromNode();
//				cout << "minNode: " << minNode->id << ", distance " << minNode->maxDistance << ", flow: " << pred->flow() << ", length: " << pred->length() << endl;
//				cout << "Total edge flow: " << bge->getFlow() << ", calcDistance: " << (*bge->costFunction())(bge->getFlow()) << endl;
			} else {
				BushEdge* pred = maxNode->maxPredecessor;
				BackwardGraphEdge* bge = graph.backward(pred->underlyingEdge());
				maxChange = min(maxChange, pred->flow());
				maxEdges.push_back(make_pair(pred, bge));
				maxNode = bge->fromNode();
//				cout << "maxNode: " << maxNode->id << ", distance " << maxNode->maxDistance << ", flow: " << pred->flow() << ", length: " << pred->length() << endl;
//				cout << "Total edge flow: " << bge->getFlow() << ", calcDistance: " << (*bge->costFunction())(bge->getFlow()) << endl;
			}
		} while(minNode->id != maxNode->id);
		if(maxChange > 1e-12) fixDifferentPaths(minEdges, maxEdges, maxChange);
		//Should I return otherwise? Should I return when I see it for the first time?
		//Not that it matters too much, we spend < 3% of our time here and down.
		//Benchmark it.
	}
	//Probably the ugliest function in the program now.
}
