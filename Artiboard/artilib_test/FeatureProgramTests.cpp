#include "stdafx.h"
#include "CppUnitTest.h"
#include <artilib\feat.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace {
	const char * regions_1 = R"ML(
region set1 = {1,2 3,4};
region set2 = {5,6 7,0};
region set3 = {1,2 0,7};
	)ML";
		const char * example_1 = R"ML(
stateset a1 = {all};
stateset ss = {all some and others};
region a2 = {1,2 2,3};
region ra = {1,2 2,3};
region rb = {1,2 0,7};

formula eq1 = ss @ {1,1} | ( {all} @ ra & {all} @ rb) | ( {all} @ ra & {all} @ rb);
formula eq2 = !({all} @ {1,1} & a1@a2 & a1 @ ra);
formula eq3 = {all some and others} @ {1,1} | !(   {all} @ {1,1} & {all} @ {1,2} | 
{all} @ {1,3 1,4 5,2} );     


function fun1 = 1.23 * {all} @ {1,3 1,4 5,2};
function fun2 = 1.23 * a1 @ a2 + 1.23 * {all} @ {1,3 1,4 5,2};
function fun3 = 1.23 * a1 @ a2;
function fun4 = 1.23 * eq1 + 1.34 * eq2;
	)ML";

				const char * example_3 = R"ML(
stateset a1 = {all};
stateset ss = {all some and others};
region a2 = {1,2 2,3};
region ra = {1,2 2,3};
region rb = {1,2 0,7};

formula eq1 = ss @ {1,1} | ( {all} @ ra & {all} @ rb) | ( {all} @ ra & {all} @ rb);
formula eq2 = !({all} @ {1,1} & a1@a2 & a1 @ ra);
formula eq3 = {all some and others} @ {1,1} | !(   {all} @ {1,1} & {all} @ {1,2} | 
{all} @ {1,3 1,4 5,2} );     
	)ML";

const char * bad_300 = R"ML(
stateset a1 = {all};
stateset ss = {all some and others};
region a2 = {1,2 2,3};
region ra = {1,2 2,3};
region rb = {1,2 0,7} baba;

formula eq1 = ss @ {1,1} | ( {all} @ ra & {all} @ rb) | ( {all} @ ra & {all} @ rb);
formula eq2 = !({all} @ {1,1} & a1@a2 & a1 @ ra);
formula eq3 = {all some and others} @ {1,1} | !(   {all} @ {1,1} & {all} @ {1,2} | 
{all} @ {1,3 1,4 5,2} );     
	)ML";

		const char * bad_ss = R"ML(
region ra = {1,2 2,3};
formula eq1 = ss @ ra;
	)ML";

};
namespace arti
{		
	TEST_CLASS(FeatureProgramTests)
	{
	public:
		
		FeatureProgram::u_ptr parse(const char * code) {
			try	{
					auto program = parse_program(string(code));
					std::stringstream ss;
					ss << *program;
					Logger::WriteMessage("PROGRAM_START\n");
					Logger::WriteMessage(ss.str().c_str());
					Logger::WriteMessage("PROGRAM_END\n");
					return std::move(program);
			} catch (exception &ex) {
				Logger::WriteMessage(code);
				Logger::WriteMessage("\n");
				Logger::WriteMessage(ex.what());
				throw ex;
			}
		}

		void assert_fail(const char * code) {
			try	{
					parse(code);
					Assert::Fail(L"expected exception");
			} catch (exception &ex) {
				// logged already
			}
		}

		void assert_one_formula(const char * code) {
					auto program = parse(code);
					Assert::AreEqual(1U,program->formulas().size());
		}

		void assert_one_stateset(const char * code) {
					auto program = parse(code);
					Assert::AreEqual(1U,program->states().size());
		}

#define TEST_FORMULA(N,S) TEST_METHOD(N) {assert_one_formula(S);}
#define TEST_STATESET(N,S) TEST_METHOD(N) {assert_one_stateset(S);}
#define TEST_PARSE(N,S) TEST_METHOD(N) {parse(S);}
#define TEST_FAIL(N,S) TEST_METHOD(N) {assert_fail(S);}

		TEST_PARSE(parse100, regions_1);

		TEST_PARSE(parse200, example_1);
		TEST_STATESET(stateset100, "stateset a1 = {all};");

		TEST_FAIL(fail100,"stateset 1 = {all};");
		TEST_FAIL(fail200,"stateset a1 = {all}");
		TEST_FAIL(fail300,bad_300);
		TEST_FAIL(fail400,"stateset a1 = {all}k\nstateset b1 = {all};");
		TEST_METHOD(undefined_ss) {assert_fail(bad_ss);};

		TEST_METHOD(undefined_region1) {assert_fail("region ra = {1,2 2,3}; formula eq1 = {a b c} @ rb;");}

		TEST_METHOD(undefined_region2) {assert_fail("region ra = {1,2 2,3}; formula eq1 = {a} @ rb;");}

		TEST_METHOD(undefined_region3) {assert_fail("stateset a1 = {all}; region ra = {1,2 2,3}; formula eq1 = a1 @ rb;");}

	  TEST_METHOD(undefined_ss2) {assert_fail("stateset a1 = {all}; region ra = {1,2 2,3}; formula eq1 = a2 @ ra;");}

		TEST_FORMULA(formula100, 
			"region ra = {1,2 2,3};formula eq1 = {da} @ ra;");

		TEST_FORMULA(formula200,
			"region ra = {1,2 2,3}; formula eq1 = {a b c} @ ra;");

		TEST_FORMULA(formula300,
			"formula eq1 = {a b c} @ {1,1};");

		TEST_FORMULA(formula400,
			"formula eq1 = {aa ba ca} @ {1,1} | {aa ba ca} @ {1,2}; ");


		TEST_FORMULA(formula500,
			"formula eq2 = !({all s} @ {1,1} & {k}@{1,1} & {s all} @ {1,2});");
		
		TEST_FORMULA(formula600,
			"formula eq1 = {aa ba ca} @ {1,1}& {aa ba ca} @ {1,2}; ");

		TEST_FORMULA(formula700,
			"formula eq1 = {a} @ {1,1}; ")

		TEST_FORMULA(formula800,
			"region ra = {1,2 2,3};formula eq1 = {ab cd} @ ra;");

		TEST_METHOD(one_item_function) {
			auto p = parse("region ra = {1,2 2,3};function fun3 = 1.23 * {da} @ ra;");
			Assert::AreEqual(1U,p->functions().size());
		}
	};
}