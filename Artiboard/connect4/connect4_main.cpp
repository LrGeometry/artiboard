#include <vector>
#include <fstream>
#include <feat.h>
#include <iostream>
#include <test_util.h>
#include <log.h>
using namespace arti;

int main(int argc, char* argv[])
{

  try {
    // auto program = load_program("../connect4/data/testfun.txt");
    test_main();
    LOG << "Done";
  } catch (std::exception &ex) {
    LOG << "*** ERROR ***: " << ex.what() << std::endl;
  }

  return 0;
}
