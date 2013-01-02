// see licence.txt

#include "html.h"
#include <sstream>

namespace web {
	using std::stringstream;

	Part::Part(const string& outerTag, const string& innerBody) : _outer(outerTag), _inner(innerBody) {}

	Part::~Part() {
		for_each(e, (*this))
			delete *e;
	}
	ostream & operator <<(ostream & os, const Part & p) {
		const bool hasOuter = *p.outer() != 0;
		if (hasOuter)
			os << "<" << p.outer() << ">";
		const bool hasInner = *p.inner() != 0;
		if (hasInner)
			os << p.inner();
		for_each(e, p)
			os << *(*e);
		if (hasOuter)
			os << "</" << p.outer() << ">";
		return os;
	}

	HtmlDocument::HtmlDocument() : Part("body","") {}

	void HtmlDocument::send(mg_connection * conn) const {
		stringstream ss;
		ss << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!doctype html><html>"
		   << "<head><title>AtriBoard</title></head>"
		   << (*this)
		   << "</html>";
		auto s = ss.rdbuf()->str();
		mg_write(conn,s.c_str(),s.length());
	}
}
