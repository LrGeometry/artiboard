#include <vector>
#include <fstream>
#include <feat.h>
#include <iostream>
#include <test_util.h>
using namespace arti;

int main(int argc, char* argv[])
{

  try {
  	test_main();
     auto program = load_program("../connect4/data/testfun.txt");
  } catch (std::exception &ex) {
    std::cout << "*** ERROR ***: " << ex.what() << std::endl;
  }

  return 0;
}
