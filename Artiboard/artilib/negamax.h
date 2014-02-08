	#include "game.h"

	namespace arti {
		typedef std::function<float (const Board&)> eval_function_t;

		struct EvalResult {Board::u_ptr_it it; float v;};

		class PickNegamax: public MoveChooser {
		private:
			const GameSpecification* _spec;
			eval_function_t _function;	
			EvalResult maximise(const Position &p, Board::u_ptr_it child_b, Board::u_ptr_it child_e, const int at);			
		public:
			int p_max;
			PickNegamax(GameSpecification* spec, eval_function_t fn, int ply)
				: _spec(spec), _function(fn), p_max(ply) {};
			Board::u_ptr_it select(const Position & current, Board::u_ptr_list &list) override ;
		};
}