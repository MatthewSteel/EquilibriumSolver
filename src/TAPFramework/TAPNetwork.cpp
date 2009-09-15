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


#include "TAPNetwork.hpp"
#include "Road.hpp"

using namespace TAPFramework;
using namespace std;

void TAPNetwork::add(const Intersection& node)
{
	vertexMapType::left_iterator i = vertexMap.left.find(node.id());
	if (i == vertexMap.left.end()) {
		//Intersection's id is unique so far, add the intersection to the graph
		vertexMap.left.insert(node.id(), boost::add_vertex(node, graph));
	} else {
		Graph[*i] = node;
		//id is already in the graph. Update its properties
	}
}

void TAPNetwork::add(const Road& arc)
{
	vertexMapType::left_iterator i = lookUpNode(arc.fromId());
	vertexMapType::left_iterator j = lookUpNode(arc.toId());

	pair<Edge, bool> r = boost::add_edge(*i, *j, arc, graph);
	if(!r.second) graph[r.first] = arc;
	//Was already in there, just need to update its properties
}

void TAPNetwork::add(const ODPair& p)
{
	vertexMapType::left_iterator i = lookUpNode(p.fromId());
	vertexMapType::left_iterator j = lookUpNode(p.toId());

	graph[*i].addDest(*j, p.demand());
}

vertexMapType::left_iterator TAPNetwork::lookUpNode(const string& s)
{
	vertexMapType::left_iterator i = vertexMap.left.find(s);
	if (i == vertexMap.left.end()) {
		//id is unique so far, add the intersection to the graph
		i = (vertexMap.left.insert(s, boost::add_vertex(graph))).first;
	}
	return i;
}
