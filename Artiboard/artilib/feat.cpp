#include "feat.h"

#include "grammars/FeatLexer.h"
#include "grammars/FeatParser.h"
#include "grammars/FeatTree.h"

#include <antlr3.h> 
#include <iostream>

namespace artilib {
FeatureProgram * load_program(const std::string& filename) {
    pANTLR3_INPUT_STREAM input;
    pFeatLexer lex;
    pANTLR3_COMMON_TOKEN_STREAM tokens;
    pFeatParser parser;

    input = antlr3FileStreamNew((unsigned char *) filename.c_str(),ANTLR3_ENC_8BIT);
    lex = FeatLexerNew(input);
    tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
                                              TOKENSOURCE(lex));
    parser = FeatParserNew(tokens);

    FeatParser_program_return r = parser->program(parser);

    pANTLR3_BASE_TREE tree = r.tree;

    std::cout << tree->toStringTree(tree)->chars << std::endl;

    auto parseTree = antlr3CommonTreeNodeStreamNewTree(tree,ANTLR3_SIZE_HINT);
    auto featTree = FeatTreeNew(parseTree);
    auto result = featTree->program(featTree);

    featTree->free(featTree);
    parser->free(parser);
    tokens->free(tokens);
    lex->free(lex);
    input->close(input);
    return result;
}

}
