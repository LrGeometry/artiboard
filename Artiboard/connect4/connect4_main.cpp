#include <vector>
#include <fstream>
#include "icu_data.h"
#include <feat.h>
#include <iostream>

using namespace arti;

int main(int argc, char* argv[])
{

  try {
    auto program = load_program("../connect4/data/testfun.txt");
  } catch (std::exception &ex) {
    std::cout << "*** ERROR ***: " << ex.what() << std::endl;
  }

  return 0;
}
