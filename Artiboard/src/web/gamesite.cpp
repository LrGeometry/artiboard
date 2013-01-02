/*
 * gamesite.cpp
 *
 *  Created on: 31 Oct 2012
 *      Author: willemd
 */

#include "gamesite.h"
using namespace systemex;

namespace web {

Part * GameSite::create(const Request& request) {
	return new Part("",string_from_file("src/web/content/index.html"));
}

}
