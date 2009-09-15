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


#ifndef PREDECESSOR_RECORDER_HPP
#define PREDECESSOR_RECORDER_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "NetworkBits.hpp"

class PredecessorRecorder : public boost::dijkstra_visitor<>
{
	public:
		template<class Edge, class Graph>
		void edge_relaxed(Edge& e, Graph& g) {
			g[boost::target(e,g)].previousEdge = e;
		}
};

#endif
