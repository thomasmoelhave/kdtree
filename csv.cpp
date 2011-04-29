#include "csv.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>

void write_csv(const std::string& filename, std::vector<std::pair<size_t,point> >& v) {
	std::ofstream f;
	f.open(filename.c_str());

	if (!f) {
		throw std::runtime_error("Could not open file.");
	}
	
	for (int i = 0; i < v.size(); ++i) {
		size_t id = v[i].first;
		const point& p = v[i].second;
		f << id << "," << p << "\n";
	}
}

void read_csv(const std::string& filename, std::vector<point>& v, int& min_year, int& max_year) {
	std::ifstream f;
	f.open(filename.c_str());

	if (!f) {
		throw std::runtime_error("Could not open file.");
	}

	bool compute_max = false;
	if (max_year==std::numeric_limits<int>::min()) {
		compute_max=true;
	}
	bool compute_min = false;
	if (min_year==std::numeric_limits<int>::max()) {
		compute_min=true;
	}
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
			for (size_t i = 0; i < t.length(); ++i) { 
				if (t[i]=='"') t[i]=' '; 
			}
			boost::trim(t);

			switch(idx) {
				case 0: //id
				case 1: //plt_cn
				{
					p.attribute(idx)=t;
				} break;
				case 5: //year
				{
					std::stringstream s(t);
					int f;
					s >> f;
					assert(s);
					p.year()=f;
				} break;
				case 2:
				case 3:
				case 4:
				{
					std::stringstream s(t);
					float f;
					s >> f;
					assert(s);
					p(idx-2)=f;
				} break;
				default:
					assert(false);
			};
			++idx;
		}
		bool ignore=false;
		if (compute_min)
			min_year=std::min(min_year,p.year());
		else 
			ignore = ignore || p.year() < min_year;

		if (compute_max)
			max_year=std::max(max_year,p.year());
		else 
			ignore = ignore || p.year() > max_year;

		if (!ignore)
			v.push_back(p);
	}
}


