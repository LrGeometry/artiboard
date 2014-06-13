#include "log.h"
#include <ostream>

namespace arti {
	static Log s_instance("log.txt");

	Log& Log::instance() {
		return s_instance;
	}

	Log::Log(const std::string& filename) : _file(filename.c_str()) {
		time(&_start);
		LOG << "*** START";
	}

	std::ostream& Log::record() {
		time(&_last);
		_file << " <:" << std::endl << _last - _start << ":> ";
		return _file;
	}

	std::ostream& Log::newline() {
		_file << std::endl;
		return _file;
	}
	Log::~Log() {
		record() << "*** END\n";
	}

}
