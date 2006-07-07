#ifndef _MP_domain_hpp_
#define _MP_domain_hpp_

#include <iostream>
#include <vector>
#include <map>

using namespace std;

namespace flopc {
  const int outOfBound = -2;    

  class Functor {
  public:
    virtual void operator()() const {};
  protected:
    Functor() {}
    virtual ~Functor() {}
    Functor(const Functor&);
    Functor& operator=(const Functor&);
  };	


  template<int nbr, class T>
  vector<T> makeVector(T i1, T i2=0, T i3=0, T i4=0, T i5=0) {
    vector<T> S(nbr);
    S[0] = i1; 
    if (nbr==1) return S;
    S[1] = i2;
    if (nbr==2) return S;
    S[2] = i3;
    if (nbr==3) return S;
    S[3] = i4;
    if (nbr==4) return S;
    S[4] = i5; 
    return S;
  }

  template<class T> class Handle {
  public:
    T* operator->() const {
      return root;
    }
  protected:
    Handle(T* r) : root(r) {
      root->count++;
    }
    Handle(const Handle& h) : root(h.root) {
      root->count++;
    }
    const Handle& operator=(const Handle& h) {
      h.root->count++;
      if (--(root->count)==0) {
	delete root;
      }
      root = h.root;
      return *this;
    }
    ~Handle() {
      if (--(root->count) == 0) {
	delete root;
      }
    }
    T* root;
  };

  class MP_index;
  
  class MP_index_exp_base {
    friend class Handle<MP_index_exp_base>;
    friend class MP_index_exp_sum;
    friend class MP_set;
  public:
    MP_index_exp_base(const MP_index* i) : I(i), count(0) {}
    
  private:
    const MP_index *I;
    int count;
  };

  class MP_index_exp : public Handle<MP_index_exp_base> {
  public:
    MP_index_exp(MP_index_exp_base* r) : Handle<MP_index_exp_base>(r) {}

  };

  class MP_index_exp_index : MP_index_exp_base {
    friend class MP_index;
    MP_index_exp_index(const MP_index* i) : MP_index_exp_base(i) {}
    
  };
  
  class MP_index  {
  public:
    MP_index() : index(0), instantiated(false) { }
    virtual ~MP_index() {}

    operator MP_index_exp() {
      return new MP_index_exp_index(this);
    }

    int evaluate() const { 
      return index; 
    }
    bool isInstantiated() const { 
      return instantiated; 
    }
    void assign(int i)  const{ 
      index = i;
    }
    void unInstantiate() const {
      instantiated = false; 
    }
    void instantiate() const {
      instantiated = true; 
    }
    static const MP_index& Empty;
  private:
    MP_index(const MP_index&);
    MP_index& operator=(const MP_index&);
    mutable int index;
    mutable bool instantiated;
  };



  class MP_index_exp_sum : MP_index_exp_base {
    friend MP_index_exp operator+(const MP_index_exp& e, int d);
    MP_index_exp_sum(const MP_index_exp& e, int d) : MP_index_exp_base(e->I), E(e), D(d) {}
      
    MP_index_exp E;
    int D;
  };

  MP_index_exp operator+(const MP_index_exp& e, int d) {
    return new MP_index_exp_sum(e,d);
  }


  class MP_domain_base : public Functor {
    friend class MP_domain;
    friend class Handle<MP_domain_base>;
  public:
    MP_domain_base() : count(0) {}

    virtual void Forall(const Functor& op) const = 0;

  private:
    int count;
    //int size() const;
  };

  class MP_domain : public Handle<MP_domain_base> {
  public:
    MP_domain(MP_domain_base* r) : Handle<MP_domain_base>(r) { }

    void Forall(const Functor& op) const {
      root->Forall(op);
    }
    
    static const MP_domain& Empty;
    
    //    int size() const;
  };

  class DomainFunctor : public Functor {
  public:
    virtual void operator()() const {
      d.Forall(f);
    }
    DomainFunctor(const MP_domain& D, const Functor& F) : d(D), f(F) {}
    virtual ~DomainFunctor() {}
  private:
    MP_domain d;
    const Functor& f;
  };	

  class MP_set : public MP_index {
  public:
    MP_set(int i = 0): cardinality(i) {}

    MP_domain operator()(const MP_index& i) const;
    MP_domain operator()(const MP_index_exp& i) const;
	
    virtual int size() const {
      return cardinality;
    }

    int check(int i) const {
      if (i>=0 && i<cardinality) {
	return i;
      } else {
	return outOfBound;
      }
    }

    static const MP_set& Empty;
  private:
    int cardinality;
  };

  const MP_set& MP_set::Empty = *new MP_set(1);
  const MP_index& MP_index::Empty = *new MP_index();

  template <int nbr> class Domain_subset;
  

  template <int nbr>
  class MP_subset : public MP_set {
    friend  class Domain_subset<nbr>;

  public:
    MP_subset(const MP_set& s1, 
	      const MP_set& s2 = MP_set::Empty, 
	      const MP_set& s3 = MP_set::Empty, 
	      const MP_set& s4 = MP_set::Empty, 
	      const MP_set& s5 = MP_set::Empty) {
      S = makeVector<nbr,const MP_set*>(&s1,&s2,&s3,&s4,&s5);
    }

    MP_subset(vector<const MP_set*> s) : S(s) {}

    ~MP_subset() {}

