#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

namespace primitives {

//////////////////////////////////////////////////////
/// Defines a 1-dimensional interval with 
/// endpoints of type T.
template <typename T>
struct interval {
	interval()
		: a(std::numeric_limits<T>::max()),
		 b(std::numeric_limits<T>::min())
	{ }

	bool is_valid() const {
		return a < b;
	}
	
	void extend(const T& x) {
		if (x < a) a=x;
		if (x > b) b=x;
	}

	T length() const {
		assert(is_valid());
		return b-a;
	}

	T a,b;
};

template <typename T>
std::ostream& operator<<(std::ostream& s, const interval<T>& i) {
	return s << "[" << i.a << "," << i.b << "]";
}



//////////////////////////////////////////////////////
/// Defines a D-dimensional point with the type
/// of each dimension being T.
template <size_t D,typename T>
struct point {
	point() { for (size_t i = 0; i < dimensions(); ++i) { _values[i]=-9999; } }

	T& operator()(size_t i) {
		assert(i < D);
		return _values[i]; 
	}
	
	const T& operator()(size_t i) const { 
		assert(i < D);
		return _values[i]; 
	}

	static size_t dimensions() { return D; }

	///Defines a lexicographic comparator where dimension "S" is the main
	///key used in the comparison
	struct lexicographic_comparator : public std::binary_function<point<D,T>,point<D,T>,bool> {
		typedef point<D,T> point_type;

		lexicographic_comparator(size_t idx) : _idx(idx) { }

		bool operator()(const point_type& lhs, const point_type& rhs) {
			for (size_t i = 0; i < D; ++i) {
				size_t adj = (i+_idx)%D;
				if (lhs(adj) < rhs(adj)) {
					return true;
				} else if (lhs(adj) > rhs(adj)) {
					return false;
				}
				//tie move to next dimension
			}

			//at this point we know that lhs==rhs
			return false;
		}
		private:
			size_t _idx;
	};

private:
	boost::array<T,D> _values;
};

template <size_t D,typename T>
std::ostream& operator<<(std::ostream& s, const point<D,T>& p) {
	s << "";
	for (size_t i=0; i < p.dimensions(); ++i) {
		if (i>0) {
			s << ",";
		}
		s << p(i);
	}
	s << "";
	return s;
}



//////////////////////////////////////////////////////
/// Defines a D-dimensionsional box
template <size_t D, typename T>
struct box {
	typedef point<D,T> point_type;
	typedef interval<T> interval_type;

	void extend(point_type& p) {
		for (size_t i = 0; i < D; ++i) {
			this->operator()(i).extend(p(i));
		}
	}
	
	interval_type& operator()(size_t i) {
		assert(i < D);
		return _extents[i]; 
	}
	
	const interval_type& operator()(size_t i) const { 
		assert(i < D);
		return _extents[i]; 
	}

	const T volume() const {
		T vol=1;
		for (size_t i=0; i < D; ++i) {
			vol *= this->operator()(i).length();
		}
		return vol;
	}

	private:
		boost::array<interval_type,D> _extents;
};

template <size_t D,typename T>
std::ostream& operator<<(std::ostream& s, const box<D,T>& b) {
	s << "";
	for (unsigned int i=0; i < D; ++i) {
		if (i>0) {
			s << "x";
		}
		s << b(i);
	}
	s << "";
	return s;
}

}

#endif
