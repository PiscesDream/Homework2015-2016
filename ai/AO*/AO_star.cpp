#include <iostream>
#include "ao_status.h"
#include <set>
#include <algorithm>
#include <cstdio>
// #define DEBUG

using namespace std;

int N = 12;
int MAXDEPTH = 3;

// A Class inherted
class CStatus;
ostream& operator << (ostream & cout, const CStatus& c);
class CStatus: public CStatusBase {
public:
    int lhs, ls, hs, s, t;
    int depth;

    CStatus() { depth = 0; }
    CStatus(int _lhs, int _ls, int _hs, int _s, int _t):
    lhs(_lhs), ls(_ls), hs(_hs), s(_s), t(_t) { depth = 0; }
    ~CStatus() {}

    void createGroups() { } 
    bool createGroups(set<CStatus*> & openSet) { 
        if (t <= 0) return false;
        int lhs1, ls1, hs1, s1,
            lhs2, ls2, hs2, s2;
        for (lhs1=0; lhs1 <= lhs/2; lhs1++)
            for (ls1=0; ls1 <= ls; ls1++)
                for (hs1=0; hs1<= hs; hs1++)
                    if (lhs1+ls1+lhs1+s >= N/4)
                        for (lhs2=lhs1; lhs2 <= lhs-lhs1; ++lhs2)
                            for (ls2=max(lhs1+ls1-lhs2,0); ls2 <= ls-ls1; ls2++)
                                for (hs2=max(lhs1+ls1+hs1-lhs2-ls2,0); hs2 <= hs-hs1; hs2++) {
                                    int left = lhs1+ls1+hs1;
                                    int right = lhs2+ls2+hs2;
                                    if (right-left>s || right+left == 0) continue;
                                    s2 = 0;
                                    s1 = right-left;
                                    CStatus *p1 = new CStatus(0, ls2+lhs2, lhs1+hs1, s+ls-ls2+hs-hs1+lhs-(lhs1+lhs2), t-1);
                                    CStatus *p2 = new CStatus(lhs-lhs1-lhs2, ls-ls1-ls2, hs-hs1-hs2, s+ls1+ls2+hs1+hs2+lhs1+lhs2, t-1);
                                    CStatus *p3 = new CStatus(0, ls1+lhs1, lhs2+hs2, s+ls-ls1+hs-hs2+lhs-(lhs1+lhs2), t-1);

                                    CAndBase *g = this->addGroup();
                                    char buffer[100]; 
                                    sprintf(buffer, "Pick[(%d, %d, %d, %d), (%d, %d, %d, %d)]", lhs1, ls1, hs1, s1, lhs2, ls2, hs2, s2);
                                    g->operation = string(buffer);
                                    this->addChild(p1, g);
                                    this->addChild(p2, g);
                                    this->addChild(p3, g);

                                    #ifdef DEBUG
                                    cout << "\t" << buffer << endl;
                                    cout << "\t\tp1: "<< *p1 << endl;
                                    cout << "\t\tp2: "<< *p2 << endl;
                                    cout << "\t\tp3: "<< *p3 << endl;
                                    #endif
                                }

        return groups.size()!=0;
    }

    double operator-(const CStatusBase &) { return 1.0; }
    double hFunction() {return solved?0:lhs*2+ls+hs-1;}
    int hash() {return 0;}

    bool operator<(const CStatus& b) const {
        return lhs < b.lhs || 
              (lhs == b.lhs && ls < b.ls) ||
              (lhs == b.lhs && ls == b.ls && hs < b.hs) ||
              (lhs == b.lhs && ls == b.ls && hs == b.hs && s < b.s) ||
              (lhs == b.lhs && ls == b.ls && hs == b.hs && s == b.s && t < b.t);
    }
    bool operator==(const CStatus& b) const {
        return lhs == b.lhs && ls == b.ls && hs == b.hs && s == b.s && t == b.t;
    }
};

