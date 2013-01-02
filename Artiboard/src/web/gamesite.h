#pragma once
#include "html.h"

namespace web {

class GameSite {
public:
	Part * create(const Request &request);
};

}
