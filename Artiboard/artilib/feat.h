#pragma once
#include <string>
#include "feat_program.h"
namespace arti {
	FeatureProgram::u_ptr load_program(const std::string& filename);
	FeatureProgram::u_ptr parse_program(std::string& code);
}