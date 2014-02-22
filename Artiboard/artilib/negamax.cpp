#include "negamax.h"
#include "systemex.h"
#include "log.h"
#include <limits>
#include <map>

namespace arti {

EvalResult PickNegamax::maximise(const Position &r, const Board::u_ptr_it child_b, const Board::u_ptr_it child_e,
	const int depth, const int sign) {
	walk_count_++;
	if ((depth == 0) || (child_b == child_e))
		return {child_e, sign * function_(r)};
	else {
		EvalResult result { child_e, std::numeric_limits<float>::min() };
		for (auto cit = child_b; cit != child_e; cit++) {
			Board::u_ptr_list grand_children;
			PositionThatPoints c(r.ply().next(), cit->get());
			spec_->collectBoards(c, grand_children);
			auto grand_result =  maximise(c, grand_children.begin(), grand_children.end(), depth-1, -sign);
			grand_result.v *= -1;
			if (result.it == child_e || grand_result.v > result.v) {
				result.v = grand_result.v;
				result.it = cit;
			}
		};
		ASSERT(result.it != child_e);
		return result;
	}
}

Board::u_ptr_it PickNegamax::select(const Position & current, Board::u_ptr_list &list) {
	walk_count_ = 0;
	const int sign = current.ply().is_odd()?-1:1;
	auto result = maximise(current, list.begin(), list.end(), max_plies_, sign);
	value_ = result.v * sign;
	return result.it;
}


EvalResult PickNegamaxAlphaBeta::maximise(const Position &r, const Board::u_ptr_it child_b, const Board::u_ptr_it child_e,
	const int depth, float alpha, const float beta, const int sign) {
	walk_count_++;
	if ((depth == 0) || (child_b == child_e))
		return {child_e, sign * function_(r)};
	else {
		EvalResult best { child_e, std::numeric_limits<float>::min() };
		for (auto cit = child_b; cit != child_e; cit++) {
			Board::u_ptr_list grand_children;
			PositionThatPoints c(r.ply().next(), cit->get());
			spec_->collectBoards(c, grand_children);
			if (ordered_) sort_by_function(grand_children,c.ply());
			auto grand_result =  maximise(c, grand_children.begin(), grand_children.end(), depth-1,-beta,-alpha, -sign);
			grand_result.v *= -1;
			if (best.it == child_e || grand_result.v > best.v) {
				best.v = grand_result.v;
				best.it = cit;
			}
			alpha = std::max(alpha,grand_result.v);
			if (alpha >= beta)
				break;
		};
		ASSERT(best.it != child_e);
		return best;
	}
}

Board::u_ptr_it PickNegamaxAlphaBeta::select(const Position & current, Board::u_ptr_list &list) {
	walk_count_ = 0;
	const int sign = current.ply().is_odd()?-1:1;
	auto result = maximise(current, list.begin(), list.end(), max_plies_,
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::max(), sign);
	value_ = result.v * sign;
	return result.it;
}

/* A better function has a greater value for the better board to play at the ply */
struct BetterThanFnFunctor {
		const eval_function_t fn_;
		const Ply parent_ply;
		std::map<const Board*,float> values_;
		float operator()(const Board * b) {
			auto fit = values_.find(b);
			float v = 0.0f;
			if (fit != values_.end()) 
				v = fit->second;
			else {
				v = fn_(PositionThatPoints(parent_ply.next(),b));
				if (!parent_ply.is_odd())
					v = -v;
				values_.emplace(b,v);
			}
			return v;
		}				
};

struct LessThanFnComparer {		
		BetterThanFnFunctor& fn_;
		bool operator()(const Board::u_ptr& a, const Board::u_ptr& b) {
			const auto av = fn_(a.get());
			const auto bv = fn_(b.get());
			return av < bv;
		}
};


void PickNegamaxAlphaBeta::sort_by_function(Board::u_ptr_list& boards, const Ply parent_ply) const {
	BetterThanFnFunctor bt{function_, parent_ply};
	LessThanFnComparer compare{bt};
	boards.sort(compare);
}

}
