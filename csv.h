#ifndef READ_CSV_
#define READ_CSV_

#include "config.h"
#include <string>

void read_csv(const std::string& filename, std::vector<point>& v, int& min_year, int& max_year);
void write_csv(const std::string& filename, std::vector<std::pair<size_t,point> >& v);

#endif
