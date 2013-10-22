#include "icu_data.h"
#include <log.h>

const size_t column_count = 7;
const size_t row_count = 6;

static IcuData * loaded = 0;

IcuData& IcuData::instance() {
	if (!loaded) {
		loaded = new IcuData();
		LOG << "ICU Data loaded";
	}
	return *loaded;
}

IcuData::IcuData()  {
	std::ifstream data("../connect4/data/connect-4.data");
	std::string s;
	while (data) {
		data >> s;
		if (!data)
			return;
		FOR_EACH(c,s) 
			if (*c == ',') *c = ' ';
		std::stringstream line(s);
		arti::Board b;
		std::string v;
		for (auto i = 0U; i < column_count; i++)
			for (auto j = 0U; j < row_count; j++) {
				line >> v;
				if (v != "b")
					b(i,j,arti::Piece(v[0]));
				else
					b(i,j,arti::Piece('-'));
			}
		MatchOutcome r = MatchOutcome::Draw;
		line >> v;
		if (v == "win")
			r = MatchOutcome::SouthPlayerWins;
		else if (v == "loss")
			r = MatchOutcome::NorthPlayerWins;
		insert({b,r});
	}	
}