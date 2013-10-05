#pragma once
#include <memory>
#include <set>
#include <string>
#include <map>
#include <list>
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
		valueT locate(const string& name) const {
			auto result = baseT::find(name);
			if (result == baseT::end())
				throw runtime_error_ex("The name '%s' cannot be located in this scope", name.c_str());
			return result->second;
		}
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


	class FeatureExpression {
	public:
		virtual ~FeatureExpression() {};
	};
	typedef std::unique_ptr<FeatureExpression> FeatureExpression_u_ptr;


	class GroundExpression : public FeatureExpression {

	public:
		GroundExpression(const string& s, const string& r): _stateset(s),_region(r) {}
		const string _stateset;
		const string _region; 
	};

	class UnaryExpression : public FeatureExpression {
	protected:
		UnaryExpression(FeatureExpression* o):_other(o){}
	private:
		const FeatureExpression_u_ptr _other;
	};

	class NotExpression : public UnaryExpression {
	public:
		NotExpression(FeatureExpression* o) : UnaryExpression(o) {}
	};


	class BinaryExpression : public FeatureExpression {
	protected:
		BinaryExpression(FeatureExpression* e1, FeatureExpression* e2): _e1(e1), _e2(e2) {}
	private:	
		const FeatureExpression_u_ptr _e1;
		const FeatureExpression_u_ptr _e2;

	};

	class AndExpression : public BinaryExpression {
	public:
		AndExpression(FeatureExpression* e1, FeatureExpression* e2): BinaryExpression(e1,e2) {}
	};

	class OrExpression : public BinaryExpression {
	public:
		OrExpression(FeatureExpression* e1, FeatureExpression* e2): BinaryExpression(e1,e2) {}
	};

	class FeatureTerm : public FeatureExpression {
	protected:
		FeatureTerm(float weight) : _weight(weight) {}
	private:
		float _weight;	
	};

	typedef std::unique_ptr<FeatureTerm> FeatureTerm_u_ptr;

	class FeatureTermWithFormula : public FeatureTerm {
	public:
		FeatureTermWithFormula(float weight, const string& formula_name) : FeatureTerm(weight), _formula_name(formula_name) {}
	private:
		const string _formula_name;
	};

	class FeatureTermWithExpression : public FeatureTerm {
	public:
		FeatureTermWithExpression(float weight, FeatureExpression * e) : FeatureTerm(weight),_e(e) {}
	private:
		FeatureExpression_u_ptr _e;
	};

	class FeatureFunction : public FeatureExpression {
	public:
		FeatureFunction():_terms() {};
		std::list<FeatureTerm_u_ptr>& terms() {return _terms;}
	private:
		std::list<FeatureTerm_u_ptr> _terms;
	};

	class FeatureProgram {
		PREVENT_COPY(FeatureProgram)
	public:
		FeatureProgram() {};
		typedef std::unique_ptr<FeatureProgram> u_ptr;
		NameMap<StateSet>& states() {return _stateMap;}
		NameMap<Region>& regions() {return _regionMap;}
		NameMap<FeatureExpression*>& formulas() {return _formulaMap;}
		NameMap<FeatureFunction*>& functions() {return _functionMap;}
		~FeatureProgram();
	private:
		NameMap<StateSet> _stateMap;	
		NameMap<Region> _regionMap;	
		NameMap<FeatureExpression*> _formulaMap;
		NameMap<FeatureFunction*> _functionMap;
	public:
		friend ostream& operator <<(std::ostream& os, const FeatureProgram& v);
	};

}