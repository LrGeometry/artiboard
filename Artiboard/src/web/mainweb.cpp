#include <stdio.h>
#include <string.h>
#include "mongoose.h"
#include "../systemex/systemex.h"
#include "gamesite.h"
#include <iostream>
using systemex::string_from_file;
using namespace std;
using namespace web;

GameSite site;

// TODO Implement interactive game in HTML5
static const void *callback(enum mg_event event, struct mg_connection *conn,
		const struct mg_request_info *request_info) {
	HtmlDocument doc;
	try {
		if (event == MG_NEW_REQUEST) {
			Request req(request_info);
			doc.push_back(site.create(req));
		} else {
			return NULL;
		}
	} catch (std::runtime_error &ex) {
		doc.push_back(new Part("",ex.what()));
	} catch (...) {
		doc.push_back(new Part("","unknown error"));
	}
	doc.send(conn);
	return ""; // Mark as processed
}

int main(void) {
  struct mg_context *ctx;
  const char *options[] = {"listening_ports", "8080", NULL};

  ctx = mg_start(&callback, 0, options);
  cout << "Web server running at http://localhost:8080\nPress ENTER to exit\n" << endl;
  getchar();  // Wait until user hits "enter"
  mg_stop(ctx);
  cout << "Web server stopped" << endl;
  return 0;
}
