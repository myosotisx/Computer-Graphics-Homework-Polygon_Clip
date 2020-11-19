#include "graph.h"

#include <iostream>
#include <algorithm>
#include <set>
#include <cstring>
using namespace std;


Polygon::Polygon(const Polygon *polygon) {
    int n = polygon->rings.size();
    for(int i=0; i<n; i++) {
        Node* head = new Node(polygon->rings[i]);
        Node* p = head, *q = polygon->rings[i];
        while(q->next) {
            p->next = new Node(q->next);
            q = q->next;
            p = p->next;
        }
        rings.push_back(head);
    }
}

Node* Polygon::genNodeList(const std::vector<std::pair<double, double>>& nodes) {
    int n = nodes.size();
    if(!n) return nullptr;
    Node *head = new Node(nodes[0].first, nodes[0].second);
    Node *p = head;
    for(int i=1; i<n; i++) {
        p->next = new Node(nodes[i].first, nodes[i].second);
        p = p->next;
    }
    return head;
}

bool containRings(const Node *head, const Polygon *polygon) {
    const Node *p = head;
    while(p) {
        if(!inPolygon(p, polygon)) return false;
        p = p->next;
    }
    return true;
}

bool containIntersects(const Node *head) {
    const Node *p = head;
    while(p) {
        if(p->isIntersect) return true;
        p = p->next;
    }
    return false;
}

Node* copyRings(Node* head) {
    if(!head) return nullptr;
    Node *newHead = new Node(head);
    Node *p = newHead;
    Node *q = head;
    while(q->next) {
        p->next = new Node(q->next);
        q = q->next;
        p = p->next;
    }
    return newHead;
}

void printRings(const Node *head) {
    const Node *p = head;
    int cnt = 0;
    while(p) {
        cnt++;
        cout << cnt;
        cout << " x: " << p->x << " y: " << p->y << " ";
        if(p->isIntersect) {
            cout << "intersect ";
            if(p->enter == 1) cout << "enter ";
            else if(p->enter == 0) cout << "leave ";
        }
        cout << endl;
        p = p->next;
    }
    cout << endl;
}

bool inPolygon(double x0, double y0, const Polygon *polygon) {
    int cnt = 0, n = polygon->rings.size();
    if(n == 0) return false;
    Node *head = polygon->rings[0];
    Node *p = head, *p1, *p2;
    while(p) {
        p1 = p;
        p2 = p->next ? p->next: head;
        if(p1->y == p2->y || (p1->y > y0 && p2->y > y0) || (p1->y < y0 && p2->y < y0)) {
            p = p->next;
            continue;
        }
        double x = (y0-(p1->y))*((p2->x)-(p1->x))/((p2->y)-(p1->y))+(p1->x);
        if(x > x0) cnt++;
        p = p->next;
    }
    if(cnt%2 == 0) return false;
    for(int i=1; i<n; i++) {
        cnt = 0;
        Node *head = polygon->rings[i];
        Node *p = head, *p1, *p2;
        while(p) {
            p1 = p;
            p2 = p->next ? p->next: head;
            if(p1->y == p2->y || (p1->y > y0 && p2->y > y0) || (p1->y < y0 && p2->y < y0)) {
                p = p->next;
                continue;
            }
            double x = (y0-(p1->y))*((p2->x)-(p1->x))/((p2->y)-(p1->y))+(p1->x);
            if(x > x0) cnt++;
            p = p->next;
        }
        if(cnt%2) return false;
    }
    return true;
}

bool inPolygon(const Node *node, const Polygon* polygon) {
    return inPolygon(node->x, node->y, polygon);
}

double sqDistance(const Node *a, const Node *b) {
    double dx = (a->x)-(b->x);
    double dy = (a->y)-(b->y);
    return dx*dx+dy*dy;
}

vector<Polygon*> clipPolygons(const Polygon *mainpg, const Polygon *windowpg) {
    vector<Polygon*> res;
    Polygon *mainpgc = new Polygon(mainpg);
    Polygon *windowpgc = new Polygon(windowpg);
    getRingsWithIntersects(mainpgc, windowpgc, windowpg);

    set<Node*> s;
    int n = mainpgc->rings.size();
    for(int i=0; i<n; i++) {
        Node *head = mainpgc->rings[i];
        Node *p = head;
        while(p) {
            if(p->isIntersect && p->enter == 1) s.insert(p);
            p = p->next;
        }
    }
    while(!s.empty()) {
        Node *start = *s.begin();
        Node *head = start->head;
        Node *p = start;
        vector<pair<double, double>> nodes;
        do {
            p = p->next ? p->next: head;
            if(p->isIntersect && p->enter >= 0) {
                p = p->copy;
                head = p->head;
                if(p->enter == 1) s.erase(p);

            }
            nodes.push_back(pair<double, double>(p->x, p->y));

        } while(p != start);
        res.push_back(new Polygon(nodes));
    }

    // 处理主多边形和裁剪多边形外环包含的情况
    if(containRings(mainpg->rings[0], windowpg)) {
        Polygon *polygon = new Polygon();
        polygon->rings.push_back(copyRings(mainpg->rings[0]));
        int n = res.size();
        for(int i=0; i<n; i++) {
            if(containRings(res[i]->rings[0], polygon)) {
                polygon->rings.push_back(copyRings(res[i]->rings[0]));
                res.erase(res.begin()+i);
                i--;
            }
        }
        res.push_back(polygon);
    }
    else if(containRings(windowpg->rings[0], mainpg)) {
        Polygon *polygon = new Polygon();
        polygon->rings.push_back(copyRings(windowpg->rings[0]));
        int n = res.size();
        for(int i=0; i<n; i++) {
            if(containRings(res[i]->rings[0], polygon)) {
                polygon->rings.push_back(copyRings(res[i]->rings[0]));
                res.erase(res.begin()+i);
                i--;
            }
        }
        res.push_back(polygon);
    }

    // 处理主多边形和裁剪多边形内部内环包括在裁剪结果中但未被添加为结果多边形内环的情况
    n = res.size();
    for(int i=0; i<n; i++) {
        int m = mainpgc->rings.size();
        for(int j=1; j<m; j++) {
            if(containIntersects(mainpgc->rings[j])) continue;
            if(containRings(mainpgc->rings[j], res[i])) {
                res[i]->rings.push_back(copyRings(mainpgc->rings[j]));
            }
        }
        m = windowpgc->rings.size();
        for(int j=1; j<m; j++) {
            if(containIntersects(windowpgc->rings[j])) continue;
            if(containRings(windowpgc->rings[j], res[i])) {
                res[i]->rings.push_back(copyRings(windowpgc->rings[j]));
            }
        }
    }

    // 处理裁剪结果多边形之间相互包含（为内外环关系）的情况
    bool isIn[n];
    memset(isIn, 0, sizeof(isIn));
    n = res.size();
    for(int i=0; i<n; i++) {
        if(isIn[i]) continue;
        for(int j=0; j<n; j++) {
            if(isIn[j] || res[j]->rings.size() > 1) continue;
            if(containRings(res[j]->rings[0], res[i])) {
                isIn[j] = true;
                res[i]->rings.push_back(copyRings(res[j]->rings[0]));
            }
        }
    }
    vector<Polygon*> resc;
    for(int i=0; i<n; i++) {
        if(isIn[i]) continue;
        resc.push_back(new Polygon(res[i]));
    }
    return resc;
}

