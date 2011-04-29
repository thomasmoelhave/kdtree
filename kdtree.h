#ifndef KDTREE_H__
#define KDTREE_H__

#include <iostream>
#include <limits>
#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>
#include <iterator>

namespace kdtree {

//////////////////////////////////////////////////////
/// Defines a D-dimensional node in kd-tree
/// with point type T
template <typename Traits>
struct node {
	typedef typename Traits::point_type point_type;
	typedef typename Traits::box_type box_type;
	typedef node<Traits> node_type;

	const int D;

	node() : D(Traits::dimensions()) { }

	void build(std::vector<point_type>& point_cloud, Traits traits) {
		build(point_cloud,D,traits);
	}


	//Simple construction algorihm that cycles through the dimensions
	//it is neither memory nor cpu efficient.
	void build(std::vector<point_type>& point_cloud, unsigned int last_split, Traits traits) {
	//	#define DEBUGLOG(x) std::cout << x
		#define DEBUGLOG(x) 

		_node_id = _global_node_id++;

		DEBUGLOG("Building node from " << point_cloud.size() << " points.\n");

		//save bounding box
		for (size_t i = 0; i < point_cloud.size(); ++i) {
			_box.extend(point_cloud[i]);
		}

		//split at next dimension
		_split_dimension=(last_split+1)%D;
		
		DEBUGLOG ("\tSplitting along dimension " << _split_dimension << "\n");

		//find median (this alters the point_cloud vector), 
		//returns false if no more splitting should be done
		point_type median;
		if (!traits.compute_median(point_cloud,_split_dimension,median)) {
			DEBUGLOG("\tNot splitting further.\n");
			//copy points
			std::copy(point_cloud.begin(),point_cloud.end(),std::back_inserter(_points));
			return;
		}

		DEBUGLOG("\tSplitting along median " << median << "\n");
		
		std::vector<point_type> left;
		std::vector<point_type> right;
		
		//distribute points
		for (size_t i = 0; i < point_cloud.size(); ++i) {
			//put point in left or right box
			typename Traits::comparator_type cmp(_split_dimension);
			if (cmp(point_cloud[i],median))  {
				left.push_back(point_cloud[i]);
			} else {
				right.push_back(point_cloud[i]);
			}
		}

		DEBUGLOG(std::cout << "\tFinal bbox " << _box << "\n");

		_left.reset(new node_type);
		_right.reset(new node_type);

		_left->build(left,_split_dimension,traits);
		_right->build(right,_split_dimension,traits);
		#undef DEBUGLOG
	}

	//get a list of the points in the leaves in the current subtree
	//each point is augmented with its node id
	void leaf_points(std::vector<std::pair<size_t,point_type> >& v) const { 
		if (_points.empty()) {
			assert(_left.get() != 0 && _right.get() != 0);
			_left->leaf_points(v);
			_right->leaf_points(v);
		} else {
			for (size_t i = 0; i < _points.size(); ++i) {
				v.push_back(std::make_pair(_node_id,_points[i]));
			}
		}
	}

	private:

	static size_t _global_node_id;
	size_t _node_id;
	int _split_dimension;
	box_type _box;
	boost::scoped_ptr<node_type> _left;
	boost::scoped_ptr<node_type> _right;
	std::vector<point_type> _points;  //point in kd tree box (only for leaves)

	template <typename Traits_>
	friend std::ostream& operator<<(std::ostream& s, const node<Traits_>& n);
};


template <typename Traits> size_t node<Traits>::_global_node_id = 0;


template <typename Traits>
std::ostream& operator<<(std::ostream& s, const node<Traits>& n) {
	s << "id: " << n._node_id << " box: " << n._box << " volume: " << std::fixed << n._box.volume() << " size: " << n._points.size() << "\n";
	if (n._points.empty()) {
		assert(n._left.get() != 0 && n._right.get() != 0);
		s << *(n._left);
		s << *(n._right);
	} 
	return s;
}

}

#endif 
