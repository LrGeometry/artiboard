#include <fstream>
#include <sstream>
#include <string>
#include "t3_icu.h"

const size_t column_count = 3;
const size_t row_count = 3;


T3IcuData::T3IcuData(const std::string& file_name)  {
	std::ifstream data(file_name);
	if (!data) throw runtime_error_ex("could not open file '%s'", file_name.c_str());
	std::string s;
	while (data) {
		data >> s;
		if (!data)
			return;
		for (auto &c : s )
			if (c == ',') c = ' ';
		std::stringstream line(s);
		arti::Board b;
		std::string v;
		for (auto i = 0U; i < column_count; i++)
			for (auto j = 0U; j < row_count; j++) {
				line >> v;
				b(i,j,arti::Piece(v[0]));
			}
		MatchOutcome r = MatchOutcome::Draw;
		line >> v;
		if (v == "positive")
			r = MatchOutcome::SouthPlayerWins;
		else
			r = MatchOutcome::Unknown;
		insert({b,r});
	}
}
