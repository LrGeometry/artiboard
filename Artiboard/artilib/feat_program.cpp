#include "feat_program.h"
#include <stdlib.h>
#include "log.h"
namespace arti {
	int name_index = 0; // used for generting new names

	string create_sequenced_name() {
		return string_from_format("$_%d",name_index);
	}

	FeatureProgram::~FeatureProgram() {
		for_each(E, _formulaMap) delete E->second;
		for_each(E, _functionMap) delete E->second;
	}

	template<class T> ostream& operator <<(std::ostream& os, const NameMap<T>& v) {
		for (auto e = v.cbegin(); e != v.cend(); e++) 
			os << e->first << " = " << e->second << ";" << std::endl;
		return os;
	} 

	template<class T> ostream& operator <<(std::ostream& os, const std::set<T>& v) {
		os << "{";
		for (auto e = v.cbegin(); e != v.cend(); e++) {os << *e << " ";}
		os << "}";
		return os;
	}

	ostream& operator <<(std::ostream& os, const FeatureProgram& v) {
		os << "PROG ";
		os << v._stateMap << v._regionMap
		;
		return os;
	}
	
}