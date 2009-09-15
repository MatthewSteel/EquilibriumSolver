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


#ifndef TESTER_HPP
#define TESTER_HPP

#include <string>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/timer.hpp>

#include "TAPFramework/TrafficAssignmentSolver.hpp"
#include "TAPFramework/NetworkProperties.hpp"

namespace TAPFramework {


class Intersection;
class Road;
class NetworkProperties;

/**
 * Provides methods to set up road networks and evaluate the efficacy of
 * solvers for the Traffic Assignment Problem. This class measures
 * solution time and solution quality.
 */

class Tester
{
	typedef boost::adjacency_list<
		boost::vecS,
		boost::vecS,
		boost::bidirectionalS,
		TAPFramework::Intersection,
		TAPFramework::Road> Graph;
	
	public:
		Tester(TrafficAssignmentSolver& solver) : timeToSolve(0), tas(solver) {};
		Tester(TrafficAssignmentSolver& solver, std::string s, std::string t) : tas(solver) {setGraph(s,t);};
		
		
		/**
		 * Sends the network data in the files named in the first two
		 * parameters to the solver. Because a solver might perform
		 * nontrivial operations in this state ("cheating"), the time
		 * (in seconds) taken by the is solver to get the data
		 * properly stored is returned.
		 */
		double setGraph(std::string networkFile, std::string TripsFile, NetworkProperties np = NetworkProperties());

		/**
		 * Tells the solver to solve the problem to a given accuracy
		 * or up to a given number of iterations (whichever is reached
		 * first, by convention.)
		 * @param iterationLimit The number of iterations the solver
		 *        may take. A hint, this parameter may be unused by
		 *        the solver. Use the getTime() function to determine
		 *        running time.
		 * @param accuracy The desired accuracy. A hint, this
		 *        parameter may be unused by the solver. Use the
		 *        getRelativeGap() or getAverageExcessCosts()
		 *        functions to determine the quality of the solution.
		 */
		void test(unsigned iterationLimit, double accuracy);

		/**
		 * Returns the total time (in seconds) the solver has spent
		 * in the test method.
		 */
		double getTime() { return timeToSolve; }

		/**
		 * Returns the relative gap - a measure of solution quality.
		 * Lower is better. This method may take some time to execute
		 * on large networks.
		 */
		double getRelativeGap() {
			tas.outputAnswer(graph);
			return relativeGap();
		}
		/**
		 * Returns the average excess costs- a measure of solution
		 * quality. Lower is better. This method may take some time to
		 * execute on large networks.
		 */
		double getAverageExcessCosts() {
			tas.outputAnswer(graph);
			return averageExcessCosts();
		}
	private:
		double relativeGap();
		double averageExcessCosts();
		double timeToSolve;
		boost::shared_ptr<Graph> graph;
		NetworkProperties properties;
		TrafficAssignmentSolver& tas;
};

}

#endif
