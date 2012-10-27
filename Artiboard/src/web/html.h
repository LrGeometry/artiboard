// see licence.txt

#pragma once
#include <vector>
#include "../systemex/systemex.h"
#include <ostream>
#include "mongoose.h"

namespace web {
	using std::list;
	using std::string;
	using std::ostream;

	class Part : public list<Part *> {
		public:
			Part(const string& outer, const string& inner);
			const char * outer() const {return _outer.c_str();}
			const char * inner() const {return _inner.c_str();}
			virtual ~Part();
		private:
			const string _outer;
			const string _inner;
	};


	class HtmlDocument : public Part {

		public:
			HtmlDocument();
			void send(mg_connection * conn) const;
	};


	ostream & operator <<(ostream & os, const Part & s);

}
