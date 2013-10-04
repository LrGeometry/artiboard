	tree grammar FeatTree;

	options{ 
	  language = C;
		tokenVocab = Feat;
		ASTLabelType = pANTLR3_BASE_TREE;
	}

 	@includes {
		#include <map>
		#include <exception>
		#include "../feat_program.h"
		using namespace artilib;
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
		: ^('region' ID square_set) 
		| ^('stateset' ID state_set)
		| ^('formula' ID expression+)
		| ^('function' ID fun_term+)	
		;

	fun_term
		: ^('*' FLOAT ID)
		| ^('*' FLOAT expression)	
		;

	expression
		: ^('@' state_set square_set)
		| ^('!' expression)
		| ^('|' expression expression)
		| ^('&' expression expression)
		;

	square_set
		: ^('{' square+)
		| ID
		;

	state_set
	 : ^('{' ID+)	
	 | ID
	 ;

	square 
		: ^(',' INTEGER INTEGER)	//{throw std::exception();}
		;	