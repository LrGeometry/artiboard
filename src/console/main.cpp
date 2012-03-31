#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>
#include <iostream>
#include "../experiment/experiment.h"
#include <cstring>
#include "../systemex/log.h"
using std::exception;
using std::cerr;
using std::endl;
using experiments::ExperimentRepository;

namespace tut {
	test_runner_singleton runner;
}

void run(const char * experimentName) {
	try {
		ExperimentRepository::instance().find(experimentName).run();
	} catch (std::runtime_error &ex) {
		std::cerr << std::endl << ex.what() << std::endl;
	} catch (...) {
		std::cerr << "unknown exception occurred";
	}
}

void showExperiments() {
	auto &lst = experiments::ExperimentRepository::instance().all();
  for_each(e,lst) {
  	std::cout
  	    << (*e)->name()
  	    << ": "
  	    << (*e)->description()
  			<< std::endl;
  }
}

int main(int argc, const char * args[]) {
	try {
		tut::console_reporter reporter(systemex::Log::instance().file());
		tut::runner.get().set_callback(&reporter);
		tut::runner.get().run_tests();
		LOG << "Tests complete";
		if (reporter.all_ok()) {
			std::cout << "Tests ran ok" << std::endl;
			if (argc != 2) {
				std::cout << "To run an experiment, use: agent.exe <experiment name>" << std::endl;
				std::cout << "Possible experiments" << std::endl;
				showExperiments();
			} else if (strcmp(args[1],"all") == 0) {
				for_each(e,ExperimentRepository::instance().all())
					(*e)->run();
			} else {

				run(args[1]);
			}
		} else {
			std::cout << "Tests FAILED!";
		}
		LOG << "Program complete";
	#ifdef _DEBUG
		system("pause");
	#endif
	} catch (std::exception &ex) {
		std::cout << "an error occurred: " << ex.what() << std::endl;
		return 0;
	} catch (...) {
		std::cout << "an error occurred";
		return 0;
	}
}
