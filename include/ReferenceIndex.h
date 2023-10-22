#ifndef REFERENCEINDEX_H
#define REFERENCEINDEX_H

// Simple reference index implementation based on SortScan

#include "Algorithm.h"

namespace Algo {


struct compare_first_gt {
	template <typename T, typename U>
	bool operator() (const std::pair<T, U> l, const std::pair<T, U> r) const { return l.first > r.first; }
};


class ReferenceIndex : public Algorithm {
private:

	std::vector<vec2> V;

protected:
	void populate_distances(vec2 q, dist_vec2_t& dist) {
		q = normalized(q);
		std::transform(V.begin(), V.end(), dist.begin(), [q](const auto vec){return std::make_pair(cosine_normed(q, vec), vec);});
	}

public:

	void preprocess(std::vector<vec2> const &vectors) override {
		V = vectors;
	}

	dist_vec2_t query(vec2 q, std::size_t k) override {
		assert("k bounds" && k <= V.size());
		if (k == 0) return dist_vec2_t();

		dist_vec2_t dist(V.size());
		populate_distances(q, dist);
		std::sort(dist.begin(), dist.end(), compare_first_gt{});

		dist.resize(k);
		return dist;
	}

};

}

#endif // REFERENCEINDEX_H
