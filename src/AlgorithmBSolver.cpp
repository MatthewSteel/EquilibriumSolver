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
#include "FrankWolfeSolver.hpp"

using namespace std;
using namespace boost;

class BushEdge;

void AlgorithmBSolver::setUp(shared_ptr<InputGraph const> g, const TAPFramework::NetworkProperties& p)
{
	graph.reset(new ABGraph(num_vertices(*g)));
	
	ODData.reset(new list<Origin>());
	
	//set up bush shared data
	size_t n = num_vertices(*g);
	v = new vector<BushNode>();
	v->reserve(n);
	for(unsigned i = 0; i < n; ++i) {
		v->push_back(BushNode(i));
	}
	
	InputGraph::edge_iterator inBegin, inEnd;
	tie(inBegin, inEnd) = edges(*g);
	//Mirror the basic graph structure (with some extra stuff):
	for(InputGraph::edge_iterator j = inBegin; j != inEnd; ++j) {
		graph->add(GraphEdge((*g)[*j], p, v->at(source(*j, *g)), v->at(target(*j, *g)), target(*j,*g)));
		pair<graph_traits<InputGraph>::edge_descriptor, bool> e = edge(target(*j,*g), source(*j,*g), *g);
		if(!e.second) {//Imaginary arc setup
			graph->add(GraphEdge(v->at(target(*j, *g)), v->at(source(*j, *g)), source(*j,*g)));
		}
	}
	
	ABGraph::EdgeIterator b = graph->begin(), e = graph->end();
	for(ABGraph::EdgeIterator i = b; i != e; ++i) {
		GraphEdge& edge = *i;
		graph->edge(edge.toNode()->getId(), edge.fromNode()->getId())->setInverse(&edge);
	}

	for(unsigned i = 0; i < num_vertices(*g); ++i) {
		const list<pair<unsigned, double> >& l = (*g)[vertex(i, *g)].dests();
		if(!l.empty()) {
			Origin o(i);
			for(list<pair<unsigned, double> >::const_iterator j = l.begin(); j != l.end(); ++j)
				o.addDestination(j->first, j->second);
			ODData->push_back(o);
		}
	}
	tempStore = new vector<pair<double, unsigned> >(n);
	for(list<Origin>::iterator i = ODData->begin(); i != ODData->end(); ++i) {
		bushes.push_back(new Bush(*i, *graph, *v, *tempStore));
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
		GraphEdge* e = graph->edge(source(*i, *inGraph), target(*i, *inGraph));
		if (e) {
			(*inGraph)[*i].updateFlow(e->getFlow());
		}
	}
}

int AlgorithmBSolver::getCount()
{
	int count = 0;
	for(list<Bush*>::iterator i = lazyBushes.begin(); i != lazyBushes.end(); ++i) {
		count += (*i)->giveCount();
	}
	return count;
}

AlgorithmBSolver::~AlgorithmBSolver()
{
	if(v) delete v;
	if(tempStore) delete tempStore;
	for(list<Bush*>::iterator i = bushes.begin(); i != bushes.end(); ++i)
		delete *i;
	for(list<Bush*>::iterator i = lazyBushes.begin(); i != lazyBushes.end(); ++i)
		delete *i;
}
