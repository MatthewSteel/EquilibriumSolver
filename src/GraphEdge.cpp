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


#include "GraphEdge.hpp"
#include "BushNode.hpp"
#include <limits>

using namespace std;

BackwardGraphEdge::BackwardGraphEdge(double d, BushNode *from) : from(from), _distance(d)
{}

BackwardGraphEdge::BackwardGraphEdge(const BackwardGraphEdge & e) : from(e.from), _distance(e._distance)
{}

ForwardGraphEdge::ForwardGraphEdge(InputGraph::VDF i, BushNode *to, BackwardGraphEdge* inverse):
	distanceFunction(i),
	inverse(inverse),
	to(to),
	flow(0)
{}

ForwardGraphEdge::ForwardGraphEdge(const ForwardGraphEdge& e):
	distanceFunction(e.distanceFunction),
	inverse(e.inverse),
	to(e.to),
	flow(e.flow)
{}
