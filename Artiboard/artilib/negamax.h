#include "game.h"

namespace arti {
typedef std::function<float(const Position&)> eval_function_t;

struct EvalResult {
		Board::u_ptr_it it;
		float v;
};

class MinimaxChooser : public MoveChooser  {
protected:
		const GameSpecification* spec_;
		eval_function_t function_;
		int max_plies_;
		int walk_count_;
		float value_;
protected:
		MinimaxChooser(GameSpecification* spec, eval_function_t fn, int ply) : spec_(spec), function_(fn), max_plies_(ply), walk_count_(0), value_(0.0f) {};
public:
		/** The number of positions traversed by the chooser during the previous call to select */
		int walk_count() const {return walk_count_;}
		/** The value of the root calculated by the chooser during the previous call to select */
		float value() const {return value_;}
};

class PickNegamax: public MinimaxChooser {
	private:
		EvalResult maximise(const Position &p, const Board::u_ptr_it child_b, const Board::u_ptr_it child_e, const int depth, const int sign);
	public:
		PickNegamax(GameSpecification* spec, eval_function_t fn, int ply) :
			MinimaxChooser(spec,fn,ply) {};
		Board::u_ptr_it select(const Position & current, Board::u_ptr_list &list) override;
};

class PickNegamaxAlphaBeta: public MinimaxChooser {
	private:
		bool ordered_;
		EvalResult maximise(const Position &p, const Board::u_ptr_it child_b, const Board::u_ptr_it child_e,
			const int depth, float alpha, const float beta,
			const int sign);
		void sort_by_function(Board::u_ptr_list& boards, const Ply parent_ply) const;
	public:
		PickNegamaxAlphaBeta(GameSpecification* spec, eval_function_t fn, int ply, bool ordered=true) :
			MinimaxChooser(spec,fn,ply),  ordered_(ordered) {
		};
		Board::u_ptr_it select(const Position & current, Board::u_ptr_list &list) override;
};

}
