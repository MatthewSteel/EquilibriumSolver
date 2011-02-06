//Using this instead of a vector<BushEdge>, trading off insertion/deletion speed for half the memory overhead.
//Gives us a few second speedup in Chicago Regional, and a saving of 200-400MB of memory. Worth it.

#ifndef EDGE_VECTOR_HPP
#define EDGE_VECTOR_HPP

class EdgeVector {
	public:
		EdgeVector() : _begin(0), size(0) {}
		void push_back(BushEdge b) {
			BushEdge *be = new BushEdge[size+1];
			for(unsigned i = 0; i < size; ++i) {
				be[i] = _begin[i];
			}
			be[size] = b;
			++size;
			delete[] _begin;
			_begin = be;
		}
		void resize(unsigned n) {
			size -=n;
			BushEdge *be = new BushEdge[size];
			for(unsigned i = 0; i < size; ++i) {
				be[i] = _begin[i];
			}
			delete[] _begin;
			_begin = be;
		}
		~EdgeVector() { delete[] _begin; }
		BushEdge* begin() { return _begin; }
		BushEdge* end() { return _begin+size; }
		BushEdge& operator[](unsigned i) { return _begin[i]; }
		BushEdge& at(unsigned i) {
			if (i >= size) {
				throw "Out of range";
			} else return _begin[i];
		}
		bool empty() { return size==0; }
		int length() { return size; }
	private:
		BushEdge* _begin;
		unsigned size;
};

#endif
