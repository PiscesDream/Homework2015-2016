// Missionaries and Cannibals

#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <limits>
#include <stack>
#include <cstdio>
#include <ctime>

using namespace std;

const double INF = numeric_limits<double>::max();

// number of missionaries and cannibals
int N = 3;
// max number for boat
int K = 2;
double alpha = 1.0;

// Translate rule
class Transfer_t {
public:
    array<int, 3> t;
    // X = (a, b, c)
    // T(X) = (X.a+T.a, X.b+T.b, X.c+T.c)
    Transfer_t(int a, int b, int c) {
        t[0] = a;
        t[1] = b;
        t[2] = c;
    }
};
ostream& operator<<(ostream& cout, Transfer_t & t) {
    cout << "<Transfer: (" << t.t[0] << ", " << t.t[1] << ", " << t.t[2] << ")>"; 
    return cout;
}

// status: X = (#missionaries, #cannibals, where is boat)
class Status_t {
public:
    array<int, 3> s;
    double f, g, h;
    Status_t(int a, int b, int c) {
        s[0] = a;
        s[1] = b;
        s[2] = c;
        f = g = h = INF;
    }
    Status_t() {
        f = g = h = INF;
    }
    // hash
    int todigit() const { return s[0]*100+s[1]*10+s[2]; }
    // used in set
    bool operator< (const Status_t & b) const {
        // return this->f < b.f;
        return this->todigit() < b.todigit(); 
    }

    
    // whether T(this) is valid
    bool check_transfer(const Transfer_t & t) {
        if (t.t[0]+s[0] > N || t.t[0]+s[0] < 0 ||
            t.t[1]+s[1] > N || t.t[1]+s[1] < 0 ||
            t.t[2]+s[2] > 1 || t.t[2]+s[2] < 0 ||
            ((t.t[0]+s[0]<t.t[1]+s[1])&&(t.t[0]+s[0]!=0)) || 
            ((N-t.t[0]-s[0]<N-t.t[1]-s[1])&&(N-t.t[0]-s[0]!=0)) )
            return false;
        else
            return true;
    }
    // return T(this)
    Status_t apply_transfer(const Transfer_t & t) {
        Status_t next = *this;
        for (int i = 0; i < 3; ++i) 
            next.s[i] += t.t[i];
        return next;
    }

    bool operator == (const Status_t &b) const {
        return this->s == b.s;
    }
};
ostream& operator<<(ostream& cout, Status_t& s) {
    cout << "<Status: (" << s.s[0] << ", " << s.s[1] << ", " << s.s[2] << ")>"; 
    return cout;
}


vector<Transfer_t> tf; // transfer functions
// create transfer functions
void init() {
    for (int i = 1; i <= min(K, N); ++i) {
//    for (int i = min(K, N); i >= 1; --i) {
        for (int j = 0; j <= min(max(K-i, 0), i); ++j) { 
            tf.push_back(Transfer_t(i, j, 1));
            tf.push_back(Transfer_t(-i, -j, -1));
        }
        tf.push_back(Transfer_t(0, i, 1));
        tf.push_back(Transfer_t(0, -i, -1));
    }

//  cout << "=====Transfer functions=====" << endl;
//  for (int i = 0; i < tf.size(); ++i) 
//      cout << tf[i] << endl;
//  cout << "============================" << endl;
}

// h(x) heuristic function
double h(Status_t & a, Status_t & b) {
//    return 0;
    return a.s[0]*0.9;
//    return a.s[0]*2+a.s[1];
}
// g(a)-g(b) distance 
double dist(Status_t & a, Status_t & b) {
    return 1.0;
}

// print solution recrusively
void print_solution(map<Status_t, Status_t> & cameFrom, Status_t & end, Status_t & start) {
    Status_t x = end;
//#define DETAIL
#ifdef DETAIL
    stack<Status_t> s;
#endif
    int count = 0;
    while (!(x == start)) {
#ifdef DETAIL
        s.push(x);
#endif
        x = cameFrom[x];
        count ++;
    }
#ifdef DETAIL
       cout << "This side ------- That side" << endl;
    while (!s.empty()) {
        x = s.top();s.pop();
        printf(" (%d, %d)   -------  (%d, %d)\n", x.s[0], x.s[1], N-x.s[0], N-x.s[1]); 
    }
#endif
    cout << "Use: " << count << " steps" << endl;
}
// only one start and one end
void A_star(Status_t start, Status_t end) {
    int searching_steps = 0;
    set<Status_t> closedSet;
    set<Status_t> openSet;
    map<Status_t, Status_t> cameFrom;

    start.g = 0;
    start.f = start.g + h(start, end);
    openSet.insert(start);

    while (!openSet.empty()) {
        Status_t cur = *openSet.begin();
        searching_steps++;

        // finding status with the least f
        for (auto & s: openSet)
            if (s.f < cur.f)
                cur = s;

        // found solution
        if (cur == end) {
            print_solution(cameFrom, end, start);
            cout << "Searched for " << searching_steps << " steps." << endl;
            return;
        }

        // remove from openSet and add into closeSet
        openSet.erase(openSet.find(cur));
        closedSet.insert(cur);

        // transfer cur
        for (auto &t: tf) { 
            if (cur.check_transfer(t)) { // T(cur) is valid
                Status_t neighbour = cur.apply_transfer(t);

                if (closedSet.find(neighbour) != closedSet.end()) continue;

                double tentative_g = cur.g + dist(cur, neighbour); // g(neighbour)

                if (openSet.find(neighbour) != openSet.end()) { // if neighbour is in openset 
                    if (tentative_g >= openSet.find(neighbour)->g) // if g(neighbour) is worse than the existed one 
                        continue;
                    else
                        openSet.erase(openSet.find(neighbour)); // if current g(neighbour) is better
                }

                cameFrom[neighbour] = cur;
                neighbour.g = tentative_g; // update g
                neighbour.f = neighbour.g + h(neighbour, end); // update f
                openSet.insert(neighbour); // insert openset
            }
        }
    }
    cout << "Failure" << endl;
}


int getNumber(char *s) {
    int answer = 0;
    for (char *p = s; *p; ++p)
        answer = answer*10 + *p-'0';
    return answer;
}

int main(int argc, char **argv) {
    clock_t start_time = clock();
    if (argc >= 3) {
        N = getNumber(argv[1]);
        K = getNumber(argv[2]);
    }

    init();

    Status_t start(N, N, 1), end(0, 0, 0);
    A_star(start, end);
    double time = double(clock() - start_time)/CLOCKS_PER_SEC;
    cout << "Used: " << time << 's' << endl; 
}
