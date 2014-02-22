#include <experiment.h>
#include <negamax.h>
#include "connect4.h"
#include <log.h>

using namespace arti;

class NegamaxExploration : public Experiment {
public:
	NegamaxExploration() : Experiment("c4-100","Does negamax alpha-beta have an impact on the items search? ") {}
protected:

	void pickAB(int level, const char * fn_name, eval_function_t fn) {
		Board::u_ptr board(new Board());
		Connect4::spec.setup(*board);
		PositionThatOwns pos(0, std::move(board));
		Board::u_ptr_list boards;
		Connect4::spec.collectBoards(pos,boards);
		PickNegamaxAlphaBeta picker(&Connect4::spec,fn,level,false);
		picker.select(pos,boards);
		file() << fn_name << " " << level << " " << picker.walk_count() << " " << picker.value();
	}


	void do_run() override	{
		file() << "Function Depth Positions Value";
		for (int i=1;i<11;i++) 	{
			pickAB(i,"WinLose",Connect4::win_lose);
			pickAB(i,"IBEF",Connect4::StenMarkIBEF);
			pickAB(i,"ADATE",Connect4::StenMarkADATE);
			pickAB(i,"IBEFB",Connect4::StenMarkIBEFB);
			pickAB(i,"ADATEB",Connect4::StenMarkADATEB);
		}
	}
} nme;

class NegamaxOrdered : Experiment {
	public:
		NegamaxOrdered() : Experiment("c4-200", "Explore the effect of ordering nodes in negamax"){}
	protected:
		void pickAB(int level, const char * fn_name, eval_function_t fn, bool ordered) {
			Board::u_ptr board(new Board());
			Connect4::spec.setup(*board);
			PositionThatOwns pos(0, std::move(board));
			Board::u_ptr_list boards;
			Connect4::spec.collectBoards(pos,boards);
			PickNegamaxAlphaBeta picker(&Connect4::spec,fn,level,ordered);
			picker.select(pos,boards);
			file() << fn_name << (ordered?"-O ":" ") << level << " " << picker.walk_count() << " " << picker.value();
		}


		void do_run() override	{
			file() << "Function Depth Positions Value";
			for (int i=1;i<10;i++) 	{
				pickAB(i,"WinLose",Connect4::win_lose,false);
				pickAB(i,"IBEF",Connect4::StenMarkIBEF,false);
				pickAB(i,"ADATE",Connect4::StenMarkADATE,false);
				pickAB(i,"IBEFB",Connect4::StenMarkIBEFB,false);
				pickAB(i,"ADATEB",Connect4::StenMarkADATEB,false);
				pickAB(i,"IBEFS",Connect4::StenMarkIBEFS,false);
				pickAB(i,"WinLose",Connect4::win_lose,true);
				pickAB(i,"IBEF",Connect4::StenMarkIBEF,true);
				pickAB(i,"ADATE",Connect4::StenMarkADATE,true);
				pickAB(i,"IBEFB",Connect4::StenMarkIBEFB,true);
				pickAB(i,"IBEFS",Connect4::StenMarkIBEFS,true);
				pickAB(i,"ADATEB",Connect4::StenMarkADATEB,true);
			}
		  //pickLevel(2);
		}
} nmo;

