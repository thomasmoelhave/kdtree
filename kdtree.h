#ifndef KDTREE_H__

#include <iostream>
#include <limits>
#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>
#include <iterator>

namespace kdtree {


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
	return s << "[" << i.a << "-" << i.b << "]";
}



//////////////////////////////////////////////////////
/// Defines a D-dimensional point with the type
/// of each dimension being T.
template <unsigned int D,typename T>
struct point {
	typedef T type;

	T& operator()(unsigned i) {
		assert(i < D);
		return _values[i]; 
	}
	
	const T& operator()(unsigned i) const { 
		assert(i < D);
		return _values[i]; 
	}

	///Defines a lexicographi comparator where dimension "S" is the main
	///key used in the comparison
	struct comparator : public std::binary_function<point<D,T>,point<D,T>,bool> {
		typedef point<D,T> point_type;


		comparator(unsigned int idx) : _idx(idx) { }

		bool operator()(const point_type& lhs, const point_type& rhs) {
			for (unsigned int i = 0; i < D; ++i) {
				unsigned int adj = (i+_idx)%D;
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
			unsigned int _idx;
	};

private:
	boost::array<T,D> _values;
};

template <unsigned int D,typename T>
std::ostream& operator<<(std::ostream& s, const point<D,T>& p) {
	s << "(";
	for (unsigned int i=0; i < D; ++i) {
		if (i>0) {
			s << ", ";
		}
		s << p(i);
	}
	s << ")";
	return s;
}


//////////////////////////////////////////////////////
/// Defines a D-dimensionsional box
template <unsigned int D, typename T>
struct box {
	typedef point<D,T> point_type;
	typedef interval<T> interval_type;

	void extend(point_type& p) {
		for (unsigned int i = 0; i < D; ++i) {
			this->operator()(i).extend(p(i));
		}
	}
	
	interval_type& operator()(unsigned i) {
		assert(i < D);
		return _extents[i]; 
	}
	
	const interval_type& operator()(unsigned i) const { 
		assert(i < D);
		return _extents[i]; 
	}

	const T volume() const {
		T vol=1;
		for (unsigned int i=0; i < D; ++i) {
			vol *= this->operator()(i).length();
		}
		return vol;
	}

	private:
		boost::array<interval_type,D> _extents;
};

template <unsigned int D,typename T>
std::ostream& operator<<(std::ostream& s, const box<D,T>& b) {
	s << "{";
	for (unsigned int i=0; i < D; ++i) {
		if (i>0) {
			s << ", ";
		}
		s << b(i);
	}
	s << "}";
	return s;
}


//////////////////////////////////////////////////////
/// Defines a D-dimensional node in kd-tree
/// with point type T
template <unsigned int D, typename T, typename Strategy>
struct node {
	typedef node<D,T,Strategy> node_type;
	typedef point<D,T> point_type;
	typedef box<D,T> box_type;

	void build(std::vector<point_type>& point_cloud, Strategy strategy) {
		build(point_cloud,D-1,strategy);
	}


	//Simple construction algorihm that cycles through the dimensions
	//it is neither memory nor cpu efficient.
	void build(std::vector<point_type>& point_cloud, unsigned int last_split, Strategy strategy) {
	//	#define DEBUGLOG(x) std::cout << x
		#define DEBUGLOG(x) 

		DEBUGLOG("Building node from " << point_cloud.size() << " points.\n");

		//save bounding box
		for (size_t i = 0; i < point_cloud.size(); ++i) {
			_box.extend(point_cloud[i]);
		}

		if (point_cloud.size() <= strategy.max_size()) {
			DEBUGLOG("\tNot splitting further.\n");
			//copy points
			std::copy(point_cloud.begin(),point_cloud.end(),std::back_inserter(_points));
			return;
		}

		//split at next dimension
		_split_dimension=(last_split+1)%D;
		
		DEBUGLOG ("\tSplitting along dimension " << _split_dimension << "\n");

		//find median (this alters the point_cloud vector
		point_type median = strategy.compute_median(point_cloud,_split_dimension);

		DEBUGLOG("\tSplitting along median " << median << "\n");
		
		std::vector<point_type> left;
		std::vector<point_type> right;
		
		//distribute points
		for (size_t i = 0; i < point_cloud.size(); ++i) {
			//put point in left or right box
			typename point_type::comparator cmp(_split_dimension);
			if (cmp(point_cloud[i],median))  {
				left.push_back(point_cloud[i]);
			} else {
				right.push_back(point_cloud[i]);
			}
		}

		DEBUGLOG(std::cout << "\tFinal bbox " << _box << "\n");

		_left.reset(new node_type);
		_right.reset(new node_type);

		_left->build(left,_split_dimension,strategy);
		_right->build(right,_split_dimension,strategy);
		#undef DEBUGLOG
	}

	private:


	int _split_dimension;
	box_type _box;
	boost::scoped_ptr<node_type> _left;
	boost::scoped_ptr<node_type> _right;
	std::vector<point_type> _points;  //point in kd tree box (only for leaves)

	template <unsigned int D_,typename T_, typename Strategy_>
	friend std::ostream& operator<<(std::ostream& s, const node<D_,T_,Strategy_>& n);
};


template <unsigned int D,typename T, typename Strategy>
std::ostream& operator<<(std::ostream& s, const node<D,T,Strategy>& n) {
	static int id = 0;
	if (n._points.empty()) {
		assert(n_left.get() != 0 && n._right.get() != 0);
		s << *(n._left);
		s << *(n._right);
	} else {
		s << "Meta: " << "box: " << n._box << " volume: " << n._box.volume() << " size: " << n._points.size() << "\n";
		
		for (size_t i = 0; i < n._points.size(); ++i) {
			s << id;
			for (unsigned int d = 0; d < D; ++d) {
				s << " " << n._points[i](d);
			}
			s << "\n";
		}
		//s << "\n";
		++id;
	}
	return s;
}

}

#endif 
