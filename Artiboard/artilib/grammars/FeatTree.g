	tree grammar FeatTree;

	options{ 
	  language = C;
		tokenVocab = Feat;
		ASTLabelType = pANTLR3_BASE_TREE;
	}

 	@includes {
		#include <map>
		#include <exception>
		#include <stdlib.h>
		#include "../board.h"
		#include "../feat_program.h"
		using namespace arti;
	}

	@members {
		std::map<pFeatTree,FeatureProgram *> programs;

		FeatureProgram * pgm(pFeatTree ctx) {
			auto result = programs.find(ctx);
			if (result == programs.end()) {
				auto newP = new FeatureProgram();
				programs[ctx] = newP;
				return newP;
			} else
				return result->second;
		}

		#define _ctx pgm(CTX)
	}
 
	program returns [FeatureProgram * value]
		:	clause+ {
				auto result = _ctx;
				programs.erase(CTX);
				return _ctx;
			}
		;

	clause
		: ^('region' n=ID r=region) {
				string name = (const char*)$n.text->chars;
				_ctx->add(name,$r.value);
			} 
		| ^('stateset' n=ID v=state_set) {
				string name = (const char*)$n.text->chars;
				_ctx->add(name,$v.value);
			}
		| ^('formula' ID expression+)
		| ^('function' ID fun_term+)	
		;

	fun_term
		: ^('*' FLOAT ID)
		| ^('*' FLOAT expression)	
		;

	expression
		: ^('@' state_set_ref region_ref)
		| ^('!' expression)
		| ^('|' expression expression)
		| ^('&' expression expression)
		;

	state_set_ref returns [string value]
	 : s = state_set {
	 			$value = _ctx->name_of(s);
	 		}
	 | n=ID {
		 		$value = (const char*)$n.text->chars;
	 				if (!_ctx->has_state_name($value))
	 			throw runtime_error("State set with name '" + $value + "' has not been defined");
 			}
 		;

	state_set returns [StateSet value]
	 : ^('{' (s=ID {
	 		 $value.insert((const char*)$s.text->chars);
	 	  })+)	
	 ;

	region_ref returns [string value]
	 : s = region {
	 			$value = _ctx->name_of(s);
	 		}
	 | n=ID {
	 		$value = (const char*)$n.text->chars;
	 		if (!_ctx->has_region_name($value))
	 			throw runtime_error("Region with name '" + $value + "' has not been defined");
	 }
	 ;

	region returns [Region value]
		: ^('{' (s=square{$value.insert(s);})+)
		;

	square returns [Square value]
		: ^(',' c=INTEGER r=INTEGER) {
			Square{
				(index_t) atoi((const char*)$c.text->chars),
				(index_t) atoi((const char*)$r.text->chars)};}
		;	