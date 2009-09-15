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

GraphEdge::GraphEdge(const TAPFramework::Road& i, const TAPFramework::NetworkProperties& p, BushNode& from, BushNode& to, unsigned toId) :
	inverse(0),
	flow (0),
	distanceFunction(*BPRFunction(i.getBPRFunction()).costFunction()),
	from(&from),
	to(&to),
	toId(toId)
{
	distanceFunction += (p.getLengthCost()*i.getLength() + i.getToll()*p.getTollCost());
	distance = distanceFunction(flow);
}

GraphEdge::GraphEdge(const GraphEdge& e):
		distance(e.distance),
		inverse(e.inverse),
		flow(e.flow),
		distanceFunction(e.distanceFunction),
		from(e.from),
		to(e.to),
		toId(e.toId)
{}
