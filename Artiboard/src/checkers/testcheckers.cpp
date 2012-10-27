#include <tut/tut.hpp>
#include <sstream>
#include "checkers.h"
#include "../player/player.h"
#include <cstring>
#define UTEST template<> template<> void test_group<checkersData>::object::test
using namespace game;
using namespace checkers;
using namespace player;

namespace tut {

namespace boards {
	const char * init =
			"\
8   o   o   o   o \n\
7 o   o   o   o   \n\
6   o   o   o   o \n\
5 -   -   -   -   \n\
4   -   -   -   - \n\
3 x   x   x   x   \n\
2   x   x   x   x \n\
1 x   x   x   x   \n\
  1 2 3 4 5 6 7 8 \n";

	const char * first =
			"\
8   o   o   o   o \n\
7 o   o   o   o   \n\
6   o   o   o   - \n\
5 -   -   -   o   \n\
4   -   -   -   - \n\
3 x   x   x   x   \n\
2   x   x   x   x \n\
1 x   x   x   x   \n\
  1 2 3 4 5 6 7 8 \n";

	const char * later =
			"\
8   o   o   o   - \n\
7 o   o   o   o   \n\
6   o   o   -   o \n\
5 O   -   -   o   \n\
4   -   -   -   - \n\
3 x   -   x   x   \n\
2   -   x   x   x \n\
1 -   -   x   x   \n\
  1 2 3 4 5 6 7 8 \n";

	const char * final =
			"\
8   -   -   -   - \n\
7 o   o   -   -   \n\
6   -   o   -   - \n\
5 -   -   o   o   \n\
4   -   -   -   - \n\
3 -   -   -   O   \n\
2   -   -   -   - \n\
1 -   -   -   -   \n\
  1 2 3 4 5 6 7 8 \n";

	const char * finalrace =
			"\
8   o   o   o   o \n\
7 -   -   -   o   \n\
6   -   -   -   - \n\
5 -   -   -   -   \n\
4   -   -   -   - \n\
3 o   -   -   -   \n\
2   -   -   x   - \n\
1 x   x   x   O   \n\
  1 2 3 4 5 6 7 8 \n";
}


	template<typename T> string toString(T& obj) {
		std::ostringstream ss;
		ss << obj;
		return ss.str();
	}
	template<typename T> void assert_equals(T& obj, const char * expected) {
		string actual = toString(obj);
		ensure_equals("lengths of strings are not equal", actual.length(),
				strlen(expected));
		ensure_equals(actual, expected);
	}

	struct checkersData {
	};
	test_group<checkersData> checkersTests("030 Checkers Tests");

	UTEST<1>() {
		set_test_name("normal game play");
		CheckersGame normal;
		// check initial state
		normal.start();
		assert_equals(normal.currentState().board(),boards::init);
		ensure_equals("Number of moves on startup",normal.current().next_ply().size(),7U);
		// play one move
		auto nextState = (CheckersState*) normal.current().next_ply().front();
		assert_equals(nextState->board(),boards::first);
		// play more moves
		for (int i = 0; i<10; i++)
		nextState = (CheckersState*) nextState->next_ply().front();
		assert_equals(nextState->board(),boards::later);
		// play to end
		while (!nextState->is_endgame_node())
		nextState = (CheckersState*) nextState->next_ply().front();
		assert_equals(nextState->board(),boards::final);
	}

	UTEST<2>() {
		set_test_name("race game play");
		CheckersGame abnormal(true);
		abnormal.start();
		// play to end
		auto nextState = (CheckersState*) &abnormal.current();
		while (!nextState->is_endgame_node()) {
			nextState = (CheckersState*) nextState->next_ply().back();
		}
		assert_equals(nextState->board(),boards::finalrace);
	}

	UTEST<3>() {
		set_test_name("play line read test");
		CheckersGame abnormal(true);
		auto line1 = abnormal.compete(new RandomPlayer(),new RandomPlayer());
		istringstream is1(toString(*line1));
		CheckersPlayLine cline1(abnormal.rule());
		is1 >> cline1;
		ensure_equals(toString(cline1).length(),(unsigned int) 6563U);
	}

	UTEST<4>() {
		set_test_name("play predictable random games");
		CheckersGame abnormal(true);
		auto line = abnormal.compete(new RandomPlayer(),new RandomPlayer());
		auto line2 = abnormal.compete(new RandomPlayer(),new RandomPlayer());
		ensure_equals(line->outcome(),line2->outcome());
		ensure_equals(line->move_count(),line2->move_count());
		ensure_equals(toString(*line),toString(*line2));
	}

}
;
