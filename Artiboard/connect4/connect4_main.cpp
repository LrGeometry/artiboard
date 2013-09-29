#include <vector>
#include <fstream>
#include "icu_data.h"
#include <grammars/FeatLexer.h>
#include <grammars/FeatParser.h>
#include <antlr3.h> 
#include <iostream>

int main(int argc, char* argv[])
{
  pANTLR3_INPUT_STREAM input;
  pFeatLexer lex;
  pANTLR3_COMMON_TOKEN_STREAM tokens;
  pFeatParser parser;

  input = antlr3FileStreamNew((unsigned char *)"../connect4/data/testfun.txt",ANTLR3_ENC_8BIT);
  lex = FeatLexerNew(input);
  tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
                                            TOKENSOURCE(lex));
  parser = FeatParserNew(tokens);

  FeatParser_formula_return r = parser->formula(parser);

  pANTLR3_BASE_TREE tree = r.tree;

  std::cout << tree->toStringTree(tree)->chars;
  // ExprTreeEvaluator eval;
  // int rr = eval.run(tree);
  // cout << "Evaluator result: " << rr << '\n';

  parser->free(parser);
  tokens->free(tokens);
  lex->free(lex);
  input->close(input);

  return 0;
}
