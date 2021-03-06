#include "experiment.h"
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "../systemex/log.h"

namespace experiments {
	Experiment::Experiment(const char * aName, const std::string aDescription) :
			_name(aName), _description(aDescription) {
		ExperimentRepository::instance().add(this);
	}

	std::ostream & Experiment::file() {
		if (!ofile.is_open()) {
			systemex::create_dir("..\\experiments");
			std::string fileName = systemex::string_from_format("..\\experiments\\%s.txt",
					_name);
			ofile.open(fileName.c_str());
			if (!ofile.is_open())
				throw systemex::runtime_error_ex("Cannot create file %s",
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
			throw systemex::runtime_error_ex("Could not find experiment with name: '%s'",name);
		return *(result->second);
	}

}
