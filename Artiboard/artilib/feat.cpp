#include "feat.h"
#ifdef _MSC_BUILD
namespace arti {
	FeatureProgram::u_ptr load_program(const std::string& filename) {
		throw std::exception("not implemented");
	}
}
#else
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
