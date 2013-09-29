#include <vector>
#include <fstream>
#include "icu_data.h"

int main(int argc, const char * args[]) {
	IcuData data;
	auto items = data.entries();
	//for_each(e,items) {LOG << (*e).board();}
	LOG << items.size();
	LOG << sizeof(IcuEntry) * items.size() / 1024 / 1024 << " mb";
}