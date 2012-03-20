#ifndef INPUT_GRAPH_HPP
#define INPUT_GRAPH_HPP

#include <map>
#include <utility>

#ifdef _MSC_VER
 #include <functional>
#elif defined __PATHCC__
 #include <boost/tr1/functional.hpp>
#else
 #include <tr1/functional>
#endif

class InputGraph {
	public:
		InputGraph() {}
		typedef std::tr1::function<double(double)> VDF;

		void addEdge(unsigned from, unsigned to, VDF vdf) {
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
		//map[from_node] --> (map[to_node] --> delay_func)
		
		std::map<unsigned, std::map<unsigned, double> > _demand;
		//map[from_node] --> (map[to_node] --> demand)
};

#endif