void getRingsWithIntersects(Polygon *mainpgc, Polygon *windowpgc, const Polygon *windowpg) {
    int nmain = mainpgc->rings.size(), nwindow = windowpgc->rings.size();
    for(int i=0; i<nmain; i++) {
        for(int j=0; j<nwindow; j++) {
            Node *head1 = mainpgc->rings[i], *head2 = windowpgc->rings[j];
            Node *p = head1, *p1, *p2, *refp, *q, *q1, *q2, *refq, *tmp;
            while(p) {
                if(p->isIntersect) {
                    p = p->next;
                    continue;
                }
                p1 = p;
                p2 = p1->next;
                while(p2 && p2->isIntersect) p2 = p2->next;
                if(!p2) p2 = head1;
                vector<pair<Node*, Node*>> intersects;
                q = head2;
                while(q) {
                    if(q->isIntersect) {
                        q = q->next;
                        continue;
                    }
                    q1 = q;
                    q2 = q1->next;
                    while(q2 && q2->isIntersect) q2 = q2->next;
                    if(!q2) q2 = head2;
                    Node *node = calcIntersect(p1, p2, q1, q2);
                    if(node) intersects.push_back(pair<Node*, Node*>(node, q));
                    q = q->next;
                }
                int n = intersects.size();
                refp = p;
                for(int i=0; i<n; i++) {
                    Node *node = new Node(intersects[i].first);
                    while(p->next && p->next->isIntersect && sqDistance(p->next, refp) <= sqDistance(node, refp)) p = p->next;
                    tmp = p->next;
                    p->next = node;
                    node->next = tmp;
                    p = p->next;
                    p->isIntersect = true;
                    p->head = head1; // 方便walk时到达链表结尾后返回头部
                    p->copy = new Node(intersects[i].first);

                    node = p->copy;
                    refq = intersects[i].second;
                    q = refq;
                    while(q->next && q->next->isIntersect && sqDistance(q->next, refq) <= sqDistance(node, refq)) q = q->next;
                    tmp = q->next;
                    q->next = node;
                    node->next = tmp;
                    q = q->next;
                    q->isIntersect = true;
                    q->head = head2;
                    q->copy = p;

                    p = refp; // p指回原顶点
                }
                p = p->next;
            }
        }
        Node *p = mainpgc->rings[i];
        while(p) {
            if(p->next && p->next->isIntersect) {
                int enter = inPolygon(p, windowpg) ? 0: 1;
                do {
                    p = p->next;
                    p->enter = enter;
                    p->copy->enter = enter;
                    enter = 1-enter;
                } while(p->next && p->next->isIntersect);
            }
            p = p->next;
        }
    }
}


Node* calcIntersect(const Node *a, const Node *b, const Node *c, const Node *d) {
    if(c->x == d->x) {
        const Node *tmp = c;
        c = a;
        a = tmp;

        tmp = d;
        d = b;
        b = tmp;
    }
    if(a->x == b->x) {
        if(c->x == d->x) return nullptr;
        double k2 = ((c->y)-(d->y))/((c->x)-(d->x));
        double b2 = (c->y)-k2*(c->x);
        double y = k2*(a->x)+b2;
        if(y >= min(a->y, b->y) && y >= min(c->y, d->y) && y <= max(a->y, b->y) && y <= max(c->y, d->y)) {
            return new Node(a->x, y, true);
        }
        else return nullptr;
    }
    double k1 = ((a->y)-(b->y))/((a->x)-(b->x)), k2 = ((c->y)-(d->y))/((c->x)-(d->x));
    double b1 = (a->y)-k1*(a->x), b2 = (c->y)-k2*(c->x);
    double x = (b2-b1)/(k1-k2), y = (k1*b2-k2*b1)/(k1-k2);
    if(k1 == k2 && b1 == b2) return nullptr;
    if(x >= min(a->x, b->x) && x >= min(c->x, d->x) && x <= max(a->x, b->x) && x <= max(c->x, d->x)) {
        return new Node(x, y, true);
    }
    else return nullptr;
}
