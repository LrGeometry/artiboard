#pragma once
#include <tut/tut.hpp>
#include <sstream>
#define ensure_error(C,E) try {C;fail("This test expected an exception");} catch(tut::failure &f){throw;} catch (std::exception &ex) {ensure_contains(ex, E);}
#define _fail(M) {std::ostringstream msg;msg << M;fail_with(msg);}
#define _ensure(M,B) {if (!B) _fail(M)};
#define _ensure_not(M,B) {if (B) _fail(M)};

// these macros make tests a bit more readable; but you ahve to define TESTDATA yourself
#define TEST(Num) template<> template<> void test_group<TESTDATA>::object::test<Num>() 
#define NAME(Name) set_test_name(Name); 
#define BEGIN(Num,Name) TEST(Num) { NAME(Name)
#define END }

void test_main();
namespace tut {
	void ensure_contains(const char * text, const char * sought);
	void ensure_contains(const std::exception & error, const char * sought);
	void ensure_equalsf(const char * text, const float& found, const float &expected);
	void fail_with(const std::ostringstream& s);
}
