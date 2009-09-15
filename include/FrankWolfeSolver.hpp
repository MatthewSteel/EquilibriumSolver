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



//FrankWolfeSolver.hpp

#ifndef FRANK_WOLFE_SOLVER_HPP
#define FRANK_WOLFE_SOLVER_HPP

#include <list>
#include <limits>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map.hpp>
#include <boost/tuple/tuple.hpp> //For tie

#include "TAPFramework/Road.hpp"
#include "TAPFramework/Intersection.hpp"
#include "TAPFramework/NetworkProperties.hpp"
#include "TAPFramework/TrafficAssignmentSolver.hpp"

#include "NetworkBits.hpp"

class Origin;

class FrankWolfeSolver : public TAPFramework::TrafficAssignmentSolver
{
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, EdgeHolder, EdgeStuff> Graph;
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, TAPFramework::Intersection, TAPFramework::Road> InputGraph;
	
	public:
		FrankWolfeSolver(){}
		FrankWolfeSolver(boost::shared_ptr<InputGraph const> g, const TAPFramework::NetworkProperties& p = TAPFramework::NetworkProperties()) { setUp(g,p); }
		void setUp(boost::shared_ptr<InputGraph const>, const TAPFramework::NetworkProperties&);
		void solve(unsigned iterationLimit = std::numeric_limits<unsigned>::max(), double accuracy = 0);
		void outputAnswer(boost::shared_ptr<InputGraph>) const;
		double relativeGap();
		double averageExcessCost();

		friend std::ostream& operator<<(std::ostream& o, FrankWolfeSolver& e)
		{
			Graph::edge_iterator begin, end; //Different class name?
			boost::tie(begin, end) = boost::edges(*(e.graph));
			for(Graph::edge_iterator i = begin; i != end; ++i)
				o << "Edge (" << boost::source(*i, *e.graph)<< ',' << boost::target(*i, *e.graph) << "), " << (*(e.graph))[*i] << '\n';
			return o;
		}//FIXME
		
	private:
		void allOrNothing();
		boost::shared_ptr<Graph> graph;
		boost::shared_ptr<std::vector<Origin> > ODData;
};

#endif
