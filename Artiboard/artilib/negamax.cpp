#include "negamax.h"
namespace arti {

EvalResult PickNegamax::maximise(const Position &r, Board::u_ptr_it child_b, Board::u_ptr_it child_e, const int at) {
	const int sign = (at % 2 == 1?1:-1);
	if (at == p_max || child_b == child_e)
		return {child_e, sign * _function(r.board())};
	else {
			EvalResult result{child_e,0};
			for (auto cit = child_b; cit != child_e; cit++) {
				Board::u_ptr_list grand_children;
				Position c(at,*cit);
				_spec->collectBoards(c,grand_children);
				auto grand_result = maximise(c,grand_children.begin(),grand_children.end(),at+1);
				grand_result.v *= sign;
				if (result.it == child_e || grand_result.v > result.v) {
					result.v = grand_result.v;
					result.it = cit;
				}
			}; 
			return result;
	}
}

Board::u_ptr_it PickNegamax::select(const Position & current,Board::u_ptr_list &list) {
	return maximise(current, list.begin(),list.end(),1).it;
};

}

