#include "BushEdge.hpp"
#include "ABGraph.hpp"

void BushEdge::swapDirection(ABGraph &g) {

	realEdge = g.backward(realEdge)->getInverse();

}
