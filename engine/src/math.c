#include <math.h>

void rde_math_set_random_seed(long _seed) {
	srand(_seed);
}

int rde_math_get_random_int(int _min_included, int _max_included) {
	return (rand() % (_max_included - _min_included + 1)) + _min_included;
}

float rde_math_get_random_float(float _min_included, float _max_included) {
	float _scale = rand() / (float) RAND_MAX;
	return _min_included + _scale * (_max_included - _min_included);
}

rde_probability rde_math_get_probability(float _normalized_chance_to_happen) {
	UNUSED(_normalized_chance_to_happen)
	if(_normalized_chance_to_happen > 1.0f) _normalized_chance_to_happen = 1.0f;
	if(_normalized_chance_to_happen < 0.0f) _normalized_chance_to_happen = 0.0f;

	float _chance = 1.f - _normalized_chance_to_happen;
	float _left_probability = rde_math_get_random_float(0.0f, 1.0f);
	rde_probability _p;
	_p.probability_rolled = _left_probability;
	_p.happened = _chance;
	return _p;
}

void rde_math_convert_world_position_to_screen_coordinates_3d(rde_window* _window, rde_vec_3F* _vec) {
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	_vec->x = (_vec->x / (_window_size.x * 0.5f));
	_vec->y = (_vec->y / (_window_size.y * 0.5f));
}

void rde_math_convert_world_position_to_screen_coordinates_2d(rde_window* _window, rde_vec_2F* _vec) {
	rde_vec_3F _vec_ = { _vec->x, _vec->y, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(_window, &_vec_);
	_vec->x = _vec_.x;
	_vec->y = _vec_.y;
}

void rde_math_convert_world_size_to_screen_size(rde_window* _window, rde_vec_2F* _vec) {
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	_vec->x = (_vec->x / (_window_size.x));
	_vec->y = (_vec->y / (_window_size.y));
}

#define RDE_IMPLEMENT_CLAMP_FUNCS(_type) 								\
_type rde_math_clamp_##_type(_type _value, _type _min, _type _max) { 	\
	return (_value < _min) ? _min : (_value > _max) ? _max : _value; 	\
}

RDE_IMPLEMENT_CLAMP_FUNCS(int)
RDE_IMPLEMENT_CLAMP_FUNCS(uint)
RDE_IMPLEMENT_CLAMP_FUNCS(float)
RDE_IMPLEMENT_CLAMP_FUNCS(double)
RDE_IMPLEMENT_CLAMP_FUNCS(long)
RDE_IMPLEMENT_CLAMP_FUNCS(ulong)

float rde_math_radians_to_degrees(float _radians) {
	return _radians * (180.f / RDE_PI);
}

float rde_math_degrees_to_radians(float _degrees) {
	return _degrees * (RDE_PI / 180.0f);
}

float rde_math_easing_in_linear(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return _change_in_value * _current_time / _duration + _start_value;
}

float rde_math_easing_in_quadratic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return _change_in_value * _current_time * _current_time + _start_value;
}

float rde_math_easing_out_quadratic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return -_change_in_value * _current_time * (_current_time - 2) + _start_value;
}

float rde_math_easing_in_out_quadratic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1) return _change_in_value / 2 * _current_time * _current_time + _start_value;
	_current_time--;
	return -_change_in_value / 2 * (_current_time * (_current_time - 2) - 1) + _start_value;
}

float rde_math_easing_in_cubic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return _change_in_value * _current_time * _current_time * _current_time + _start_value;
}

float rde_math_easing_out_cubic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	_current_time--;
	return _change_in_value * (_current_time * _current_time * _current_time + 1) + _start_value;
}

float rde_math_easing_in_out_cubic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1) return _change_in_value / 2 * _current_time * _current_time * _current_time + _start_value;
	_current_time -= 2;
	return _change_in_value / 2 * (_current_time * _current_time * _current_time + 2) + _start_value;
}

float rde_math_easing_in_quartic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return _change_in_value * _current_time * _current_time * _current_time * _current_time + _start_value;
}

float rde_math_easing_out_quartic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	_current_time--;
	return -_change_in_value * (_current_time * _current_time * _current_time * _current_time - 1) + _start_value;
}

float rde_math_easing_in_out_quartic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1)
		return _change_in_value / 2 * _current_time * _current_time * _current_time * _current_time + _start_value;
	_current_time -= 2;
	return -_change_in_value / 2 * (_current_time * _current_time * _current_time * _current_time - 2) + -_start_value;
}

float rde_math_easing_in_quintic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return _change_in_value * _current_time * _current_time * _current_time * _current_time * _current_time + _start_value;
}

float rde_math_easing_out_quintic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	_current_time--;
	return _change_in_value * (_current_time * _current_time * _current_time * _current_time * _current_time + 1) + _start_value;
}

float rde_math_easing_in_out_quintic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1)
		return _change_in_value / 2 * _current_time * _current_time * _current_time * _current_time * _current_time + _start_value;
	_current_time -= 2;
	return _change_in_value / 2 * (_current_time * _current_time * _current_time * _current_time * _current_time + 2) + _start_value;
}

float rde_math_easing_in_sine(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return -_change_in_value * (float) cos(_current_time / _duration * (RDE_PI / 2)) + _change_in_value + _start_value;
}

float rde_math_easing_out_sine(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return _change_in_value * (float) sin(_current_time / _duration * (RDE_PI / 2)) + _start_value;
}

float rde_math_easing_in_out_sine(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return -_change_in_value / 2 * ((float) cos(RDE_PI * _current_time / _duration) - 1) + _start_value;
}

float rde_math_easing_in_exponential(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return _change_in_value * (float) pow(2, 10 * (_current_time / _duration - 1)) + _start_value;
}

float rde_math_easing_out_exponential(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return _change_in_value * (-(float) pow(2, -10 * _current_time / _duration) + 1) + _start_value;
}

float rde_math_easing_in_out_exponential(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1) return _change_in_value / 2 * (float) pow(2, 10 * (_current_time - 1)) + _start_value;
	_current_time--;
	return _change_in_value / 2 * (-(float) pow(2, -10 * _current_time) + 2) + _start_value;
}

float rde_math_easing_in_circular(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return -_change_in_value * ((float) sqrt(1 - _current_time * _current_time) - 1) + _start_value;
}

float rde_math_easing_out_circular(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	_current_time--;
	return _change_in_value * (float) sqrt(1 - _current_time * _current_time) + _start_value;
}

float rde_math_easing_in_out_circular(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1)
		return -_change_in_value / 2 * ((float) sqrt(1 - _current_time * _current_time) - 1) + _start_value;
	_current_time -= 2;
	return _change_in_value / 2 * ((float) sqrt(1 - _current_time * _current_time) + 1) + _start_value;
}
