#include "feat_program.h"
#include <stdlib.h>

namespace arti {

	string create_name(const int index) {
		return string_from_format("$_%d",index);
	}

	string FeatureProgram::name_of(const StateSet& states) {
		for_each(I,_stateMap) {
			if (I->second == states)
				return I->first;
		}
		string new_name = create_name(_name_index++);
		while (has_state_name(new_name))
			new_name = create_name(_name_index++);
		_stateMap[new_name] = states;
		return new_name;
	}

	string FeatureProgram::name_of(const Region& squares) {
		for_each(I,_regionMap) {
			if (I->second == squares)
				return I->first;
		}
		string new_name = create_name(_name_index++);
		while (has_region_name(new_name))
			new_name = create_name(_name_index++);
		_regionMap[new_name] = squares;
		return new_name;
	}

}