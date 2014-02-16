#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include "systemex.h"

namespace arti {
	int experi_main(int argc, char* argv[]);

	class ArgList {
		private:
			std::map<std::string,std::string> values_;
		public:
			void reset(int argc, char * argv[]);
			const std::string& operator[](const std::string& k) const;
	};
	/**
	 * When an experiment is created, it is registered
	 * with the repository
	 */
	class Experiment {

		PREVENT_COPY(Experiment)
		public:
			void run(int argc=0,char *argv[]=nullptr);
			virtual ~Experiment(){};
			const char * name() const {return _name;}
			const std::string & description() const {return _description;}
		protected:
			/**
			 * name is used to create files
			 * description is for display on console
			 */
			Experiment(const char * name, const std::string description);
			// writes newline
			std::ostream& file();
			virtual void doRun() = 0;
			const ArgList& args() const {return args_;}
		private:
			const char * _name;
			const std::string _description;
			time_t start;
			std::ofstream ofile;
			ArgList args_;
	};

	class ExperimentRepository {
			friend class Experiment;
		public:
			static ExperimentRepository& instance();
			Experiment & find(const char * name);
			const std::list<Experiment*>& all() const {return _list;};
		private:
			void add(Experiment * value);
			static ExperimentRepository * _instance;
			ExperimentRepository();
		private:
			std::map<std::string, Experiment *> _map;
			std::list<Experiment*> _list;
	};

}

