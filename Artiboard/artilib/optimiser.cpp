#include <limits>
#include "optimiser.h"
#include "systemex.h"
namespace arti {
	Optimiser::Optimiser(VectorFunctor * f) 
	: _functor(f), _best_args(), 
		_best_value(std::numeric_limits<float>::min()),
		_apply_count(0) {
		ENSURE(f != nullptr, "functor cannot cannot be null");
	}

	double Optimiser::apply(const arguments_t &a) {
		const auto r = _functor->value_of(a);
		_apply_count++;
		if (r > _best_value) {
			_best_args = a;
			_best_value = r;
		} 
		return r;
	}
};