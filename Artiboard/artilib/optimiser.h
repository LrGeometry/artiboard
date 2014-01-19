#pragma once
#include <vector>

namespace arti {

	typedef std::vector<double> arguments_t;

	class VectorFunctor {
		public:
			virtual double value_of(const arguments_t &arguments) const = 0;
			double operator () (const std::vector<double> &a) const {return value_of(a);}
	};

	class Optimiser {
	private:
		const VectorFunctor * _functor;
		arguments_t _best_args;
		double _best_value;
		int _apply_count;
	protected:
		/** Default constructor.  The function must map to doubles > double::min */
		Optimiser(VectorFunctor * functor);
		/**
		 * Applies the functor and returns true if there was an improvement on the
		 * best value.
		 */
		double apply(const arguments_t &a);	
	};
}