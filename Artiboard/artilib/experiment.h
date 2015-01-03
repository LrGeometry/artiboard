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
			const char * name() const {return name_;}
			const std::string & description() const {return description_;}
		protected:
			/**
			 * name is used to create files
			 * description is for display on console
			 */
			Experiment(const char * name, const std::string description);
			// writes newline
			std::ostream& file();
			virtual void do_run() = 0;
			const ArgList& args() const {return args_;}
			const string& data_dir() const {return args_["data_dir"];}
			const string data_fn(const string& filename) {return data_dir() + "/" + filename;}
			void set_steps(int v) {steps_ = v;}
			void step() {at_step++; std::cout << " at " << ((at_step*10000) / steps_)/100.0f << "%" << std::endl;}
		private:
			const char * name_;
			const std::string description_;
			time_t start_;
			std::ofstream ofile_;
			ArgList args_;
			int steps_ = 1;
			int at_step = 0;
	};

	class ExperimentRepository {
			friend class Experiment;
		public:
			static ExperimentRepository& instance();
			Experiment & find(const char * name);
			/* The experiments sorted by name.  */
			std::list<const Experiment *> all() const;
		private:
			void add(Experiment * value);
			static ExperimentRepository * instance_;
			ExperimentRepository();
		private:
			std::map<std::string, Experiment *> map_;
	};

}

