#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

class Node {
public:
    double x, y;
    bool isIntersect;
    int enter; // -1表示未定义，1表示入点，2表示出点
    Node *next, *copy, *head;
    Node(double _x, double _y, bool _isIntersect=false, int _enter=-1, Node *_next=nullptr, Node *_copy=nullptr, Node *_head=nullptr): x(_x), y(_y),
        isIntersect(_isIntersect),
        enter(_enter),
        next(_next),
        copy(_copy),
        head(_head) {}
    Node(Node *node): x(node->x), y(node->y), isIntersect(false), enter(-1), next(nullptr), copy(nullptr), head(nullptr) {}
};


class Polygon {
public:
    std::vector<Node*> rings;
    Polygon() {}
    Polygon(const std::vector<std::pair<double, double>>& outNodes) {
        Node *head = genNodeList(outNodes);
        if(head) rings.push_back(head);
    }
    Polygon(const Polygon *polygon);
    static Node* genNodeList(const std::vector<std::pair<double, double>>& nodes);

};


void printRings(const Node *head);

bool copyRings(const Node *head, const Polygon *polygon);

bool inPolygon(double x, double y, const Polygon* polygon);

bool inPolygon(const Node *node, const Polygon* polygon);

std::vector<Polygon*> clipPolygons(const Polygon *mainpg, const Polygon *windowpg);

void getRingsWithIntersects(Polygon *mainpgc, Polygon *windowpgc, const Polygon *windowpg);

double sqDistance(const Node *a, const Node *b);

Node* calcIntersect(const Node *a, const Node *b, const Node *c, const Node *d);

bool containIntersects(const Node *head);

#endif // GRAPH_H
