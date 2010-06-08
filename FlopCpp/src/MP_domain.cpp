// ******************** FlopCpp **********************************************
// File: MP_domain.cpp
//****************************************************************************

#include "MP_domain.hpp"
#include "MP_set.hpp"
#include "MP_boolean.hpp"
#include "MP_model.hpp"

namespace flopc {





    MP_domain_set::MP_domain_set(const MP_set* s, MP_index* i) 
        : S(s), I(i)  {} 
    MP_domain_set::~MP_domain_set() {}
    MP_domain MP_domain_set::getDomain(MP_set* s) const {
        return MP_domain(const_cast<MP_domain_set*>(this));
    }

    class Functor_conditional : public Functor {
    public:
        Functor_conditional(const Functor* f, const std::vector<MP_boolean> & condition)
            : F(f), Condition(condition) {}
        virtual ~Functor_conditional() {}
        void operator()() const {
            bool goOn = true;
            for (size_t i = 0; i<Condition.size(); i++) {
                if (Condition[i]->evaluate()==false) {
                    goOn = false;
                    break;
                }
            }
            if (goOn == true) {
                F->operator()();
            }
        }
        const Functor* F;
        std::vector<MP_boolean> Condition;
    };	
}

using namespace flopc;

const MP_domain& MP_domain::getEmpty() {
    static MP_domain Empty(new MP_domain_set(&MP_set::getEmpty(),&MP_set::getEmpty()));
    return Empty;
}


MP_domain_base::MP_domain_base() : count(0), donext(0) {}
MP_domain_base::~MP_domain_base() {}

Functor* MP_domain_base::makeInsertFunctor() const {
    return 0;
}

size_t MP_domain_base::size() const { 
    return count;
}


void MP_domain_base::display()const { 
    std::stringstream ss;
    ss<<"domain_base::display() size="<<size()<<std::ends;
    MP_model::getCurrentModel()->getMessenger()->logMessage(5,ss.str().c_str());
}

MP_domain::MP_domain() : Handle<MP_domain_base*>(0), last(0) {}
MP_domain::MP_domain(MP_domain_base* r) : Handle<MP_domain_base*>(r), last(r) {}
MP_domain::~MP_domain() {}

MP_domain MP_domain::such_that(const MP_boolean& b) {
    if (b.operator ->() != 0) {
        condition.push_back(b);
    }
    return *this;
}

void MP_domain::forall(const Functor& op) const {
    forall(&op);
}
void MP_domain::forall(const Functor* op) const {
    if (condition.size()>0) {
        last->donext = new Functor_conditional(op,condition);
    } else {
        last->donext = op;
    }
    //Iterate over all elements of the domain and call op on them.
    operator->()->operator()();
}

const MP_set_base* MP_domain_set::getSet() const {
    return S;
}

size_t MP_domain::size() const {
    return operator->()->getSet()->size();
}

int MP_domain_set::evaluate() const {
    return I->evaluate();
}

// Function call get's invoked by MP_domain::forall.
// donext Structure is created during Domain creation from Sets.
// If element of domain is reached, donext pointer points to Functor.
// This method iterates through all the elements of the domain and performs
// a function call at every element.
// Element of a domain is: Sets |S1| = 3, |S2| = 2. We have six elements.
void MP_domain_set::operator()() const {
    if (I->isInstantiated() == true) {
        (*donext)(); 
    } else {
        I->instantiate();
        for (int k=0; k<S->size(); k++) {
            I->assign(k);
            (*donext)();
        }
        I->assign(0);
        I->unInstantiate();
    }
}

MP_index* MP_domain_set::getIndex() const {
    return I;
}

//This operator creates a new domain from two already existing domains
//What happens is this
// 1: Check if the first or second MP_domain is empty. If that is the case, return the other one. If both are empty, an empty domain is returned.
// Question: When are domains empty? Usually "empty" domains with size 1 get created..
// 2: Do the linking stuff: Set donext of current last pointer to set b, Set last pointer to the latest added set b.
// Following this, we get the following b = d1 * d2 * d3 * d4 * d5. Operator is left-asoziativ, so in the end b's last pointer points to d5->root, but b's donext points to d2.operator->(),
// whose donext points to d3.operator(), up to till d5.operator(). The clue is: donext of d5 can be replaced via the last pointer from anyone getting b.
flopc::MP_domain flopc::operator*(const flopc::MP_domain& a, const flopc::MP_domain& b) {
    if (a.operator->() == MP_domain::getEmpty().operator->()) {
        return b;
    } else if (b.operator->() == MP_domain::getEmpty().operator->()) {
        return a;
    } else { //Both Domains are nonempty
        //Create new Domain by copying a
        MP_domain retval = a;
        // Set functor of what a's last pointer currently points to, to MP_domain_base of a to b's MP_domain_base
        retval.last->donext = b.operator->();
        //Memory Management: increment both counters
        const_cast<MP_domain&>(b).increment();
        const_cast<MP_domain&>(a).increment();
        //Set last pointer of new MP_domain to what b currently points to
        retval.last = b.last;
        //Subsets only: If there are any conditions on b, transfer them to new MP_domain
        retval.condition.insert(retval.condition.end(),b.condition.begin(),
            b.condition.end());
        return retval;
    }

}
