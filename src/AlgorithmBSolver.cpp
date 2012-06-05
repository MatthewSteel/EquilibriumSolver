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
#include <iostream>
#include <map>

using namespace std;

class BushEdge;

AlgorithmBSolver::AlgorithmBSolver(const InputGraph& g): graph(g), tempStore(graph.nodes().size()), reverseTS(g.numNodes())
{
	//NOTE: A little heavy work in the graph ctor in the init list.
	//Read ODData out of graph
	for(map<unsigned,map<unsigned, double> >::const_iterator i = g.demand().begin(); i != g.demand().end(); ++i) {
		unsigned index = i->first;
		Origin o(index);
		ODData.push_back(o);
		for(map<unsigned,double>::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			ODData.back().addDestination(j->first, j->second);
		}
	}
	
/*	for(unsigned i = 0; i < num_vertices(*g); ++i) {
		const list<pair<unsigned, double> >& l = (*g)[vertex(i, *g)].dests();
		if(!l.empty()) {
			Origin o(i);
			ODData.push_back(i);
			for(list<pair<unsigned, double> >::const_iterator j = l.begin(); j != l.end(); ++j)
				ODData.back().addDestination(j->first, j->second);
		}
	}*/
	//Set up a bush for every origin. Most of the work is in here - Dijkstra over the graph in the Bush ctor.
	for(list<Origin>::iterator i = ODData.begin(); i != ODData.end(); ++i) {
		bushes.push_back(new Bush(*i, graph, tempStore, reverseTS));
	}
}

bool AlgorithmBSolver::fixBushSets(list<Bush*>& fix, list<Bush*>& output, double average, bool whetherMove)
{
	//TODO: Replace with std::partition and list.splice when we get lambdas (C++0x).
	vector<list<Bush*>::iterator> move;
	for(list<Bush*>::iterator i = fix.begin(); i != fix.end(); ++i) {
		if((*i)->fix(average) == whetherMove) move.push_back(i);
	}
	for(vector<list<Bush*>::iterator>::iterator i = move.begin(); i != move.end(); ++i) {
		output.push_back(**i);
		fix.erase(*i);
	}//promote
	return output.empty();
}

void AlgorithmBSolver::solve(unsigned iterationLimit)
{
	//TODO: Change this to something better than .25*avg (probably nth_element)
	
	
	double sum = 0.0;
	for(list<Bush*>::iterator i = bushes.begin(); i != bushes.end(); ++i) 
		sum += (*i)->maxDifference();
	for(list<Bush*>::iterator i = lazyBushes.begin(); i != lazyBushes.end(); ++i)
		sum += (*i)->maxDifference();

	double average= 0.25*sum / ((double)(bushes.size() + lazyBushes.size()));
	
	for(unsigned iteration = 0; iteration < iterationLimit; ++iteration) {
		if(iteration % 4 == 3) {
			if(fixBushSets(lazyBushes, bushes, average, true)) return;
		}
		fixBushSets(bushes, lazyBushes, average, false);
	}
}

/*void AlgorithmBSolver::outputAnswer(shared_ptr<InputGraph> inGraph) const
{
	/*
	NOTE: Relies on the first inGraph.numEdges of our edges lining up with
	theirs. We have imaginary edges in our graph structure, so this may break
	in the future? If so, we can always just work it by vertex pairs.
	* /
	InputGraph::edge_iterator outBegin, outEnd;
	tie(outBegin, outEnd) = edges(*inGraph);
	
	InputGraph::edge_iterator i;
	for(i = outBegin; i != outEnd; ++i) {
		GraphEdge* e = graph.edge(source(*i, *inGraph), target(*i, *inGraph));
		if (e) {
			(*inGraph)[*i].updateFlow(e->getFlow());
		}
	}
}*/

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

double AlgorithmBSolver::averageExcessCost()
{
	double upperBound = graph.currentCost();
	double lowerBound = 0.0;
	for(list<Bush*>::iterator i = bushes.begin(); i != bushes.end(); ++i)
		lowerBound += (*i)->allOrNothingCost();
	for(list<Bush*>::iterator i = lazyBushes.begin(); i != lazyBushes.end(); ++i)
		lowerBound += (*i)->allOrNothingCost();
	double demand = 0;
	for(list<Origin>::iterator i = ODData.begin(); i != ODData.end(); ++i) {
		for(vector<pair<int,double> >::const_iterator j = i->dests().begin(); j != i->dests().end(); ++j) {
			demand += j->second;
		}
	}
	return (upperBound-lowerBound)/demand;
}
AlgorithmBSolver::~AlgorithmBSolver()
{
	for(list<Bush*>::const_iterator i = bushes.begin(); i != bushes.end(); ++i)
		delete *i;
	for(list<Bush*>::const_iterator i = lazyBushes.begin(); i != lazyBushes.end(); ++i)
		delete *i;
}
