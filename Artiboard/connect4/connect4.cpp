#include "connect4.h"
#include <log.h>
#include <vector>

const Piece south('o');
const Piece north('x');
const Piece open('-');
const index_t num_files = 7;
const index_t num_ranks = 6;
const Region all(num_files,num_ranks);
// diagonals
const Region udl3(Square(0,3),1,1,4);
const Region udl1(Square(0,1),1,1,6);
const Region udl2(Square(0,2),1,1,5);
const Region udb0(Square(0,0),1,1,7);
const Region udb3(Square(3,0),1,1,4);
const Region udb2(Square(2,0),1,1,5);
const Region udb1(Square(1,0),1,1,6);

const Region ddf2(Square(2,6),1,-1,4);
const Region ddf1(Square(1,6),1,-1,5);
const Region ddf0(Square(0,6),1,-1,6);
const Region ddr5(Square(0,5),1,-1,6);
const Region ddr4(Square(0,4),1,-1,5);
const Region ddr3(Square(0,3),1,-1,4);
// files 
const Region f0(Square(0,0),0,1,num_ranks);
const Region f1(Square(1,0),0,1,num_ranks);
const Region f2(Square(2,0),0,1,num_ranks);
const Region f3(Square(3,0),0,1,num_ranks);
const Region f4(Square(4,0),0,1,num_ranks);
const Region f5(Square(5,0),0,1,num_ranks);
const Region f6(Square(6,0),0,1,num_ranks);
// ranks
const Region r0(Square(0,0),1,0,num_files);
const Region r1(Square(0,1),1,0,num_files);
const Region r2(Square(0,2),1,0,num_files);
const Region r3(Square(0,3),1,0,num_files);
const Region r4(Square(0,4),1,0,num_files);
const Region r5(Square(0,5),1,0,num_files);

const std::vector<const Region*> all_regions{
	&udl3,&udl1,&udl2,&udb0,&udb3,&udb2,&udb1,
	&ddf2,&ddf1,&ddf0,&ddr5,&ddr4,&ddr3,
	&r0,&r1,&r2,&r3,&r4,&r5,
	&f0,&f1,&f2,&f3,&f4,&f5,&f6
};

const std::vector<const Region*> ranks{&r0,&r1,&r2,&r3,&r4,&r5};
const std::vector<const Region*> files{&f0,&f1,&f2,&f3,&f4,&f5,&f6};


static const Piece& piece_for(const Side &side) {
	if (side == Side::South)
		return south;
	else
		return north;
};

static const Piece& piece_for_other(const Side &side) {
	if (side == Side::South)
		return north;
	else
		return south;
};

/** Start with an empty board */
void Connect4::setup(Board& b) const {
	b(all, open);
}

/** The game is done when someone gets four in a row */
MatchOutcome Connect4::outcome_of(const Position& p) const {
	if (p.board().count(r5,open) == 0)
			return Draw;
	auto winningPiece = piece_for_other(p.ply().side_to_move());
	for (auto i = 0; i < all_regions.size(); i++) {
		const Region& r = *all_regions[i];
		if (p.board().count_repeats(r,winningPiece) > 3) {
			if (winningPiece == north)
				return NorthPlayerWins;
			else
				return SouthPlayerWins;
		} 
	}
	return Unknown;
}

/** Next open square in every file is a posible move */
void Connect4::collectMoves(const Position& pos, Move::SharedFWList &result) const {
	auto piece = piece_for(pos.ply().side_to_move());
	for (auto i = 0; i < files.size(); i++) {
		const Region& r = *files[i];
		auto it = pos.board().find(r,open);
		if (it != r.cend()) {
			std::shared_ptr<arti::Step> s(new StepToPlace(*it,piece));
			result.emplace_front(new Move(s));
		}
	}
}
