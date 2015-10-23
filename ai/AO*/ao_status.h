#ifndef __AO_STATUS_H__
#define __AO_STATUS_H__

#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <set>

using namespace std;
const double INF = numeric_limits<double>::max();

class CAndBase;
class CStatusBase {
public:
    // structure
    set<CAndBase*> belonging; // belong to which group
//    vector<CStatusBase*> children; // all childrens // removed beacause redundancy
    double f, g, h;
    bool solved, failed;

    // auxiliary
    vector<CAndBase*> groups;  //  and groups (or is considered as an individual group)
    CAndBase* bestPath; // ptr of the best group

    virtual void createGroups() {}; // create a group
    virtual double operator-(const CStatusBase &) {}; // dist, used to calc g(n)
    virtual double hFunction() {}; // h(n)
    virtual int hash() {}; // used in closed list
    friend ostream& operator<<(ostream &, const CStatusBase&); // used to output

    void updateF(); // update from groups
    void traverse(void (*f)(CStatusBase*)); // traverse nodes
    void addChild(CStatusBase*, CAndBase*); // existed group
    void backward();
    void backwardFailed();
    CAndBase* addGroup(); // new group
    
    CStatusBase() ;
    ~CStatusBase() ; 
};
class CAndBase {
public:
    vector<CStatusBase*> members;
    CStatusBase* parent;
    bool failed, solved;
    string operation;

    void updateStatus();

    CAndBase();
    ~CAndBase();
};

#endif
