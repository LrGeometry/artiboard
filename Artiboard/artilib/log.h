#pragma once
#include <string>
#include <fstream>
#include <time.h>
#include "systemex.h"

#define LOG ::arti::Log::instance().record()
namespace arti {

	class Log {
			PREVENT_COPY(Log)
		public:
			static Log& instance();
		public:
			Log(const std::string& fileName);
			std::ostream& record();
			std::ostream& file() {return _file;}
			virtual ~Log();
		private:
			std::ofstream _file;
			time_t _start;
			time_t _last;
	};

}
