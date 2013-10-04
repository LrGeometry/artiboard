#include <vector>
#include <fstream>
#include "icu_data.h"
#include <feat.h>
#include <iostream>

using namespace artilib;

int main(int argc, char* argv[])
{
  try {
    auto program = load_program("../connect4/data/testfun.txt");
    delete program;
  } catch (std::exception &ex) {
    std::cout << "error";
  }

  return 0;
}
