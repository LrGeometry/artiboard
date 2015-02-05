#ifdef _MSC_BUILD
#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE
#define BOOST_VARIANT_MINIMIZE_SIZE
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "feat_program.h"
#include "feat.h"

namespace arti {
	using namespace boost::spirit;
	using namespace boost::spirit::ascii;
	using base_iterator_t = std::string::iterator;
	using token_t = lex::lexertl::token < base_iterator_t, boost::mpl::vector<unsigned int, std::string> > ;
	using lexer_t = lex::lexertl::lexer < token_t > ;

	struct Tokenizer : lex::lexer < lexer_t > {
		Tokenizer() {
			identifier = "[a-z|A-Z][a-z|A-Z|0-9]*";
			index = "[0-7]";
			weight = "[0-9]+[.][0-9]+";
			formula = "formula";
			function = "function";
			stateset = "stateset";
			region = "region";
			this->self = formula | function | stateset | region ;
			this->self += lex::token_def<>('=')
				| '{' | '}' | '(' | ')' | ',' | '@' | '|' | '&' | '!' | '*' | '+' | ';';
			this->self += identifier | index | weight;

			this->self("WS") = lex::token_def<>("[ \\t\\r\\n]+")
				| "\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/";

		}
		lex::token_def<string> identifier;
		lex::token_def<unsigned int> index;
		lex::token_def<float> weight;
		lex::token_def<> formula, stateset, function, region;
	};

	using skipper_t = qi::in_state_skipper < Tokenizer::lexer_def > ;
	struct Grammar : qi::grammar < Tokenizer::iterator_type, skipper_t > {
		Tokenizer t;
		using rule_t = qi::rule < Tokenizer::iterator_type, skipper_t > ;
		using rule_named_t = qi::rule < Tokenizer::iterator_type,arti::string(), skipper_t>;
		rule_t   program_;
#define TYPED_RULE(T) qi::rule < Tokenizer::iterator_type,T, skipper_t>
		using expression_t = FeatureExpression *;
		using feature_term_t = FeatureTerm *;
		using feature_fun_t = FeatureFunction *;
		using string_pair_t = std::pair<string,string>;
		TYPED_RULE(arti::Square()) square;
		TYPED_RULE(arti::Region()) square_set;
		TYPED_RULE(arti::StateSet()) state_set;
		TYPED_RULE(string_pair_t()) ground;
		TYPED_RULE(expression_t()) andExpr, orExpr,	term;
		TYPED_RULE(feature_term_t()) fun_term;
		TYPED_RULE(feature_fun_t()) feature_fun;
		rule_named_t region, ref_state, ref_region, formula , function, stateset;
		Grammar() : Grammar::base_type(program_) {
			using boost::spirit::_val;
			using boost::phoenix::ref;
			using boost::phoenix::size;
			program_ = *(region | stateset | formula | function) >> qi::eoi;

			auto add_region = [&](const string& name, arti::Region& r) {
				result->regions().add(name,r);
			};
			region = t.region >> 
				t.identifier[_val = _1] >> '=' >> square_set [bind(add_region,_val,_1)] 
				>> ';'
				;
			
			auto add_stateset = [&](string& n, arti::StateSet & e) {
				result->states().add(n,e);
			};
			stateset = t.stateset >> t.identifier[_val=_1] >> '=' >> 
				state_set [bind(add_stateset,_val,_1)] >> ';';

			auto add_function = [&](string& n, feature_fun_t e) {
				if (!e) throw std::exception("t is null");
				result->functions().add(n,e);
			};
			function = t.function >> t.identifier[_val = _1] >> '=' 
				>> feature_fun [bind(add_function,_val,_1)] >> ';';
			auto mk_function = [](feature_term_t t) {
				feature_fun_t f = new FeatureFunction();
				f -> terms().emplace_back(upFeatureTerm(t));
				return f;
			};

			auto add_term = [](feature_fun_t f, feature_term_t t) {
				f -> terms().emplace_back(upFeatureTerm(t));
			};
			feature_fun = fun_term [_val = bind(mk_function,_1)]  
				>> *('+' >> fun_term [bind(add_term,_val,_1)]);

			auto add_formula = [&](string& n, expression_t e) {
				result->formulas().add(n,e);
			};
			formula = t.formula >> t.identifier [_val = _1] >> '=' >> orExpr[bind(add_formula,_val,_1)] >> ';';

			auto mk_ft = [](float a) {
				return new FeatureTermDummy(a);
			};

			auto mk_fte = [](feature_term_t a,expression_t b) {
				auto w = a->weight();
				delete a;
				return new FeatureTermWithExpression(w,b);
			};

			auto mk_ftf = [&](feature_term_t a,string& b) {
				auto w = a->weight();
				delete a;
				result->formulas().check_name(b); // make sure the formula exists
				return new FeatureTermWithFormula(w,b);
			};
			fun_term = t.weight[_val = bind(mk_ft,_1)] >> '*' >> 
					(orExpr [_val = bind(mk_fte,_val,_1)] 
				| t.identifier [_val = bind(mk_ftf,_val,_1)]);

			auto mk_ground = [&](string_pair_t&o) {
				return new GroundExpression(o.first,o.second);
			};
			auto mk_not = [](expression_t a) {
				return new NotExpression(a);
			};
			term = ground [_val = bind(mk_ground,_1)] 
				| '(' >> orExpr [_val=_1] >> ')' 
				| '!' >> term [_val = bind(mk_not,_1)];

			auto mk_or = [](expression_t a, expression_t b) {
				return new OrExpression(a,b);
			};
			orExpr = andExpr[_val = _1] >> *('|' >> andExpr[_val = bind(mk_or,_val,_1)]);

			auto mk_and = [](expression_t a, expression_t b) {
				return new AndExpression(a,b);
			};
			andExpr = term [_val =_1] >> *('&' >> term [_val = bind(mk_and,_val,_1)]);

			auto upd_pair = [&](string_pair_t&o, bool first, string& v) {
				if (first) {
					o.first = v;
				}	else {
					o.second = v;
					result->states().check_name(o.first);
					result->regions().check_name(o.second);
				}
			};
			ground = ref_state [bind(upd_pair,_val,true,_1)] 
				>> '@' >> 
					ref_region [bind(upd_pair,_val,false,_1)]
			;

			auto name_states = [&](StateSet&s) {return result->states().assign_name(s);}; 
			ref_state = 	
					( state_set [_val = bind(name_states,_1)] ) 
					| t.identifier [_val = _1]
					;

			auto name_squares = [&](Region &s) {
				return result->regions().assign_name(s);
			};
			ref_region = 	square_set [_val = bind(name_squares,_1)] | t.identifier [_val = _1];

			auto add_state = [](StateSet& s, const string& n){
				s.insert(n);
			};

			state_set = '{' >> +t.identifier[bind(add_state,_val,_1)] >> '}' 
			;
			
			auto add_sq = [](Region& r, Square &s) {r.insert(s);};
			square_set = ('{' >> +square [bind(add_sq,_val,_1)] >> '}');
			
			auto mk_sq = [](unsigned int r, unsigned int c){return arti::Square(r,c);};
			square = (t.index >> ',' >> t.index) 
				[ _val = bind(mk_sq,_1,_2) ]
				;
			using namespace qi::labels;
		}

