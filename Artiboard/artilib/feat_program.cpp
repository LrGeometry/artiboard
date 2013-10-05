#include "feat_program.h"
#include <stdlib.h>

namespace arti {
	int name_index = 0; // used for generting new names

	string create_sequenced_name() {
		return string_from_format("$_%d",name_index);
	}

	FeatureProgram::~FeatureProgram() {
		for_each(E, _formulaMap) delete E->second;
	}

}