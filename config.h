#ifndef CONFIG_H_
#define CONFIG_H_

#include "cditraits.h"
#include "kdtree.h"

const size_t D = 3;
typedef traits::CDITraits<D,float> Traits;
typedef kdtree::node<Traits> node;
typedef Traits::point_type point;


#endif
