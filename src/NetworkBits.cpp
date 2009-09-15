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


#include "NetworkBits.hpp"

EdgeStuff::EdgeStuff(const TAPFramework::Road& i, const TAPFramework::NetworkProperties& p) :
	flow (i.getFlow()),
	secondFlow(0.0),
	distanceFunction(BPRFunction(i.getBPRFunction()))
{
	distanceFunction.addCost(p.getLengthCost()*i.getLength() + i.getToll()*p.getTollCost());
	distance = distanceFunction(flow);
}

EdgeStuff::EdgeStuff(const EdgeStuff& e):
		distance(e.distance),
		flow(e.flow),
		secondFlow(0.0),
		distanceFunction(e.distanceFunction)
{}

void EdgeStuff::update(double lambda)
{
	flow = (lambda*secondFlow) + ((1-lambda)*flow);
	secondFlow = 0;
	distance = distanceFunction(flow);
}

void EdgeStuff::tempFlow(double flow)
{
	this->secondFlow += flow;
}
