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


#ifndef TRAFFIC_ASSIGNMENT_SOLVER_HPP
#define TRAFFIC_ASSIGNMENT_SOLVER_HPP

#include <limits>

#include <boost/shared_ptr.hpp>
#include <boost/graph/adjacency_list.hpp>


namespace TAPFramework {
	class Intersection;
	class Road;
	class NetworkProperties;

	/**
	 * Abstract base class defining the necessary operations on a solver
	 * for the Tester class to be able to properly initialise it and
	 * measure its convergence properties.
	 */

	class TrafficAssignmentSolver
	{
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Intersection, Road> Graph;

		public:
			/**
			 * Stores/copies the problem data into solver-specific
			 * data structures.
			 */
			virtual void setUp(boost::shared_ptr<Graph const>, const NetworkProperties&)=0;

			/**
			 * Solves the traffic assignment problem.
			 */
			virtual void solve(unsigned iterationLimit = std::numeric_limits<unsigned>::max(),
			                   double accuracy = std::numeric_limits<double>::infinity())=0;
		
			/**
			 * Copies the solver's solution data to the problem
			 * int a network passed in. It is assumed (on trust)
			 * no computation of note takes place in this method
			 * to "game" performance statistics.
			 */
			virtual void outputAnswer(boost::shared_ptr<Graph>) const=0;
			virtual ~TrafficAssignmentSolver(){}
	};

}

#endif
