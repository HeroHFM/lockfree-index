#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "Geometry.h"
#include <algorithm>
#include <vector>

namespace Algo {

using namespace Geom;

using dist_vec2_t = std::vector<std::pair<double, vec2>>;

class Algorithm {

public:
	virtual void preprocess(std::vector<vec2> const &vectors) = 0;
	virtual dist_vec2_t query(vec2 q, std::size_t k) = 0;

};

}

#endif // ALGORITHM_H

