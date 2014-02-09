#include "game.h"

namespace arti {
typedef std::function<float(const Position&)> eval_function_t;

struct EvalResult {
		Board::u_ptr_it it;
		float v;
};

class PickNegamax: public MoveChooser {
	private:
		const GameSpecification* _spec;
		eval_function_t _function;
		int _max_plies;
		int _walk_count;
		float _value;
		EvalResult maximise(const Position &p, const Board::u_ptr_it child_b, const Board::u_ptr_it child_e, const int depth, const int sign);
	public:
		PickNegamax(GameSpecification* spec, eval_function_t fn, int ply) :
			_spec(spec), _function(fn), _max_plies(ply), _walk_count(0), _value(0.0f) {
		};
		/** The number of positions traversed by the chooser during the previous call to select */
		int walk_count() {return _walk_count; }
		/** The value of the root calculated by the chooser during the previous call to select */
		float value() {return _value;}
		Board::u_ptr_it select(const Position & current, Board::u_ptr_list &list) override;
};

class PickNegamaxAlphaBeta: public MoveChooser {
	private:
		const GameSpecification* _spec;
		eval_function_t _function;
		int _max_plies;
		int _walk_count;
		float _value;
		EvalResult maximise(const Position &p, const Board::u_ptr_it child_b, const Board::u_ptr_it child_e,
			const int depth, float alpha, const float beta,
			const int sign);
	public:
		PickNegamaxAlphaBeta(GameSpecification* spec, eval_function_t fn, int ply) :
			_spec(spec), _function(fn), _max_plies(ply), _walk_count(0), _value(0.0f) {
		};
		/** The number of positions traversed by the chooser during the previous call to select */
		int walk_count() {return _walk_count; }
		/** The value of the root calculated by the chooser during the previous call to select */
		float value() {return _value;}
		Board::u_ptr_it select(const Position & current, Board::u_ptr_list &list) override;
};

}
