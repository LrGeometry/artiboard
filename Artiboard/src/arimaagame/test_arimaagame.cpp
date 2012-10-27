#include <tut/tut.hpp>
//#include "arimaagame.h"
#include <exception>
#define UTEST template<> template<> void test_group<arimaaTestData>::object::test
namespace tut {
	//using namespace arimaa_game;

	struct arimaaTestData {};

	test_group<arimaaTestData> arimaaGameTests("020 Arimaa Tests");

	UTEST<1>() {
		set_test_name("The first arimaa test");
		ensure_equals(1,1);
	}

}
