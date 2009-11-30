#ifndef INPUT_GRAPH_HPP
#define INPUT_GRAPH_HPP

#include <map>
#include <utility>
#include <tr1/functional>

class InputGraph {
	public:
		InputGraph() {}
		typedef std::tr1::function<double(double)> VDF;
		void addEdge(unsigned from, unsigned to, std::tr1::function<double(double)> vdf) {
			_graph[from][to] = vdf;
		}
		void addDemand(unsigned from, unsigned to, double demand) {
			_demand[from][to] += demand;
		}
		const std::map<unsigned,std::map<unsigned, VDF> >& graph() const {
			return _graph;
		}
		const std::map<unsigned,std::map<unsigned, double> >& demand() const {
			return _demand;
		}
		unsigned numNodes() const { return nodes; }
		void setNodes(unsigned u) { nodes = u; }
		//TODO: fix/lock/finalise?
		
		// NOTE: specs should come from GraphImporter.cpp, ABGraph.cpp
		//and maybe some graph classes in TAPFramework.
	private:
		unsigned nodes;
		std::map<unsigned, std::map<unsigned, VDF> > _graph;
		std::map<unsigned, std::map<unsigned, double> > _demand;
		/*NOTE:
		- Seems reasonable to keep these things separate.
		- Should think about making a demand class instead of a
		std::pair, but this isn't too bad, really.
		- Maps don't help efficiency, but code cleanliness wins
		here - things to do with this graph go about 3-5s slower
		in total as a result, but total runtime is > 15mins. */

		/* Think about having refs to nodes instead of an indexes?
		Probably not a good idea. */
};

#endif
