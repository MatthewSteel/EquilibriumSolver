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


#include "FrankWolfeSolver.hpp"
#include "HornerPolynomial.hpp"
#include "Origin.hpp"
#include "SecantSolver.hpp"
#include "BisectionSolver.hpp"
#include "PredecessorRecorder.hpp"
#include "NaiveAdder.hpp"

#include <numeric>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace std;
using namespace boost;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, EdgeHolder, EdgeStuff> Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::edge_descriptor Edge;

void FrankWolfeSolver::setUp (shared_ptr<InputGraph const> g, const TAPFramework::NetworkProperties& p)
{
	bool warm = false;
	graph.reset(new Graph(num_vertices(*g)));
	ODData.reset(new vector<Origin>());
	InputGraph::edge_iterator inBegin, inEnd;
	tie(inBegin, inEnd) = edges(*g);
	for(InputGraph::edge_iterator i = inBegin; i != inEnd; ++i) {
		warm =  warm || ((*g)[*i].getFlow()!=0);
		add_edge(source(*i, *g), target(*i, *g), EdgeStuff((*g)[*i], p), *graph);
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
	
	//Initial flows
	if(!warm) {
		Graph::edge_iterator begin, end;
		tie(begin, end) = edges(*graph);
		allOrNothing();
		for(Graph::edge_iterator i = begin; i != end; ++i)
			(*graph)[*i].update(); //Update weights
	}
}

void FrankWolfeSolver::allOrNothing()
{
	for(vector<Origin>::iterator i = ODData->begin(); i != ODData->end(); ++i) {
		
		PredecessorRecorder pr;
		dijkstra_shortest_paths(
			*graph,
			vertex(i->getOrigin(), *graph),
			visitor(pr).
			weight_map(get(&EdgeStuff::distance, *graph))
		);
		
		for(vector<pair<int,double> >::const_iterator j = i->dests().begin(); j != i->dests().end(); ++j) {
			double flow = j->second;
			for(Vertex v = vertex(j->first, *graph); v != vertex(i->getOrigin(), *graph); v = opposite((*graph)[v].previousEdge, v, (*graph))) {
				(*graph)[(*graph)[v].previousEdge].tempFlow(flow);
			}
		}
	}
}

void FrankWolfeSolver::solve(unsigned iterationLimit, double accuracy)
{
	//Initial all-or-nothing assignment:
	
	SecantSolver solver;
	double lambda = 1.0;
	unsigned iteration = 1;//initial assignment is iteration 1
	
	Graph::edge_iterator begin, end;
	tie(begin, end) = edges(*graph);
//	cout << (*this) << endl;
	while(lambda > accuracy && iteration < iterationLimit) {
		
		allOrNothing();

		HornerPolynomial function;
		for(Graph::edge_iterator i = begin; i != end; ++i)
			function += *(((*graph)[*i]).integralFunction());

//		cout << function << endl;
		lambda = solver.solve(function);
//		cout << lambda << endl;
		for(Graph::edge_iterator i = begin; i != end; ++i)
			(*graph)[*i].update(lambda);
//		cout << (*this) << endl;
		iteration++;
	}
}

double FrankWolfeSolver::relativeGap()
{
	allOrNothing();
	Graph::edge_iterator begin, end;
	tie(begin, end) = edges(*graph);
	double upperBound = 0.0;
	double lowerBound = 0.0;
	for(Graph::edge_iterator i = begin; i != end; ++i) {
		upperBound += (*graph)[*i].getFlow() * (*graph)[*i].distance;
		lowerBound +=  (*graph)[*i].getAugmentingFlow() * (*graph)[*i].distance;
	}
	return 1-lowerBound/upperBound;
}
double FrankWolfeSolver::averageExcessCost()
{
	allOrNothing();
	Graph::edge_iterator begin, end;
	tie(begin, end) = edges(*graph);
	double numerator = 0.0;
	for(Graph::edge_iterator i = begin; i != end; ++i) {
		numerator += (*graph)[*i].getFlow() * (*graph)[*i].distance;
		numerator -=  (*graph)[*i].getAugmentingFlow() * (*graph)[*i].distance;
	}
	double denominator = 0.0;
	for(vector<Origin>::const_iterator i = ODData->begin(); i != ODData->end(); ++i)
		for(vector<pair<int, double> >::const_iterator j = i->dests().begin(); j != i->dests().end(); ++j)
			denominator += j->second;
	
	return numerator/denominator;
}

void FrankWolfeSolver::outputAnswer(shared_ptr<InputGraph> inGraph) const
{
	InputGraph::edge_iterator outBegin, outEnd;
	tie(outBegin, outEnd) = edges(*inGraph);
	
	Graph::edge_iterator ourBegin, ourEnd;
	tie(ourBegin, ourEnd) = edges(*graph);
	
	InputGraph::edge_iterator i;
	Graph::edge_iterator j = ourBegin;
	for(i = outBegin; i != outEnd; ++i, ++j)
		(*inGraph)[*i].updateFlow((*graph)[*j].getFlow());
}
