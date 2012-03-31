#include <tut/tut.hpp>
#include "game.h"
namespace tut {
	using namespace game;
	struct gameData {
	};
	test_group<gameData> gameTests("020 Game Tests");

	template<> template<> void test_group<gameData>::object::test<1>() {
		set_test_name("Square bounds checks");
		ensure(Square::in_bounds(1, 1));
		ensure(Square::in_bounds(8, 8));
		ensure_not(Square::in_bounds(0, 0));
		ensure_not(Square::in_bounds(1, 9));
	}

	template<> template<> void test_group<gameData>::object::test<2>() {
		set_test_name("Square flipped");
		Square normal(1, 1);
		Square flipped(normal, true);
		ensure_equals(flipped.file(), 8);
		ensure_equals(flipped.rank(), 8);

		Square normal2(2, 7);
		Square flipped2(normal2, true);
		ensure_equals(flipped2.file(), 7);
		ensure_equals(flipped2.rank(), 2);

		Square normal3(2, 7);
		Square flipped3(normal3, false);
		ensure_equals(flipped3.file(), 2);
		ensure_equals(flipped3.rank(), 7);
	}
}
;
