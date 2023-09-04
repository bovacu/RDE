#include <random>

struct rde_random {
	std::random_device rd;
	std::mt19937 mt;
};

void rde_math_set_random_seed(long _seed) {
	ENGINE.random->mt.seed(_seed);
}

int rde_math_get_random_int(int _min_included, int _max_not_included) {
	if(_min_included > _max_not_included) {
		int _aux = _min_included;
		_min_included = _max_not_included;
		_min_included = _aux;
	}
	std::uniform_int_distribution<int> _dist(_min_included, _max_not_included);
	return _dist(ENGINE.random->mt);
}

float rde_math_get_random_float(float _min_included, float _max_not_included) {
	if(_min_included > _max_not_included) {
		float _aux = _min_included;
		_min_included = _max_not_included;
		_max_not_included = _aux;
	}
	std::uniform_real_distribution<float> _dist(_min_included, _max_not_included);
	return _dist(ENGINE.random->mt);
}

rde_probability rde_math_get_probability(float _normalized_chance_to_happen) {
	if(_normalized_chance_to_happen > 1.0f) _normalized_chance_to_happen = 1.0f;
	if(_normalized_chance_to_happen < 0.0f) _normalized_chance_to_happen = 0.0f;

	float _chance = 1.f - _normalized_chance_to_happen;
	float _leftProbability = rde_math_get_random_float(0.0f, 1.0f);

	rde_probability _p {_leftProbability, _leftProbability >= _chance};
	return _p;
}