ostream& operator << (ostream & cout, CStatusBase * c) {
    cout << *(CStatus*)c << endl;
    return cout;
}

ostream& operator << (ostream & cout, const CStatus& c) {
    cout << "<CStatus: (" << c.lhs << ", " << c.ls << ", "<<c.hs<<", " << c.s << ", "<<c.t <<")>";
    return cout;
}

ostream& operator << (ostream & cout, const CAndBase& g) {
        cout << "groups: [" << endl;
    for (auto & m: g.members) {
        cout << "           " << (*(CStatus*)m) << "," << endl;
    }
        cout << "        ]" << endl;
    return cout;
}

template <typename T>
void print_solution(T *s, int indent) {
    for (int i=1; i <= indent; ++i) cout << "    ";
    cout << *s;
    if (s->bestPath) {
        cout << " ^ " << s->bestPath->operation << endl;
        for (auto & m: s->bestPath->members)
            print_solution((T*)m, indent+1);
    }
    else
        cout << endl;
    return;
}

template <typename T>
void AO_star(set<T> & startSet, set<T> & endSet) {
    int searching_steps = 0;

    vector<T*> startList;
    set<T*> openSet;
    set<T*> closedSet;
    set<T> failedSet;

    for (auto & s: startSet) {
        T *p = new T(s);
        openSet.insert(p);
        startList.push_back(p);
    }

    while (!openSet.empty()) {
        searching_steps++;
        for (auto & s: startList) // stop when solved
            if (s->solved) {
                print_solution<T>(s, 0);
                cout << "Searched for " << searching_steps << " steps." << endl;
                return;
            }

        // find status with least f
        T* leastFStatus = *openSet.begin();
        for (auto & s: openSet) 
            if (s->f < leastFStatus->f)
                leastFStatus = s;
#ifdef DEBUG
        cout << "Search: " << (*leastFStatus) << endl;
#endif 
        openSet.erase(openSet.find(leastFStatus));
        closedSet.insert(leastFStatus);

        if (leastFStatus->solved) {
            leastFStatus->backward();
            continue;
        }

        if (!leastFStatus->createGroups(openSet)) {
            leastFStatus->failed = true;
            leastFStatus->backwardFailed();
            continue;
        }

        for (auto & g: leastFStatus->groups) {
            for (auto & m: g->members) {
                T *p = (T*)m;

                for (auto & c: openSet)
                    if ((*p) == (*c)) continue; //m = (CStatusBase*)c;
                for (auto & c: closedSet)
                    if ((*p) == (*c)) continue; //m = (CStatusBase*)c;

                if (endSet.find(*p) != endSet.end()) {
                    p->solved = true;
                    closedSet.insert(p);
                }
                else {
                    if (p->t == 0) { // cannot search anymore, failed
                        p->failed = true;
                        closedSet.insert(p);
                        //failedSet.insert(*p);
                    }
                    else 
                        openSet.insert(p);
                }
            }
            g->updateStatus();
        }
        leastFStatus->backward();
        leastFStatus->backwardFailed();
    }
    cout << "Fail to find a solution." << endl;
}

int getNumber(char *s) {
    int answer = 0;
    for (char *p = s; *p; ++p)
        answer = answer*10 + *p-'0';
    return answer;
}

int main(int argc, char **argv) {
    if (argc >= 3) {
        N = getNumber(argv[1]);
        MAXDEPTH = getNumber(argv[2]);
    }
    set<CStatus> startSet;
    set<CStatus> endSet;

    startSet.insert(CStatus(N, 0, 0, 0, MAXDEPTH));

    for (int i = 0; i <= MAXDEPTH-1; ++i) {
        endSet.insert(CStatus(0, 1, 0, N-1, i));
        endSet.insert(CStatus(0, 0, 1, N-1, i));
        endSet.insert(CStatus(0, 0, 0, N, i)); // the impossible status
    }
    AO_star<CStatus>(startSet, endSet);
}
