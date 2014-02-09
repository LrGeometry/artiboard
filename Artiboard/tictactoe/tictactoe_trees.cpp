#include <experiment.h>
#include <negamax.h>
#include "tictactoe.h"
#include <log.h>

using namespace arti;

static TicTacToeSpecification spec;
float WinLoseEval(const Position& pos) {
	switch (spec.outcome_of(pos)) {
	case SouthPlayerWins: return 1.0f;
	case NorthPlayerWins: return -1.0f;
	default:
		return 0.0f;
	}
}

float SmartEval(const Position& pos) {
	auto result = WinLoseEval(pos);
	if (result == 0.0) {
		auto goodpiece = TicTacToeSpecification::tictacCircle;
		auto badpiece = TicTacToeSpecification::tictacCross;
		if (pos.board().at(1,1) == goodpiece)
			result = 0.5;
		else if (pos.board().at(1,1) == badpiece)
			result = -0.5;
	}
	//TRACE << pos.board() << "SCORE:" << result;
	return result;
}

class NegamaxExploration : public Experiment {
public:
	NegamaxExploration() : Experiment("t3-100","Does negamax alpha-beta have an impact on the items search? YES up to 50 times! ") {}
protected:

	void pickNormal(int level) {
		Board::u_ptr board(new Board());
		spec.setup(*board);
		Position pos(0, std::move(board));
		Board::u_ptr_list boards;
		spec.collectBoards(pos,boards);
		PickNegamax picker(&spec,SmartEval,level);
		picker.select(pos,boards);
		file() << "negamax " << level << " " << picker.walk_count() << " " << picker.value();
	}

	void pickAB(int level) {
		Board::u_ptr board(new Board());
		spec.setup(*board);
		Position pos(0, std::move(board));
		Board::u_ptr_list boards;
		spec.collectBoards(pos,boards);
		PickNegamaxAlphaBeta picker(&spec,SmartEval,level);
		picker.select(pos,boards);
		file() << "negamax-ab " << level << " " << picker.walk_count() << " " << picker.value();
	}


	void doRun() override	{
		file() << "Method Depth Positions Value";
		for (int i=1;i<10;i++) 	{
			pickNormal(i);
			pickAB(i);
		}
	  //pickLevel(2);
	}
};

static NegamaxExploration minimax;
