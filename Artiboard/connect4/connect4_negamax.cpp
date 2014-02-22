#include <experiment.h>
#include <negamax.h>
#include "connect4.h"
#include <log.h>

using namespace arti;

static Connect4 spec;
float WinLoseEval(const Position& pos) {
	switch (spec.outcome_of(pos)) {
	case SouthPlayerWins: return 6*7*1000;
	case NorthPlayerWins: return -6*7*1000;
	default:
		return 0.0f;
	}
}

float weighted_south(const int w[], const Position& pos) {
	float result = 0.0f;
	for (int r = 0; r < 7; r++)
		for (int c = 0; c < 8; c++) {
			if (pos.board().at(c,r) == Connect4::south)
				result += w[c*r];
		}
	return result;		
}

float weighted_balanced(const int w[], const Position& pos) {
	float s = 1.0f;
	float n = 1.0f;
	for (int r = 0; r < 7; r++)
		for (int c = 0; c < 8; c++) {
			const auto p = pos.board().at(c,r);
			if (p == Connect4::south)
				s += w[c*r];
			else if (p == Connect4::north)
				n += w[c*r];
		}
	return s - n;		
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


float StenMarkIBEF(const Position& pos) {
	auto result = WinLoseEval(pos);
	if (result == 0.0) {
		result = weighted_south(stenmark_ibef,pos);
	}
	//TRACE << pos.board() << "SCORE:" << result;
	return result;
}

float StenMarkIBEFS(const Position& pos) {
	return weighted_south(stenmark_ibef,pos);
}


float StenMarkADATE(const Position& pos) {
	auto result = WinLoseEval(pos);
	if (result == 0.0) {
		result = weighted_south(stenmark_adate,pos);
	}
	//TRACE << pos.board() << "SCORE:" << result;
	return result;
}

float StenMarkIBEFB(const Position& pos) {
	auto result = WinLoseEval(pos);
	if (result == 0.0) {
		result = weighted_balanced(stenmark_ibef,pos);
	}
	//TRACE << pos.board() << "SCORE:" << result;
	return result;
}

float StenMarkADATEB(const Position& pos) {
	auto result = WinLoseEval(pos);
	if (result == 0.0) {
		result = weighted_balanced(stenmark_adate,pos);
	}
	//TRACE << pos.board() << "SCORE:" << result;
	return result;
}


class NegamaxExploration : public Experiment {
public:
	NegamaxExploration() : Experiment("c4-100","Does negamax alpha-beta have an impact on the items search? ") {}
protected:

	void pickAB(int level, const char * fn_name, eval_function_t fn) {
		Board::u_ptr board(new Board());
		spec.setup(*board);
		PositionThatOwns pos(0, std::move(board));
		Board::u_ptr_list boards;
		spec.collectBoards(pos,boards);
		PickNegamaxAlphaBeta picker(&spec,fn,level,false);
		picker.select(pos,boards);
		file() << fn_name << " " << level << " " << picker.walk_count() << " " << picker.value();
	}


	void do_run() override	{
		file() << "Function Depth Positions Value";
		for (int i=1;i<11;i++) 	{
			pickAB(i,"WinLose",WinLoseEval);
			pickAB(i,"IBEF",StenMarkIBEF);
			pickAB(i,"ADATE",StenMarkADATE);
			pickAB(i,"IBEFB",StenMarkIBEFB);
			pickAB(i,"ADATEB",StenMarkADATEB);
		}
	}
} nme;

class NegamaxOrdered : Experiment {
	public:
		NegamaxOrdered() : Experiment("c4-200", "Explore the effect of ordering nodes in negamax"){}
	protected:
		void pickAB(int level, const char * fn_name, eval_function_t fn, bool ordered) {
			Board::u_ptr board(new Board());
			spec.setup(*board);
			PositionThatOwns pos(0, std::move(board));
			Board::u_ptr_list boards;
			spec.collectBoards(pos,boards);
			PickNegamaxAlphaBeta picker(&spec,fn,level,ordered);
			picker.select(pos,boards);
			file() << fn_name << (ordered?"-O ":" ") << level << " " << picker.walk_count() << " " << picker.value();
		}


		void do_run() override	{
			file() << "Function Depth Positions Value";
			for (int i=1;i<10;i++) 	{
				pickAB(i,"WinLose",WinLoseEval,false);
				pickAB(i,"IBEF",StenMarkIBEF,false);
				pickAB(i,"ADATE",StenMarkADATE,false);
				pickAB(i,"IBEFB",StenMarkIBEFB,false);
				pickAB(i,"ADATEB",StenMarkADATEB,false);
				pickAB(i,"IBEFS",StenMarkIBEFS,false);
				pickAB(i,"WinLose",WinLoseEval,true);
				pickAB(i,"IBEF",StenMarkIBEF,true);
				pickAB(i,"ADATE",StenMarkADATE,true);
				pickAB(i,"IBEFB",StenMarkIBEFB,true);
				pickAB(i,"IBEFS",StenMarkIBEFS,true);
				pickAB(i,"ADATEB",StenMarkADATEB,true);
			}
		  //pickLevel(2);
		}
} nmo;

