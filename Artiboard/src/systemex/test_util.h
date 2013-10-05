// see licence.txt
#pragma once
#include <tut/tut.hpp>
#include "systemex.h"

/**
 * Template unit test framework, see <http://tut-framework.sourceforge.net/>
 */
namespace tut {
	using namespace systemex;
	void ensure_contains(const char * text, const char * sought);
	void ensure_contains(const std::exception & error, const char * sought);
	void ensure_equalsf(const char * text, const float& found, const float &expected);
}
