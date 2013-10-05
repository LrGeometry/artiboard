#pragma once
#include <memory>
#include <set>
#include <string>
#include <map>
#include "systemex.h"
#include "board.h"

namespace arti {
	typedef std::set<string> StateSet;
	typedef std::set<Square> Region;

	class FeatureProgram {
	public:
		typedef std::unique_ptr<FeatureProgram> u_ptr;
		/** Creates new name if states do not exist */
		string name_of(const StateSet& states);
		/** Creates new name if squares do not exist */
		string name_of(const Region& squares);
		bool has_state_name(const string& name) const {return _stateMap.find(name) != _stateMap.end();}
		bool has_region_name(const string& name) const {return _regionMap.find(name) != _regionMap.end();}
		void add(const string& name, const StateSet& value) {
			if (has_state_name(name))
				throw runtime_error_ex("State set with name '%s' has already been defined", name.c_str());
			_stateMap[name] = value;
		}
		void add(const string& name, const Region& value) {
			if (has_region_name(name))
				throw runtime_error_ex("Region with name '%s' has already been defined", name.c_str());
			_regionMap[name] = value;
		}
	private:
		std::map<string, StateSet> _stateMap;	
		std::map<string, Region> _regionMap;	
		int _name_index = 0; // used for generting new names
	};
}