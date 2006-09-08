// $Id$
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <OsiCbcSolverInterface.hpp>

#include "flopc.hpp"
using namespace flopc;

int readFile(string fname, map<string, vector<double> >& d) {
    std::ifstream file(fname.c_str());
    string cline;

    while(getline(file,cline)) {
 	if (cline.compare(0,5,"begin")==0) {
	    string field = cline.substr(6,cline.find_first_of(')')-6);
	    double val;
	    while (file>>val) {
		d[field].push_back(val);
	    }
	    file.clear();
 	}
    }
    return 0;
}

void scheduleInsert(MP_subset<4>& sched, vector<double>& v, int c1, int c2) {
    for (int i = 0; i != v.size(); i++) {
	sched.insert(c1,c2,int(v[i])-1,int(v[i+1])-1);
	i++;
    }
}

void demandInsert(MP_data& demand, vector<double>& v, int c1, int c2, MP_subset<4>& sched) {
    for (int i = 0; i != v.size(); i++) {
	demand(sched(c1,c2,int(v[i])-1,int(v[i+1])-1)) = v[i+2];
	i++; i++;
    }
}

int main() {
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);
    MP_model::getDefaultModel().verbose();
    const int numTimeIntervals=48;
    enum {BO, NY, PH, WA, numCities};

    MP_set cities(numCities);           // Set of cities,
    MP_subset<2> links(cities, cities); // Set of intercity links
    MP_set times(numTimeIntervals);     // Set of time intervals in a day
    times.cyclic();

    links.insert(BO,NY);
    links.insert(NY,PH);
    links.insert(PH,WA);
    links.insert(NY,BO);
    links.insert(PH,NY);
    links.insert(WA,PH);

    links.display("LINKS");

    MP_subset<4> schedule(cities, cities, times, times); 
    // Member ((c1,c2),t1,t2) of this set represents a train that leaves 
    // city c1 at time t1 and arrives in city c2 at time t2

    map<string, vector<double> > d;
    readFile("train.dat",d);
    scheduleInsert(schedule,d["WAPH"],WA,PH);
    scheduleInsert(schedule,d["PHNY"],PH,NY);
    scheduleInsert(schedule,d["NYBO"],NY,BO);
    scheduleInsert(schedule,d["BONY"],BO,NY);
    scheduleInsert(schedule,d["NYPH"],NY,PH);
    scheduleInsert(schedule,d["PHWA"],PH,WA);
   
    schedule.display("SCHEDULE");

    MP_data demand(schedule); 
    // Smallest number of cars that can meet demand for each scheduled train    
    demandInsert(demand,d["demWAPH"],WA,PH,schedule);
    demandInsert(demand,d["demPHNY"],PH,NY,schedule);
    demandInsert(demand,d["demNYBO"],NY,BO,schedule);
    demandInsert(demand,d["demBONY"],BO,NY,schedule);
    demandInsert(demand,d["demNYPH"],NY,PH,schedule);
    demandInsert(demand,d["demPHWA"],PH,WA,schedule);

    demand.display("DEMAND");

    // Maximum number of cars in one section of a train
    double section = 14;

    MP_data low(schedule);  // Minimum number of cars needed to meet demand
    MP_data high(schedule); // Maximum number of cars allowed on a train:
    // 2 if demand is for less than one car; otherwise, lesser of
    // number of cars needed to hold twice the demand, and
    // number of cars in minimum number of sections needed

    MP_data dist_table(links);
    MP_data distance(links);

    dist_table(links(BO,NY)) = 232;
    dist_table(links(NY,PH)) =  90;
    dist_table(links(PH,WA)) = 135;

    MP_index c1,c2,t1,t2;

    low(schedule) = ceil(demand(schedule));
    high(schedule) = maximum (2, minimum (ceil(2*demand(schedule)), 
				  section*ceil(demand(schedule)/section) ));
    low.display("LOW");
    high.display("HIGH");

    distance(links).such_that(dist_table(links)>0) = dist_table(links);
    distance(links(c1,c2)).such_that(dist_table(links(c1,c2))==0.0) = dist_table(links(c2,c1));

    distance.display("distance");

    MP_variable U(cities,times); 
    // u[c,t] is the number of unused cars stored at city c in the interval beginning at time t

    MP_variable X(schedule);    // x[c1,t1,c2,t2] is the number of cars assigned to
    // the scheduled train that leaves c1 at t1 and arrives in c2 at t2

    MP_index c,t;

    MP_expression cars =  
	sum( cities(c), U(c,times.size()-1)) + 
	sum( schedule(c1,c2,t1,t2).such_that(t2 < t1), X(schedule) );

    // Number of cars in the system:
    // sum of unused cars and cars in trains during the last time interval of the day

    MP_expression miles = sum(schedule(c1,c2,t1,t2), distance(links(c1,c2)) * X(schedule));
    // Total car-miles run by all scheduled trains in a day

    MP_constraint account(cities,times);

    account(c,t) =  
	U(c,t) == U(c,t-1) + 
	   sum( schedule(c1,c,t1,t), X(schedule(c1,c,t1,t)) ) - 
	   sum( schedule(c,c2,t,t2), X(schedule(c,c2,t,t2)) );
    
    // For every city and time: unused cars in the present interval must equal
    // unused cars in the previous interval, plus cars just arriving in trains, 
    // minus cars just leaving in trains

    X.lowerLimit(schedule) = low(schedule);  // number of cars must meet demand,
    X.upperLimit(schedule) = high(schedule); // but must not be so great that unnecessary sections are run

    minimize(cars);

    assert(MP_model::getDefaultModel()->getNumRows()==192);
    assert(MP_model::getDefaultModel()->getNumCols()==411);
    assert(MP_model::getDefaultModel()->getNumElements()==822);
    assert(MP_model::getDefaultModel()->getObjValue()==1143);

    //cout<<cars.level()<<"  -----  "<<miles.level()<<endl;

    minimize(miles);

    assert(MP_model::getDefaultModel()->getNumRows()==192);
    assert(MP_model::getDefaultModel()->getNumCols()==411);
    assert(MP_model::getDefaultModel()->getNumElements()==822);
    assert(MP_model::getDefaultModel()->getObjValue()==163882);
    //cout<<cars.level()<<"  -----  "<<miles.level()<<endl;

    cout<<"Test train passed."<<endl;
}
