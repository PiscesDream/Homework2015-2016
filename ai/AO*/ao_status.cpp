#include "ao_status.h"

void CStatusBase::traverse( void (*f)(CStatusBase*) ) {
//  for (auto & c: children) { 
//      c->traverse(f);
//      f(c);
//  }
}

void CStatusBase::updateF() { 
    f = INF;
    for (auto & g: groups) {
        if (g->failed) continue;
        double temp_f = 0;
        for (auto & m: g->members) {
            m->h = m->hFunction();
            temp_f += (*this)-(*m) + // dist(parent, m)
                m->h;
        }
        if (temp_f < f) {
            f = temp_f;
            bestPath = g;
        }
    }
}

void CStatusBase::addChild(CStatusBase* child, CAndBase* g) { 
    child->belonging.insert(g);
    g->members.push_back(child);
//    children.push_back(child);
}

CAndBase* CStatusBase::addGroup() { // new group
    CAndBase * newg = new CAndBase;
    newg->parent = this;
    groups.push_back(newg);
    return newg;
}

CStatusBase::CStatusBase() {
        f = g = h = 0.0;
        solved = failed = false;
    };

CStatusBase::~CStatusBase() {
    for (auto & g: groups) delete g;
//    for (auto & c: children) delete c;
}

void CStatusBase::backward() {
    set<CStatusBase*> S;
    S.insert(this);

    while (!S.empty()) {
        // find a Status which has no children in S
        CStatusBase* cur = NULL;
        for (auto & s: S) {
            bool noChildrenFlag = true;
//            for (auto & c: s->children)
            for (auto & g: s->groups)
                for (auto & c: g->members)
                    if (S.find(c) != S.end())
                        noChildrenFlag  = false;
            if (noChildrenFlag) {
                cur = s;
                break;
            }
        }
        S.erase(S.find(cur));

        if (cur->solved) continue;

        double oldF = cur->f;
        cur->updateF();

        bool oldSolved = cur->solved;
        if (!cur->solved) 
            for (auto & g: cur->groups) {
                g->updateStatus();
                if (g->solved) {
                    cur->solved = true; 
                    cur->bestPath = g;
                }
                // cur->solve is true only if there is one group is solved
            }

        if (oldF != cur->f || oldSolved != cur->solved) // if nothing change, dont pass to its parents
            for (auto & b: cur->belonging)
                S.insert(b->parent);
    }
}

void CStatusBase::backwardFailed() {
    if (!failed) {
        failed = true;
        for (auto & g: groups)
            if (!g->failed) { // still available
                failed = false;
                return;
            }
    }

    for (auto & b: belonging) {
        b->failed = true;
        b->parent->backwardFailed();
    }
}

void CAndBase::updateStatus() {
    solved = true;
    for (auto & m: members) {
        if (!m->solved) solved = false;
        if (m->failed) { failed = true;break; }
    }
}

CAndBase::CAndBase() {
    failed = false;
}

CAndBase::~CAndBase() {
}