		void parse(std::string& str) {
			std::string::iterator it = str.begin();
			iterator_type iter = t.begin(it, str.end());
			iterator_type end = t.end();
			bool r = qi::phrase_parse(iter, end, *this, qi::in_state("WS")[t.self]);
			if (!r || iter < end) {
				std::stringstream ss;
				ss << "Parsing failed\n";
				if (it != str.end()) {
					// count new lines
					int line = 0;
					std::string::iterator line_pos = str.begin();
					for (auto cit = str.begin();cit != it;cit++) {
						if (*cit == '\n') {
							line++;
							line_pos = cit;
						};
					}
					ss << "Unexpected token at line: " << line+1;
					if (line_pos < it) 
						ss << " col: " << (it - line_pos);
					else
						ss << "(end of line)";
					if (*it == ' ') ss << ". Before the space.";
					else if (*it == '\n') ss << ". The last token in the line.";
					else ss << ". Before this :>" << *it << "<:";
				} else
					ss << "Unexpected end of file\n";
				throw std::runtime_error(ss.str());
			} 
		}
	
		FeatureProgram::u_ptr result = FeatureProgram::u_ptr(new FeatureProgram());
	};

	FeatureProgram::u_ptr parse_program(std::string& str) {
		Grammar g;
		g.parse(str);
		return std::move(g.result);
	};

	FeatureProgram::u_ptr load_program(const std::string& filename) {
		return parse_program(string_from_file(filename.c_str()));
	}
}
#else
#include "feat.h"
#include "grammars/FeatLexer.h"
#include "grammars/FeatParser.h"
#include "grammars/FeatTree.h"

#include <antlr3.h> 
#include <iostream>

namespace arti {
	FeatureProgram::u_ptr load_program(const std::string& filename) {
		pANTLR3_INPUT_STREAM input;
		pFeatLexer lex;
		pANTLR3_COMMON_TOKEN_STREAM tokens;
		pFeatParser parser;
		FeatureProgram * result = 0;
		input = antlr3FileStreamNew((unsigned char *) filename.c_str(),ANTLR3_ENC_8BIT);
		if (!input)
			throw runtime_error_ex("file '%s' not found",filename.c_str());
		lex = FeatLexerNew(input);
		tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
			TOKENSOURCE(lex));
		parser = FeatParserNew(tokens);

		FeatParser_program_return r = parser->program(parser);

		pANTLR3_BASE_TREE tree = r.tree;

		// I should implement a custom displayRecognitionError(), but have to figure out how first
		const bool hasErrors = parser->pParser->rec->state->errorCount > 0;
		if (!hasErrors) {
			auto parseTree = antlr3CommonTreeNodeStreamNewTree(tree,ANTLR3_SIZE_HINT);
			auto featTree = FeatTreeNew(parseTree);
			result = featTree->program(featTree);
			featTree->free(featTree);
		} else {
			LOG << tree->toStringTree(tree)->chars << std::endl;
		}
		parser->free(parser);
		tokens->free(tokens);
		lex->free(lex);
		input->close(input);
		if (!result)
			throw runtime_error_ex("There were parse errors in '%s'", filename.c_str());
		return FeatureProgram::u_ptr(result);
	}
}

#endif
