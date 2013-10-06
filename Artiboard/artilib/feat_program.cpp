#include "feat_program.h"
#include <stdlib.h>
#include "log.h"
namespace arti {
	int name_index = 0; // used for generting new names

	string create_sequenced_name() {
		return string_from_format("e_%d",name_index++);
	}

	FeatureProgram::~FeatureProgram() {
		for_each(E, _formulaMap) delete E->second;
		for_each(E, _functionMap) delete E->second;
	}

	template<class T> ostream& operator <<(std::ostream& os, const std::set<T>& v) {
		os << "{";
		for (auto e = v.cbegin(); e != v.cend(); e++) {os << *e << " ";}
		os << "}";
		return os;
	}

	void FeatureFunction::to_stream(std::ostream& os) const {
		for (auto e = _terms.cbegin(); e != _terms.cend(); e++) {
			if (e != _terms.cbegin())
				os << " + ";
			os << *(*e);
		}
	}

	ostream& operator <<(std::ostream& os, const FeatureProgram& v) {
		for (auto e = v._stateMap.cbegin(); e != v._stateMap.cend(); e++) 
			os << "stateset " << e->first << " = " << e->second << ";" << std::endl;
		for (auto e = v._regionMap.cbegin(); e != v._regionMap.cend(); e++) 
			os << "region " << e->first << " = " << e->second << ";" << std::endl;
		for (auto e = v._formulaMap.cbegin(); e != v._formulaMap.cend(); e++) 
			os << "formula " << e->first << " = " << *(e->second)  << std::endl;
		for (auto e = v._functionMap.cbegin(); e != v._functionMap.cend(); e++) 
			os << "function " << e->first << " = " << *(e->second) << std::endl;
		return os;
	}	
}