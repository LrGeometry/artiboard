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
	string create_sequenced_name();

	template<class valueT> class NameMap : public std::map<string, valueT> {
	public:
		typedef std::map<string, valueT> baseT;
		bool has_name(const string& name) const {return baseT::find(name) != baseT::end();}
		bool add(const string& name, const valueT& value) {
			if (has_name(name))
				throw runtime_error_ex("The name '%s' has already been defined for this scope", name.c_str());
			baseT::insert(std::pair<string,valueT>(name,value));
		}
		string assign_name(const valueT& value) {
			for (auto e = baseT::begin(); e != baseT::end(); e++) {
				if (e->second == value)
					return e->first;
			}
			string new_name = create_sequenced_name();
			while (has_name(new_name))
				new_name = create_sequenced_name();
			(new_name,value);
			return new_name;
		}
	};

	class FeatureProgram {
	public:
		typedef std::unique_ptr<FeatureProgram> u_ptr;
		NameMap<StateSet>& states() {return _stateMap;}
		NameMap<Region>& regions() {return _regionMap;}
	private:
		NameMap<StateSet> _stateMap;	
		NameMap<Region> _regionMap;	
	};
}