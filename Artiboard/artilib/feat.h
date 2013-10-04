#pragma once
#include <string>
#include "feat_program.h"
namespace artilib {
	FeatureProgram * load_program(const std::string& filename);
}