#pragma once
#include <board.h>
using namespace arti;
class Connect4: public GameSpecification {
	void setup(arti::Board&) const override;
	MatchOutcome outcome_of(const Position&) const override;
	void collectMoves(const Position& pos, Move::SharedFWList &result) const override;
};