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


#include "AlgorithmBSolver.hpp"

#include <memory>
#include <algorithm> //For max
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <iostream>

using namespace std;
using namespace boost;

class BushEdge;

AlgorithmBSolver::AlgorithmBSolver(shared_ptr<InputGraph const> g, const TAPFramework::NetworkProperties& p): v(0), graph(*g, p), tempStore(num_vertices(*g))
{
	//NOTE: A little heavy work in the graph ctor in the init list.
	//Read ODData out of graph
	for(unsigned i = 0; i < num_vertices(*g); ++i) {
		const list<pair<unsigned, double> >& l = (*g)[vertex(i, *g)].dests();
		if(!l.empty()) {
			Origin o(i);
			ODData.push_back(i);
			for(list<pair<unsigned, double> >::const_iterator j = l.begin(); j != l.end(); ++j)
				ODData.back().addDestination(j->first, j->second);
		}
	}
	//Set up a bush for every origin. Most of the work is in here - Dijkstra over the graph in the Bush ctor.
	for(list<Origin>::iterator i = ODData.begin(); i != ODData.end(); ++i) {
		bushes.push_back(new Bush(*i, graph, tempStore));
	}
}

void AlgorithmBSolver::solve(unsigned iterationLimit, double accuracy)
{
	for(unsigned iteration = 0; iteration < iterationLimit; ++iteration) {
		vector<list<Bush*>::iterator> demoted;
		for(list<Bush*>::iterator i = bushes.begin(); i != bushes.end(); ++i) {
			/*bool localFlowChanged = false;
			do
				localFlowChanged = (*i)->equilibriateFlows(accuracy) | localFlowChanged;
			while (!((*i)->updateEdges()));*/
			if(!(*i)->fix(accuracy)) demoted.push_back(i);
		}
		
		for(vector<list<Bush*>::iterator>::iterator i = demoted.begin(); i != demoted.end(); ++i) {
			lazyBushes.push_back(*(*i));
			bushes.erase(*i);
		}//demote
		
		if(iteration % 4 == 3) {
			//Curiously, replacing this if statement with a for loop (to 3) above changes the results and running time.
			vector<list<Bush*>::iterator> promoted;
			for(list<Bush*>::iterator i = lazyBushes.begin(); i != lazyBushes.end(); ++i) {
				/*bool localFlowChanged = false;
				do
					localFlowChanged = (*i)->equilibriateFlows(accuracy) | localFlowChanged;
				while (!((*i)->updateEdges()));
				*/
				if((*i)->fix(accuracy)) promoted.push_back(i);
			}
			for(vector<list<Bush*>::iterator>::iterator i = promoted.begin(); i != promoted.end(); ++i) {
				bushes.push_back(**i);
				lazyBushes.erase(*i);
			}//promote
			if(bushes.empty()) break;
		}
	}
}

void AlgorithmBSolver::outputAnswer(shared_ptr<InputGraph> inGraph) const
{
	/*
	NOTE: Relies on the first inGraph.numEdges of our edges lining up with
	theirs. We have imaginary edges in our graph structure, so this may break
	in the future? If so, we can always just work it by vertex pairs.
	*/
	InputGraph::edge_iterator outBegin, outEnd;
	tie(outBegin, outEnd) = edges(*inGraph);
	
	InputGraph::edge_iterator i;
	for(i = outBegin; i != outEnd; ++i) {
		GraphEdge* e = graph.edge(source(*i, *inGraph), target(*i, *inGraph));
		if (e) {
			(*inGraph)[*i].updateFlow(e->getFlow());
		}
	}
}

double AlgorithmBSolver::relativeGap()
{
	double upperBound = graph.currentCost();
	double lowerBound = 0.0;
	for(list<Bush*>::iterator i = bushes.begin(); i != bushes.end(); ++i)
		lowerBound += (*i)->allOrNothingCost();
	for(list<Bush*>::iterator i = lazyBushes.begin(); i != lazyBushes.end(); ++i)
		lowerBound += (*i)->allOrNothingCost();
	return 1-lowerBound/upperBound;
}

int AlgorithmBSolver::getCount() const
{
	int count = 0;
	for(list<Bush*>::const_iterator i = bushes.begin(); i != bushes.end(); ++i)
		count += (*i)->giveCount();
	for(list<Bush*>::const_iterator i = lazyBushes.begin(); i != lazyBushes.end(); ++i)
		count += (*i)->giveCount();
	return count;
}

AlgorithmBSolver::~AlgorithmBSolver()
{
	if(v) delete v;
	for(list<Bush*>::const_iterator i = bushes.begin(); i != bushes.end(); ++i)
		delete *i;
	for(list<Bush*>::const_iterator i = lazyBushes.begin(); i != lazyBushes.end(); ++i)
		delete *i;
}
