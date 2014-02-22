#include <iostream>
#include <stdio.h>
#include <time.h>
#include "experiment.h"
#include "log.h"
#include <map>
namespace {
}
namespace arti {
	void ArgList::reset(int argc, char * argv[]) {
		values_.clear();
		for (int i=0; i < argc; i++) {
			std::string e(argv[i]);
			auto p = e.find("=");
			if (p != std::string::npos)
				values_.emplace(std::string(e,0,p),std::string(e,p+1));
		}
	}

	const std::string& ArgList::operator[](const std::string& k) const {
		auto it = values_.find(k);
		if (it == values_.end())
			throw runtime_error_ex("Argument with name '%s' was not provided.",k.c_str());
		return it->second;
	}


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
				for (auto e: ExperimentRepository::instance().all())
					std::cout << e->name() << "\t" << e->description() << std::endl;
			else
				ExperimentRepository::instance().find(argv[1]).run(argc-2,argv+2);
			LOG << "Done";
			std::cout << "ended OK" << std::endl;
			return 0;
		} catch (std::exception &ex) {
			LOG << "*** ERROR ***: " << ex.what() << std::endl;
			std::cout << "error:" << ex.what() << std::endl << "ended" << std::endl;
			return 1;
		}
	}

	std::list<const Experiment*> ExperimentRepository::all() const {
		std::list<const Experiment*> result;
		for (auto &e : map_) result.push_back(e.second);
		return result;
	}

	Experiment::Experiment(const char * aName, const std::string aDescription) :
			name_(aName), description_(aDescription), start_(0), ofile_() {
		ExperimentRepository::instance().add(this);
	}

	std::ostream & Experiment::file() {
		if (!ofile_.is_open()) {
			create_dir("..\\experiments");
			std::string fileName = string_from_format("..\\experiments\\%s.txt",
					name_);
			ofile_.open(fileName.c_str());
			if (!ofile_.is_open())
				throw runtime_error_ex("Cannot create file %s",
						fileName.c_str());
		} else
			ofile_ << std::endl;
		return ofile_;
	}


	void Experiment::run(int argc, char* argv[]) {
		args_.reset(argc,argv);
		time(&start_);
		LOG << "Start " << name_ << ":" << description_;
		do_run();
		if (ofile_.is_open()) {
			ofile_ << std::endl;
			ofile_.close();
		}
		LOG << "Complete " << name_ << std::endl;
	}

	ExperimentRepository* ExperimentRepository::instance_ = 0;

	ExperimentRepository& ExperimentRepository::instance() {
		if (instance_ == 0)
			instance_ = new ExperimentRepository();
		return *instance_;
	}

	ExperimentRepository::ExperimentRepository() {

	}

	void ExperimentRepository::add(Experiment * value) {
		map_[std::string(value->name())] = value;
	}

	Experiment & ExperimentRepository::find(const char * name) {
		auto result = map_.find(std::string(name));
		if (result == map_.end())
			throw runtime_error_ex("Could not find experiment with name: '%s'",name);
		return *(result->second);
	}

}
