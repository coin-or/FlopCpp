#include <iostream>
#include "New_domain.hpp"

using namespace flopc;

class myF : public Functor {
public:
    myF(MP_index& i, MP_index& j) : I(i), J(j) {}
    
    void operator()() const {
	std::cout<<I.evaluate()<<" and "<<J.evaluate()<<std::endl;
    }

    MP_index &I;
    MP_index &J;
};


int main() {
    cout<<"================="<<endl;

    MP_set A(3);
    MP_set B(4);
    MP_set C(5);

    cout<<"================="<<endl;
    MP_subset<2> L(A,B);
    cout<<"================="<<endl;

    L.insert(1,1);
    L.insert(1,2);
    L.insert(0,1);
    L.insert(2,3);


    MP_index I;
    MP_index J;
    MP_index K;

    
    myF myOp(I,J);
    
    (A(I)*B(J)).Forall(myOp);
    cout<<"================="<<endl;

    (A(I)*L(I,J)).Forall(myOp);

    cout<<"================="<<endl;
    (L(I,J)*C(K+1)).Forall(myOp);

   
    cout<<"================="<<endl;
    return 0;
}
