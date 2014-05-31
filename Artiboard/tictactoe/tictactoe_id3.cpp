#include <experiment.h>
#include <log.h>
#include <id3.h>
#include "t3_icu.h"

using namespace arti;

class T3UCIData: public Experiment {
public:
		T3UCIData(): Experiment("t3-050","Display information about the ICU data") {}
		void do_run() override {
			const auto filename = args()["data_dir"] + "\\tic-tac-toe.data";
			file() << "Name of file: " << filename.c_str();
			T3IcuData data(filename);
			file() << data.calculate_stats();
		}
} t3ucidata;
