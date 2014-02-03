#include <vector>
#include <fstream>
#include <feat.h>
#include <iostream>
#include <test_util.h>
#include <experiment.h>
#include <systemex.h>
#include <log.h>
using namespace arti;

int main(int argc, char* argv[])
{

   try {
    LOG << argc << " argument(s) supplied:";
    for (int c=0; c<argc;c++) LOG << "\t" << argv[c];
    if (argc == 1) 
      throw runtime_error_ex("incorrect usage, expected:\n\t%s  experiment_name>|test|list",argv[0]);
    std::string name(argv[1]);
    if (name == "test")
      test_main();
    else if (name == "list")
      for (const auto& e: ExperimentRepository::instance().all()) {LOG << e->name() << "\t" << e->description();}
    else
      ExperimentRepository::instance().find(argv[1]).run();
    LOG << "Done";
  } catch (std::exception &ex) {
    LOG << "*** ERROR ***: " << ex.what() << std::endl;
  }

  return 0;
}
