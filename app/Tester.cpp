#include <Algorithm.h>
#include <LockfreeIndex.h>
#include <ReferenceIndex.h>

#include <iostream>
#include <vector>
#include <string>

#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <random>

using namespace Geom;
using namespace Algo;

// Note: Most of this code originates from SemanticIDs

/* Read vectors from input file in format
 * word x_val y_val
 */
void read_data(const char *filename, std::vector<vec2> &vectors, std::vector<std::string> &words) {
	std::ifstream handle(filename, std::ios::in);

	double x, y;
	std::string word;

	while (handle.good()) {
		handle >> word >> x >> y;
		words.push_back(word);
		vectors.push_back({x, y});
	}

	handle.close();
}

struct Test_Setup_k {
	size_t k_min;
	size_t k_max;
	size_t resolution;
	size_t repeat;
	double q_min;
	double q_max;
};

void test_c(const std::vector<vec2> &vectors, Algorithm *algo1, Algorithm *algo2) {
	Test_Setup_k TS = {
			.k_min      = 1,
			.k_max      = vectors.size(),
			.resolution = 1,
			.repeat     = 50,
			.q_min      = -10,
			.q_max      = 10,
		};

	algo1->preprocess(vectors); // Pre-process vectors
	algo2->preprocess(vectors);

    std::mt19937 gen(5567);
    std::uniform_real_distribution<> dis(TS.q_min, TS.q_max);

	for(size_t k = TS.k_min; k <= TS.k_max; k += TS.resolution) {

		std::vector<vec2> q(TS.repeat);
		for (auto it = q.begin(); it != q.end(); ++it)
			*it = vec2{.x = dis(gen), .y = dis(gen)};

		auto q_it = q.cbegin();
		
		for (size_t i = 0; i < TS.repeat; ++i) {
			dist_vec2_t res1 = algo1->query(*q_it, k);
			dist_vec2_t res2 = algo2->query(*q_it, k);

			std::vector<vec2> v1(res1.size());
			std::vector<vec2> v2(res2.size());
			std::transform(res1.begin(), res1.end(), v1.begin(), [](auto val){return val.second;});
			std::transform(res2.begin(), res2.end(), v2.begin(), [](auto val){return val.second;});

			// Assume that res1 is sorted
			if (!std::is_permutation(v1.begin(), v1.end(), v2.begin(), [](const auto &l, const auto &r){
				return (l.x == r.x) && (l.y == r.y);
			})) {
				std::cout << "(i=" << i << "/" << TS.repeat << ", k=" << k << ") "<< "Vectors are not equal!" << std::endl;
				std::cout << "Expected:" << std::endl;
				for (auto it = v1.begin(); it != v1.end(); ++it) std::cout << it->x << " " << it->y << std::endl;
				std::cout << "Received:" << std::endl;
				for (auto it = v2.begin(); it != v2.end(); ++it) std::cout << it->x << " " << it->y << std::endl;
				return;
			}
			q_it++;
		}
	}

	std::cout << "Vectors are equal!" << std::endl;

}

int main(int /* argc */, char **argv) {

	std::vector<vec2> vectors;
	std::vector<std::string> words;

	// Load vectors
	read_data(argv[1], vectors, words);

	ReferenceIndex ref;
	LockfreeIndex lfi;

	std::cout << "Checking against reference implementation: " << std::flush;
	test_c(vectors, &ref, &lfi);
}