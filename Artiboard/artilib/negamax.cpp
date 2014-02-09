#include "negamax.h"
#include "systemex.h"
#include "log.h"
#include <limits>


namespace arti {

EvalResult PickNegamax::maximise(const Position &r, const Board::u_ptr_it child_b, const Board::u_ptr_it child_e,
	const int depth, const int sign) {
	_walk_count++;
	if ((depth == 0) || (child_b == child_e))
		return {child_e, sign * _function(r)};
	else {
		EvalResult result { child_e, std::numeric_limits<float>::min() };
		for (auto cit = child_b; cit != child_e; cit++) {
			Board::u_ptr_list grand_children;
			Position c(r.ply().next(), std::move(*cit));
			_spec->collectBoards(c, grand_children);
			auto grand_result =  maximise(c, grand_children.begin(), grand_children.end(), depth-1, -sign);
			grand_result.v *= -1;
			if (result.it == child_e || grand_result.v > result.v) {
				result.v = grand_result.v;
				*cit = std::move(c.board_p());
				result.it = cit;
			}
		};
		ASSERT(result.it != child_e);
		return result;
	}
}

Board::u_ptr_it PickNegamax::select(const Position & current, Board::u_ptr_list &list) {
	_walk_count = 0;
	const int sign = current.ply().is_odd()?-1:1;
	auto result = maximise(current, list.begin(), list.end(), _max_plies, sign);
	_value = result.v * sign;
	return result.it;
}


EvalResult PickNegamaxAlphaBeta::maximise(const Position &r, const Board::u_ptr_it child_b, const Board::u_ptr_it child_e,
	const int depth, float alpha, const float beta, const int sign) {
	_walk_count++;
	if ((depth == 0) || (child_b == child_e))
		return {child_e, sign * _function(r)};
	else {
		EvalResult best { child_e, std::numeric_limits<float>::min() };
		for (auto cit = child_b; cit != child_e; cit++) {
			Board::u_ptr_list grand_children;
			Position c(r.ply().next(), std::move(*cit));
			_spec->collectBoards(c, grand_children);
			auto grand_result =  maximise(c, grand_children.begin(), grand_children.end(), depth-1,-beta,-alpha, -sign);
			grand_result.v *= -1;
			if (best.it == child_e || grand_result.v > best.v) {
				best.v = grand_result.v;
				*cit = std::move(c.board_p());
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
	_walk_count = 0;
	const int sign = current.ply().is_odd()?-1:1;
	auto result = maximise(current, list.begin(), list.end(), _max_plies,
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::max(), sign);
	_value = result.v * sign;
	return result.it;
}

}





