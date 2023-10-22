#ifndef LOCKFREEINDEX_H
#define LOCKFREEINDEX_H

#include "Algorithm.h"
#include "LockfreeBST.h"

namespace Algo {

class LockfreeIndex : public Algorithm {
private:
	LF::BST<double, vec2> index;

public:
	// Interface

	// not thread-safe
	void preprocess(std::vector<vec2> const &vectors) override
	{
		// Linearize points, insert into index
		std::for_each(vectors.cbegin(), vectors.cend(), [this](const auto & vec){
			insert(vec);
		});
	}

	// thread-safe
	// assume we have enough nodes to complete query
	dist_vec2_t query(vec2 q, std::size_t k) override
	{
		// No points to find
		if (k == 0) return dist_vec2_t();
		
		// Find closest point
		auto lin = linearize(q);
		auto it = index.find(lin);

		// Empty tree
		if (it.ptr() == nullptr) return dist_vec2_t();

		// Assign wrapping at the edges
		auto next = it.inc(); auto prev = it.dec();
		if (circular_distance(lin, it.ptr()->key) > circular_distance(lin, next.ptr()->key)) it = next;
		if (circular_distance(lin, it.ptr()->key) > circular_distance(lin, prev.ptr()->key)) it = prev;

		dist_vec2_t results;
		results.reserve(k);
		results.push_back(it.pair());

		auto left = it.inc();
		auto right = it.dec();

		while (results.size() < k) {
			if (circular_distance(left.ptr()->key, lin) < circular_distance(right.ptr()->key, lin)) {
				results.push_back(left.pair());
				left = left.inc();
			} else {
				results.push_back(right.pair());
				right = right.dec();
			}
		}

		return results;
	}

	// Requirements

	// 1. `contains( vector2d query )`. Returns true if `query` is in the index or false otherwise.
	// Runtime: O(height), wait-free
	bool contains(vec2 q) { return index.find(linearize(q)).ptr()->val == q; }
	
	// 2. `knn( vector2d query, std::size_t k )`. Returns the `k` vectors in the index that are closest to `query` per the cosine similarity function.
	// Runtime: O(k*height), wait-free
	dist_vec2_t knn(vec2 q, std::size_t k) { return query(q, k); }
	
	// 3. `nearest( vector2d query )`. Returns the vector in the index that are closest to `query` per the cosine similarity function. This is logically equivalent to `knn(query, 1lu)`
	// Runtime: O(height), wait-free
	vec2 nearest(vec2 q)
	{
		dist_vec2_t r = knn(q, 1lu);
		assert(r.size() == 1);
		return r[0].second;
	}

	// 4. `insert( vector2d new_vector )`. Inserts `new_vector` into the index or returns some error condition if it already exists
	// Runtime: O(height), lock-free
	void insert(vec2 new_vector) { index.insert(linearize(new_vector), new_vector); }

};

}

#endif // LOCKFREEINDEX_H