    int operator()(int i1, int i2=0, int i3=0, int i4=0, int i5=0) {
      std::map<vector<int>, int>::const_iterator pos;
      pos = elements.find(makeVector<nbr>(i1, i2, i3, i4, i5));
      if (pos==elements.end()) {
	return outOfBound;
      } else {
	return pos->second;
      }
    }

    MP_domain operator()(const MP_index& i1, 
			 const MP_index& i2=MP_index::Empty,  
			 const MP_index& i3=MP_index::Empty,
			 const MP_index& i4=MP_index::Empty,
			 const MP_index& i5=MP_index::Empty);
 	
    int evaluate(const vector<MP_index*>& I) const {
      vector<int> vi;
      for (int k=0; k<nbr; k++) {
	int temp = I[k]->evaluate();
	vi.push_back(temp);
      }
      std::map<vector<int>, int>::const_iterator pos;
      pos = elements.find(vi);
      if (pos==elements.end()) {
	return outOfBound;
      } else {
	return pos->second;
      }
    }
    
    void insert(const vector<int> &args) {
      bool isOk = true;
      for (int i=0; i<nbr; i++) {
	if ( S[i]->check(args[i]) == outOfBound ) {
	  isOk = false;
	}
      }
      if (isOk == true) {
	std::map<vector<int>, int>::const_iterator pos;
	pos = elements.find(args);
	if (pos==elements.end()) {  // insert if not existent
	  const int v = elements.size();
	  elements[args] = v;
	}
      }
    }
    void insert(int i1, int i2=0, int i3=0, int i4=0, int i5=0) {
      insert(makeVector<nbr>(i1, i2, i3, i4, i5));
    }
    void display(const string& s = "") const {
      cout<<s<<endl;
      std::map<vector<int>, int>::const_iterator i;
      for (i = elements.begin(); i != elements.end(); i++) {
	for (int j=0; j<nbr; j++) {
	  cout<<(*i).first[j]<<"  ";
	}
	cout<<(*i).second<<endl;
      }
    }
    virtual int size() const {
      return elements.size();
    }

  private:
    vector<const MP_set*> S; 
    map<vector<int>, int> elements;
  };


   
  class Domain_set : public  MP_domain_base {
  public:
    Domain_set(const MP_set* s, const MP_index* i) : S(s), I(i) {}
	
    void Forall(const Functor& op) const {
      if (I->isInstantiated() == true) {
	(op)(); 
      } else {
	I->instantiate();
	for (int k=0; k<S->size(); k++) {
	  I->assign(k);
	  (op)();
	}
	I->assign(0);
	I->unInstantiate();
      }
    }
	
  private:	
    Domain_set(const Domain_set&);
    Domain_set& operator=(const Domain_set&);

    const MP_set *S;
    const MP_index *I;
  };
    
  template <int nbr>
  class Domain_subset : public  MP_domain_base {
  public:
    Domain_subset(const MP_subset<nbr>* s, vector<const MP_index*> i) : 
      S(s), I(i) {}
	
    void Forall(const Functor& op) const {
      bool isBound[nbr];
      bool allBound = true;
      for (int j=0; j<nbr; j++) {
	if (I[j]->isInstantiated() == true) {
	  isBound[j] = true;
	} else {
	  isBound[j] = false;
	  allBound = false;
	  if (I[j]!=0) {
	    I[j]->instantiate();
	  }
	}
      }
      if (allBound == true) {
	(op)(); 
      } else {
	std::map<vector<int>, int>::const_iterator i;
	int counter = 0;
	for (i = S->elements.begin(); i != S->elements.end(); i++) {
	  S->assign(counter);
	  counter++;
	  bool goOn = true;
	  for (int j=0; j<nbr; j++) {
	    if (isBound[j] == true) {
	      if (I[j]->evaluate() != i->first[j]) {
		goOn = false;
		break;
	      }
	    } else {
	      I[j]->assign(i->first[j]);
	    }
	  }
	  if (goOn == true) {
	    (op)();
	  }
	}
      }
      for (int j=0; j<nbr; j++) {
	if (isBound[j] == false) {
	  I[j]->assign(0);
	  I[j]->unInstantiate();
	}
      }
    }    
	

  private:	
    Domain_subset(const Domain_subset&);
    Domain_subset& operator=(const Domain_subset&);
    const MP_subset<nbr> *S;
    vector<const MP_index *> I;
  };


  MP_domain MP_set::operator()(const MP_index& i) const {
    return new Domain_set(this,&i);
  }
    
  MP_domain MP_set::operator()(const MP_index_exp& i) const {
    return new Domain_set(this,i->I);
  }
    
  template<int nbr>
  MP_domain MP_subset<nbr>::operator()(const MP_index& i1, 
				       const MP_index& i2,  
				       const MP_index& i3,
				       const MP_index& i4,
				       const MP_index& i5) {
    return new Domain_subset<nbr>(this,
				  makeVector<nbr>(&i1,&i2,&i3,&i4,&i5));
  }
    
  class Domain_composite : public  MP_domain_base {
  public:
    Domain_composite(const MP_domain& D1, const MP_domain& D2) : 
      d1(D1), d2(D2) {}

    void Forall(const Functor& op) const {
      d1.Forall(DomainFunctor(d2,op));
    }
  private:
    MP_domain d1,d2;
  };
    
  MP_domain operator*(const MP_domain& d1, const MP_domain& d2) {
    return new Domain_composite(d1,d2);
  }
}
#endif
