#ifndef _FF_H
#define _FF_H

#include <vector>
#include <map>

struct node;
struct edge;

typedef struct edge {
    int capacity;
    node *u;
    node *v;
    edge *twin;
} edge_t;

typedef struct node {
    int ind;
    int numNeighbors;
    std::map<int, edge_t> neighbors;
} node_t;

#endif 