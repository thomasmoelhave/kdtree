#include <iostream>
#include <fstream>
#include <cstdlib>
#include "config.h"
#include "csv.h"
#include <boost/program_options.hpp>

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
	int min_year=std::numeric_limits<int>::max();
	int max_year=std::numeric_limits<int>::min();
	size_t min_size = 20;

	namespace po = boost::program_options;

	po::positional_options_description p;
	p.add("input-file", 1);
	p.add("output-file", 1);

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help", "produce help message")
	    ("max-year", po::value<int>(&max_year), "Years higher than this are ignored.")
	    ("min-year", po::value<int>(&min_year), "Years lower than this are ignored.")
	    ("min-size", po::value<size_t>(&min_size)->default_value(min_size), "Do not create boxes with fewer than this number of sites.")
	    ("input-file", po::value<std::string>(), "Input csv file.")
	    ("output-file", po::value<std::string>(), "Output csv file.")
	;

	po::variables_map vm;
	//po::store(po::parse_command_line(argc, argv, desc), vm);
	po::store(po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);
	po::notify(vm);    

	if (vm.count("help")) {
		std::cout << desc << "\n";
	    return 1;
	}

	if (!vm.count("input-file")) {
		std::cout << "No input file specified.\n";
		std::cout << desc << "\n";
		return 1;
	}
	if (!vm.count("output-file")) {
		std::cout << "No output file specified.\n";
		std::cout << desc << "\n";
		return 1;
	}


	std::string input_file=vm["input-file"].as<std::string>();
	std::string output_file=vm["output-file"].as<std::string>();


	std::vector<node::point_type> points;
	
	try {
		read_csv(input_file,points, min_year, max_year);
	} catch(std::exception e) {
		std::cerr << "Error loading file: " << input_file << ":" << e.what() << "\n";
		return 1;
	}

	Traits traits(min_size,min_year,max_year);

	node n;

	std::cout << "Building " << D << "D-tree from " << points.size() << " points from years " << min_year << "-" << max_year << "." << std::endl;

	n.build(points,traits);

	std::vector<std::pair<size_t,point> > leaf_points;
	n.leaf_points(leaf_points);	

	try {
		write_csv(output_file,leaf_points);
		std::ofstream nodefile((output_file+".node").c_str());
		nodefile << n << "\n";
	} catch(std::exception e) {
		std::cerr << "Error loading file: " << output_file << ":" << e.what() << "\n";
		return 1;
	}

}
