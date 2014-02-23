#include <experiment.h>
#include <negamax.h>
#include "connect4.h"
#include <log.h>

using namespace arti;

class NegamaxExploration : public Experiment {
public:
	NegamaxExploration() : Experiment("c4-100","Does negamax alpha-beta have an impact on the items search? ") {}
protected:

	void do_step(int level, const char * fn_name, eval_function_t fn) {
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
			do_step(i,"WinLose",Connect4::win_lose);
			do_step(i,"IBEF",Connect4::StenMarkIBEF);
			do_step(i,"ADATE",Connect4::StenMarkADATE);
			do_step(i,"IBEFB",Connect4::StenMarkIBEFB);
			do_step(i,"ADATEB",Connect4::StenMarkADATEB);
		}
	}
} nme;

class NegamaxOrdered : Experiment {
	public:
		NegamaxOrdered() : Experiment("c4-200", "Explore the effect of ordering nodes in negamax"){}
	protected:
		void do_step(int level, const char * fn_name, eval_function_t fn, bool ordered) {
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
				do_step(i,"WinLose",Connect4::win_lose,false);
				do_step(i,"IBEF",Connect4::StenMarkIBEF,false);
				do_step(i,"ADATE",Connect4::StenMarkADATE,false);
				do_step(i,"IBEFB",Connect4::StenMarkIBEFB,false);
				do_step(i,"ADATEB",Connect4::StenMarkADATEB,false);
				do_step(i,"WinLose",Connect4::win_lose,true);
				do_step(i,"IBEF",Connect4::StenMarkIBEF,true);
				do_step(i,"ADATE",Connect4::StenMarkADATE,true);
				do_step(i,"IBEFB",Connect4::StenMarkIBEFB,true);
				do_step(i,"ADATEB",Connect4::StenMarkADATEB,true);
			}
		  //pickLevel(2);
		}
} nmo;


class PerformanceMeasurements : Experiment {
	public:
		PerformanceMeasurements() : Experiment("c4-350","What affect does search depth have on performance? (3h+)") {}
		void do_run() override {
			file() << "Function Depth Performance";
			//do_step("WinLose",Connect4::win_lose);
			do_step("IBEF",Connect4::StenMarkIBEF);
			do_step("ADATE",Connect4::StenMarkADATE);
			do_step("IBEFB",Connect4::StenMarkIBEFB);
			do_step("ADATEB",Connect4::StenMarkADATEB);
		}
	private:
		void do_step(const string& fname, eval_function_t fn) {
			for (int p=1; p < 7;p++) {
				file() << fname << " " << p << " " << p_metric(fn,p);
				LOG << fname << " " << p;
			}
		}

		float p_metric(eval_function_t fn, const int ply) {
			static const int N = 15000;
			static PickRandom randpick(Connect4::spec);
			PickNegamaxAlphaBeta negapick(&Connect4::spec,fn,ply);
			PickDual dual(negapick,randpick);
			int wp = 0;
			int lp = 0;
			for (int i=0;i<N;i++) {
				Match match(Connect4::spec,dual);
				match.play();
				auto result = match.outcome();
				if (result == SouthPlayerWins)
					wp++;
				else if (result == NorthPlayerWins)
					lp++;
			}
			return 100 * (N+wp-lp)/(2.0f*N);
		}
} pmm;
