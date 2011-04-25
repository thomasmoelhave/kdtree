#include "kdtree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

const int mindim=2;
const int maxdim=4;
const int D = maxdim-mindim+1;

class CDIStrategy;
typedef kdtree::node<D,int, CDIStrategy> node;
typedef node::point_type point;
typedef node::box_type box;
typedef box::interval_type interval;



class CDIStrategy {
public:
	CDIStrategy(size_t max_size) : _max_size(max_size) { }
	///O(nlog n) median finding algorithm, can be trivially replaced
	///with one of the standard O(N) (approximate, randomized) median
	///finding algorithms,
	node::point_type compute_median(std::vector<node::point_type>& v, int d) const
	{
		std::sort(v.begin(),v.end(), node::point_type::comparator(d));
		return v[v.size()/2];
	}

	size_t max_size() const { return _max_size; }

private:

	const size_t _max_size;
};

void read_csv(const std::string& filename, std::vector<point>& v) {
	std::ifstream f;
	f.open(filename.c_str());

	std::string line;

	//skip header line;
	std::getline(f,line);

	while (std::getline(f,line)) {

		boost::char_separator<char> sep(", \r");
		boost::tokenizer<boost::char_separator<char> > tokens(line, sep);

		int idx=0;
		point p;


		BOOST_FOREACH(std::string t, tokens)
		{
			for (size_t i = 0; i < t.length(); ++i) { if (t[i]=='"') t[i]=' '; }
			std::stringstream s(t);
			float f;
			s >> f;
			assert(s);
			if (idx>=mindim) {
				p(idx-mindim)=f;
			} else if (idx >= maxdim)  {
				continue;
			}
			++idx;
		}
		v.push_back(p);
	}
}

void random_data(std::vector<point>& v, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		point p;
		for (int d = 0; d < D; ++d) {
			p(d)=static_cast<int>(drand48()*100.0);
		}
		v.push_back(p);
	}
}

int main(int argc, char** argv) {

	std::vector<point> points;

	read_csv(argv[1],points);
	//random_data(points,200);

	CDIStrategy strategy(200);

	node n;

	std::cout << "Building " << D << "D-tree from " << points.size() << " points." << std::endl;

	n.build(points,strategy);

	std::cout << n << "\n";
}
