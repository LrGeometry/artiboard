#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>
#include <cstring>
#include <systemex.h>

#include "test_util.h"


void test_main() {
	tut::console_reporter reporter(std::cout);
	tut::runner.get().set_callback(&reporter);
	tut::runner.get().run_tests();
	if (reporter.all_ok())
		std::cout << "Unit tests passed" << std::endl;
	else
		std::cerr << "Some unit tests failed!" << std::endl;
#ifdef FIND_LEAKS
	puts(systemex::memoryLeakReport().c_str());
#endif
}

namespace tut {
	using namespace arti;
void ensure_contains(const char * text, const char * sought) {
	char * fnd = std::strstr(text, sought);
	if (fnd == 0) {
		auto msg = string_from_format("could not find '%s' in '%s'", sought,
				text);
		fail(msg.c_str());
	}
}

void ensure_contains(const std::exception & error, const char * sought) {
	ensure_contains(error.what(), sought);
}

void ensure_equalsf(const char * text, const float& found,
		const float &expected) {
	if (fabs(found - expected) > 0.0001)
		ensure_equals(text, expected, found);
}

void fail_with(const std::ostringstream& s) {
	fail(s.str().c_str());
}

}
