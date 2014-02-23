#pragma once
#include <forward_list>
#include <game.h>
using namespace arti;

int ply_of(const Board&b);
const std::forward_list<Piece>& annotation_pieces();

class AnnotatedBoard: public Board {
public:
	AnnotatedBoard(const Board& source);
};

class Connect4: public GameSpecification {
public:
	void setup(arti::Board&) const override;
	MatchOutcome outcome_of(const Position&) const override;
	void collectMoves(const Position& pos, Move::SharedFWList &result) const override;
	static const Piece north;
	static const Piece south;
	static const Piece open;
	static const Connect4 spec;
	static float win_lose(const Position& pos);
	static float StenMarkADATE(const Position& pos);
	static float StenMarkIBEF(const Position& pos);
	static float StenMarkIBEFB(const Position& pos);
	static float StenMarkADATEB(const Position& pos);
};

