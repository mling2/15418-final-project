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
    int flow;
} edge_t;

typedef struct node {
    int ind;
    int numNeighbors;
    std::vector<int> neighs;
    std::map<int, edge_t> neighbors;
    int height;
    int excess;
} node_t;

int pr(int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes);
 
#endif 