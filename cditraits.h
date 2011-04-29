#ifndef CDITRAITS_H_
#define CDITRAITS_H_

#include <iostream>
#include <limits>
#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>
#include <iterator>
#include "primitives.h"

namespace traits {

template<size_t D, typename T>
struct site : public primitives::point<D,T> {

	const std::string& attribute(size_t i) const {
		return _attributes(i);
	}
	std::string& attribute(size_t i) {
		return _attributes(i);
	}

	size_t attributes() const {
		return _attributes.dimensions();
	}

	const int& year() const {
		return _year;
	}

	int& year() {
		return _year;
	}
	private:
		primitives::point<2,std::string> _attributes;
		int _year;
};

template<size_t D, typename T>
class CDITraits {
public:

	//Defines the order on the points
	//typedef primitives::point<D,T> point_type;
	typedef site<D,T> point_type;
	typedef typename point_type::lexicographic_comparator comparator_type;
	typedef primitives::box<D,T> box_type;

	CDITraits(size_t min_size, int min_year, int max_year) 
		: _min_size(min_size), _min_year(min_year), _max_year(max_year)
		{ }

	void count_years (
		typename std::vector<point_type>::const_iterator begin, 
		typename std::vector<point_type>::const_iterator end,
		std::vector<size_t>& yearcounts) const
	{
		while (begin != end) {
			int year = (*begin).year();
			assert(year<=_max_year && year >= _min_year);
			++yearcounts[year-_min_year];
			++begin;
		}
	}

	/// \return Does each year have enough sites.
	bool check_years (
		typename std::vector<point_type>::const_iterator begin, 
		typename std::vector<point_type>::const_iterator end
		) const
	{
		std::vector<size_t> counts(_max_year-_min_year+1,0);

		count_years(begin,end,counts);

		for (int i = 0; i < counts.size(); ++i) {
			if (counts[i] < min_size()) {
				return false;
			}
		}
		/*
		std::cout<< "-----------------------------------------------\n";
		for (int i = 0; i < counts.size(); ++i) {
			std::cout << _min_year+i << " " << counts[i] << "\n";
		}
		std::cout<<"\n";
		*/
		return true;
	}

	///O(nlog n) median finding algorithm, can be trivially replaced
	///with one of the standard O(N) (approximate, randomized) median
	///finding algorithms,
	/// \returns true if split ok, false if we should stop splitting
	bool compute_median(std::vector<point_type>& v, int d, point_type& median) const
	{
		//if there is too few points, abort
		if (!check_years(v.begin(),v.end())) 
			return false;

		std::sort(v.begin(),v.end(), comparator_type(d));

		typename std::vector<point_type>::const_iterator m = v.begin();
		std::advance(m,v.size()/2);

		//if there is too few points, abort
		if (!check_years(v.begin(),m))
			return false;

		median=*m;
		std::advance(m,1);
		
		if (!check_years(m,v.end()))
			return false;

		return true;
	}

	static size_t dimensions() { return D; }
	size_t min_size() const { return _min_size; }
private:
	const size_t _min_size;
	const int _min_year;
	const int _max_year;
};


template<size_t D, typename T>
std::ostream& operator<<(std::ostream& s, const site<D,T>& p) {
	s << p.year() << ",";
	s << "";
	for (size_t a = 0; a < p.attributes(); ++a) {
		if (a != 0) s <<",";
		s << p.attribute(a);
	}
	s << "";
	const primitives::point<D,T>&  newp = p;
	s << "," << newp << "";
	return s;
}

}

#endif
