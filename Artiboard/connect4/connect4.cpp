#include "connect4.h"
#include <log.h>
#include <vector>
const Connect4 Connect4::spec;
const Piece Connect4::south('o');
const Piece Connect4::north('x');
const Piece Connect4::open('-');
const char * annotations = "o12345678xabcdefgh-ijklmnop";
const int annotations_per_side = 9;
const char * south_annotations = annotations;
const char * north_annotations = south_annotations + annotations_per_side;
const char * open_annotations = north_annotations + annotations_per_side;

const index_t num_files = 7;
const index_t num_ranks = 6;
const Region all(num_files,num_ranks);
// diagonals - up 
const Region udr2(Square(0,2),1,1,4);
const Region udr1(Square(0,1),1,1,5);
const Region ud0(Square(0,0),1,1,6);
const Region udf1(Square(1,0),1,1,6);
const Region udf2(Square(2,0),1,1,5);
const Region udf3(Square(3,0),1,1,4);
// diagonals - down 
const Region ddf3(Square(3,5),1,-1,4);
const Region ddf2(Square(2,5),1,-1,5);
const Region ddf1(Square(1,5),1,-1,6);
const Region dd0(Square(0,5),1,-1,6);
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
	&udr2,&udr1,&ud0,&udf1,&udf2,&udf3,
	&ddf3,&ddf2,&ddf1,&dd0,&ddr4,&ddr3,
	&f0,&f1,&f2,&f3,&f4,&f5,&f6,
	&r0,&r1,&r2,&r5,&r3,&r4};

const std::vector<const Region*> ranks{&r0,&r1,&r2,&r3,&r4,&r5};
const std::vector<const Region*> files{&f0,&f1,&f2,&f3,&f4,&f5,&f6};

std::forward_list<Piece> * annotation_piece_list = 0;

const std::forward_list<Piece>& annotation_pieces() {
	if (annotation_piece_list == 0) {
		annotation_piece_list = new std::forward_list<Piece>();
		const char * e = annotations;
		while (*e != 0) 
			annotation_piece_list->emplace_front(Piece(*(e++)));
	};
	return *annotation_piece_list;

}

int ply_of(const Board& b) {
	return num_files*num_ranks - b.count(all,Connect4::open);
}

static const Piece& piece_for(const Side &side) {
	if (side == Side::South)
		return Connect4::south;
	else
		return Connect4::north;
};

static const Piece& piece_for_other(const Side &side) {
	if (side == Side::South)
		return Connect4::north;
	else
		return Connect4::south;
};

/** Start with an empty board */
void Connect4::setup(Board& b) const {
	b(all, Connect4::open);
}

/** The game is done when someone gets four in a row */
MatchOutcome Connect4::outcome_of(const Position& p) const {
	auto winningPiece = piece_for_other(p.ply().side_to_move());
	for (auto i = 0U; i < all_regions.size(); i++) {
		const Region& r = *all_regions[i];
		if (p.board().count_repeats(r,winningPiece) > 3) {
			if (winningPiece == north)
				return NorthPlayerWins;
			else
				return SouthPlayerWins;
		} 
	}
	if (p.board().count(r5,open) == 0)
			return Draw;
	else	
		return Unknown;
}

/** Next open square in every file is a possible move */
void Connect4::collectMoves(const Position& pos, Move::SharedFWList &result) const {
	if (outcome_of(pos) != Unknown)
		return; // there are no more moves to make
	auto piece = piece_for(pos.ply().side_to_move());
	for (auto i = 0U; i < files.size(); i++) {
		const Region& r = *files[i];
		auto it = pos.board().find(r,open);
		if (it != r.cend()) {
			std::shared_ptr<arti::Step> s(new StepToPlace(*it,piece));
			result.emplace_front(new Move(s));
		}
	}
}

arti::Piece annotate(const Board::const_iterator& it) {
	if (*it == Piece::EMPTY) return *it;
	else { 
		arti::Region n;
		n.insert_neighbours(it.pos()); 
		int r = it.board()->count(n,*it);
		if (*it == *south_annotations) return Piece(south_annotations[r]);
		else if(*it == *north_annotations) return Piece(north_annotations[r]);
		else if(*it == *open_annotations) return Piece(open_annotations[r]);
		else throw runtime_error_ex("Invalid char %d",*it);
	}
};

AnnotatedBoard::AnnotatedBoard(const Board& s) {
	for(auto i = s.begin(); i != s.end(); i++) {
		//TRACE << i.file() << " " << i.rank() << " " << annotate(i);
		place(i.file(),i.rank(),annotate(i));
	}
}

float win_lose_or(const Position& pos, eval_function_t fn) {
	switch (Connect4::spec.outcome_of(pos)) {
	case SouthPlayerWins: return 6*7*1000;
	case NorthPlayerWins: return -6*7*1000;
	case Draw: return 6*7*1000-1; // draw is nearly as good as a win
	default:
		return fn(pos);
	}
}


float weighted_south(const int w[], const Position& pos) {
	float result = 0.0f;
	float count = .0f;
	for (int r = 0; r < 7; r++)
		for (int c = 0; c < 8; c++) {
			auto p = pos.board().at(c,r);
			if (p != Connect4::open)
				count = count + 1.f;;
			if (p == Connect4::south)
				result += w[c*r];
		}
	return result/count;
}

float weighted_balanced(const int w[], const Position& pos) {
	float s = 0.0f;
	float n = 0.0f;
	float count = 0.0f;
	for (int r = 0; r < 7; r++)
		for (int c = 0; c < 8; c++) {
			const auto p = pos.board().at(c,r);
			if (p != Connect4::open)
				count = count + 1.f;;
			if (p == Connect4::south)
				s += w[c*r];
			else if (p == Connect4::north)
				n += w[c*r];
		}
	return (s - n)/count;
}

static const int stenmark_ibef[] = {
	3,4, 5, 7, 5,4,3,
	4,6, 8,10, 8,6,4,
	5,8,11,13,11,8,5,
	5,8,11,13,11,8,5,
	4,6, 8,10, 8,6,4,
	3,4, 5, 7, 5,4,3};

static const int stenmark_adate[] = {
	2, 0,2, 2, 2, 2,1,
	0, 2,6, 6, 2, 4,1,
	0,12,6,14,12,11,2,
	0, 1,4,16, 0, 0,2,
	0, 2,5, 0, 5, 4,4,
	0, 2,0, 1, 12,0,0};

float Connect4::win_lose(const Position& pos) {
	return win_lose_or(pos,
		[](const Position& pos){return 0.0f;});
}

float Connect4::StenMarkIBEF(const Position& pos) {
	return win_lose_or(pos,
		[](const Position& pos){
			return weighted_south(stenmark_ibef,pos);});
}

float Connect4::StenMarkADATE(const Position& pos) {
	return win_lose_or(pos,
		[](const Position& pos){
			return weighted_south(stenmark_adate,pos);});
}

float Connect4::StenMarkIBEFB(const Position& pos) {
	return win_lose_or(pos,
		[](const Position& pos){
			return weighted_balanced(stenmark_ibef,pos);});
}

float Connect4::StenMarkADATEB(const Position& pos) {
	return win_lose_or(pos,
		[](const Position& pos){
			return weighted_balanced(stenmark_adate,pos);});
}


