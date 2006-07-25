// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiCbcSolverInterface.hpp"

// NB NOT WORKING YET

enum { STOCKS, BONDS, numINSTR};

class Stage {
public:
    Stage(Stage* p, int numINSTR, double probability=0.5) : parent(p), INSTR(numINSTR), Buy(INSTR), 
							    pathprob(probability) {
	if (parent != 0) pathprob *= parent->pathprob;
    }
    
    MP_set INSTR;
    MP_variable Buy;
    MP_expression Final_wealth;

    Stage* parent;
    vector<Stage *> children;
    double pathprob;
};

class Stage_1 : public Stage {
public:
//    MP_set INSTR;
    MP_data initial_wealth;
    //    MP_variable Buy;
    MP_constraint InvestAll;

    Stage_1(int numINSTR, double w, MP_data& Good, MP_data& Bad, int n);
};

class Stage_i : public Stage {
public:
//    MP_set INSTR;
    // MP_data Return;
    // MP_variable Buy;
    MP_constraint ReinvestAll, Def_Final_wealth;
//    MP_expression Final_wealth;
    
    Stage_i(int numINSTR, MP_data& Return, int n, Stage* p, MP_data& Good, MP_data& Bad); 
};

class Stage_n : public Stage {
public:
//    MP_set INSTR;
//    MP_data Return;
    //MP_data shortage_penalty, overage_reward, goal;

    MP_variable Shortage, Overage;
    //  MP_expression Final_wealth;
    MP_constraint ReinvestAll, Def_Final_wealth;

    Stage_n(int numINSTR, MP_data& Return, double goal, Stage* p) : Stage(p,numINSTR)  {
	ReinvestAll() =  sum(INSTR, parent->Buy(INSTR) * Return(INSTR)) ==  goal - Shortage() + Overage();
	Final_wealth = pathprob*Overage() - pathprob*4*Shortage();
    }
};


Stage_1::Stage_1(int numINSTR, double w, MP_data& Good, MP_data& Bad, int n) : Stage(0, numINSTR, 1) {
    initial_wealth() = w;
    InvestAll() = sum(INSTR, Buy(INSTR)) == initial_wealth();
    
    children.push_back(new Stage_i(numINSTR, Good, n-1, this, Good, Bad));
    children.push_back(new Stage_i(numINSTR, Bad, n-1, this, Good, Bad));

    maximize(children[0]->Final_wealth + children[1]->Final_wealth);
}

Stage_i::Stage_i(int numINSTR, MP_data& Return, int n, Stage* p, MP_data& Good, MP_data& Bad) : Stage(p, numINSTR) {
    
    ReinvestAll() = sum(INSTR, parent->Buy(INSTR) * Return(INSTR)) == sum(INSTR, Buy(INSTR));
    
    if (n==2) {
	children.push_back(new Stage_n(numINSTR, Good, 80, this));
	children.push_back(new Stage_n(numINSTR, Bad, 80, this));
    } else {
	children.push_back(new Stage_i(numINSTR, Good, n-1, this, Good, Bad));
	children.push_back(new Stage_i(numINSTR, Bad, n-1, this, Good, Bad));
    }

    Final_wealth = children[0]->Final_wealth + children[1]->Final_wealth;
}

main() {
    const int numStages = 4;
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);
    MP_model::getDefaultModel().verbose();

    MP_set INSTR(numINSTR);
    MP_data GoodReturn(INSTR);
    MP_data BadReturn(INSTR);

    GoodReturn(STOCKS) = 1.25;
    GoodReturn(BONDS) = 1.14;
    BadReturn(STOCKS) = 1.06;
    BadReturn(BONDS) = 1.12;

    Stage_1 M(numINSTR, 55, GoodReturn, BadReturn, numStages);
}
