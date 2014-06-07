#include <experiment.h>
#include <log.h>
#include <id3.h>
#include "t3_icu.h"
#include <id3.h>

using namespace arti;

class T3UCIExperiment: public Experiment {
	public:
		T3UCIExperiment(const char * name, const std::string description):
			Experiment(name,description) {}
		string data_filename() const {
			return args()["data_dir"] + "\\tic-tac-toe.data";
		}
};

class T3UCIDataStats: public T3UCIExperiment {
public:
		T3UCIDataStats(): T3UCIExperiment("t3-050","Display information about the ICU data") {}
		void do_run() override {
			const auto filename = data_filename();
			file() << "Name of file: " << filename.c_str();
			T3IcuData data(filename);
			file() << data.calculate_stats();
		}
} t3ucidatastats;


class T3ICUID3 : public T3UCIExperiment {
public:
		T3ICUID3(): T3UCIExperiment("t3-060","Classify the ICU data"){}
		/*
		 * @startuml
		 * class T3IcuData
		 * class ID3Classifier
		 * OutcomeDataID3 --|> ID3Classifier
		 * OutcomeDataID3 o-->  OutcomeDataIndexed
		 * class OutcomeDataIndexed
		 * note top of OutcomeDataIndexed : Indexed payload -- built from T3IcuData
		 * OutcomeDataIndexed ..> OutcomeData
		 * T3IcuData --|> OutcomeData
		 * note top of T3IcuData : The payload
		 * OutcomeDataIndexed --|> ID3NameResolver
		 * @enduml
		 */
		void do_run() override {
			const auto filename = data_filename();
			file() << "Name of file: " << filename.c_str();
			T3IcuData data(filename);
			file() << data.calculate_stats();
		}
};
