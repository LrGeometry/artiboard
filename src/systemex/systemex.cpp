#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include "systemex.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

using namespace std;

namespace systemex {

	char * duplicate_string(const char *str) {
		size_t len = strlen(str);
		/* 1 for the null terminator */
		char *result = static_cast<char *>(malloc(len + 1));
		ENSURE(result != 0, "could not allocate memory");
		memcpy(result, str, len + 1);
		return result;
	}

	void create_dir(const string& path) {
			if (mkdir(path.c_str()) != 0) {
				if (errno != EEXIST)
					throw runtime_error_ex("could not create directory:'%s'",path.c_str());
			}
	}

	string FormatString(const char *szFormat, ...) {
		char buffer[1024];
		buffer[sizeof(buffer) - 1] = 0;
		va_list args;
		va_start(args, szFormat);
		vsnprintf(buffer, sizeof(buffer) - 1, szFormat, args);
		va_end(args);
		return string(buffer);
	}

	static const char *PARSE_ERROR = "Looking for '%s' but found '%s'";
	file_not_found::file_not_found(const char *str) :
			runtime_error_ex("File could not be opened:%s", str) {

	}

	not_implemented::not_implemented(const char *str) :
			runtime_error_ex("Not implemented:%s", str) {

	}

	parse_error::parse_error(const char *strExpected, const char *strFound) :
			runtime_error_ex(PARSE_ERROR, strExpected, strFound) {
	}

	runtime_error_ex::runtime_error_ex(const char *szFormat, ...) :
			runtime_error("Extended") {
		char buffer[1024];
		buffer[sizeof(buffer) - 1] = 0;
		va_list args;
		va_start(args, szFormat);
		vsnprintf(buffer, sizeof(buffer) - 1, szFormat, args);
		va_end(args);
		m_message = std::string(buffer);
	}

	runtime_error_ex::runtime_error_ex(void) :
			runtime_error("Extended") {
	}

	runtime_error_ex::~runtime_error_ex() throw () {
	}

	const char *runtime_error_ex::what() const throw () {
		return m_message.c_str();
	}

	Answer::Answer() {
		_what = 0;
		_value = true;
	}

	Answer::Answer(bool value, const char * format, ...) {
		char buffer[1024];
		buffer[sizeof(buffer) - 1] = 0;
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer) - 1, format, args);
		va_end(args);
		_what = duplicate_string(buffer);
		_value = value;
	}

	Answer::Answer(Answer&& rhs) {
		moveData(static_cast<Answer&&>(rhs));
	}

	Answer::~Answer() {
		if (_what != 0)
			free(_what);
	}

	Answer& Answer::operator=(Answer &&rhs) {
		moveData(static_cast<Answer&&>(rhs));
		return *this;
	}

	const char * Answer::what() const {
		if (_what == 0)
			return "No explanation";
		return _what;
	}

	void Answer::moveData(Answer&& rhs) {
		_what = rhs._what;
		_value = rhs._value;
		rhs._what = 0;
	}

}
