#include <iostream>
#include <stdio.h>
#include <time.h>
#include "experiment.h"
#include "log.h"

namespace arti {

	int experi_main(int argc, char* argv[])
	{
		try {
			std::cout << "started" << std::endl;
			LOG << argc << " argument(s) supplied:";
			for (int c=0; c<argc;c++) LOG << "\t" << argv[c];
			if (argc == 1)
				throw runtime_error_ex("incorrect usage, expected:\n\t%s  experiment_name>|test|list",argv[0]);
			std::string name(argv[1]);
			if (name == "list")
				for (const auto& e: ExperimentRepository::instance().all()) {LOG << e->name() << "\t" << e->description();}
			else
				ExperimentRepository::instance().find(argv[1]).run();
			LOG << "Done";
			std::cout << "ended OK" << std::endl;
			return 0;
		} catch (std::exception &ex) {
			LOG << "*** ERROR ***: " << ex.what() << std::endl;
			std::cout << "error:" << ex.what() << std::endl << "ended" << std::endl;
			return 1;
		}
	}


	Experiment::Experiment(const char * aName, const std::string aDescription) :
			_name(aName), _description(aDescription), start(0), ofile() {
		ExperimentRepository::instance().add(this);
	}

	std::ostream & Experiment::file() {
		if (!ofile.is_open()) {
			create_dir("..\\experiments");
			std::string fileName = string_from_format("..\\experiments\\%s.txt",
					_name);
			ofile.open(fileName.c_str());
			if (!ofile.is_open())
				throw runtime_error_ex("Cannot create file %s",
						fileName.c_str());
		} else
			ofile << std::endl;
		return ofile;
	}


	void Experiment::run() {
		time(&start);
		LOG << "Start " << _name << ":" << _description;
		doRun();
		if (ofile.is_open()) {
			ofile << std::endl;
			ofile.close();
		}
		LOG << "Complete " << _name << std::endl;
	}

	ExperimentRepository* ExperimentRepository::_instance = 0;

	ExperimentRepository& ExperimentRepository::instance() {
		if (_instance == 0)
			_instance = new ExperimentRepository();
		return *_instance;
	}

	ExperimentRepository::ExperimentRepository() {

	}

	void ExperimentRepository::add(Experiment * value) {
		_map[std::string(value->name())] = value;
		_list.push_back(value);
	}

	Experiment & ExperimentRepository::find(const char * name) {
		auto result = _map.find(std::string(name));
		if (result == _map.end())
			throw runtime_error_ex("Could not find experiment with name: '%s'",name);
		return *(result->second);
	}

}
