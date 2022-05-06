#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include <iostream>
#include <string>
#include <utility>
#include <unistd.h>
#include <queue>
#include <map>
#include <omp.h>
#include <chrono>
#include "pr.h"
#include <math.h> 
#include <limits>
using namespace std;


bool bfs(int *parents, int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {
    bool visited[numNodes];
    for (int i = 0; i < numNodes; i++) {
        visited[i] = false;
    }
    std::queue<int> q;
    q.push(sourceNode);
    visited[sourceNode] = true;
    parents[sourceNode] = -1;

    while (!q.empty()) {
        int currInd = q.front();
        q.pop();
        node_t currNode = nodes[currInd];
        for (auto const& x : currNode.neighbors) {
            int neighborInd = x.first;
            if (visited[neighborInd] == false && x.second.capacity > 0) {
                if (neighborInd == sinkNode) {
                    parents[sinkNode] = currInd;
                    return true;
                }
                q.push(neighborInd);
                parents[neighborInd] = currInd;
                visited[neighborInd] = true;
            }
        }
    }
    return false;
}

void print_node(int i, int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {
    node_t nod = nodes[i];
    std::cout << "Node " << i << ": ";
    std::cout << "neighbors = ";
    for (auto const& x : nod.neighbors) {
        std::cout << "(node = " << x.first << ", cap = " << x.second.capacity << "), ";
    }
    std::cout << "\n";
}

void print_graph(int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {
    std::cout << "PRINTING GRAPH...\n";
    for (int i = 0; i < numNodes; i++) {
        print_node(i, numNodes, numEdges, sourceNode, sinkNode, nodes);
    }
    std::cout << "DONE PRINTING GRAPH!\n";
}

void print_parents(int parents[], int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {
    std::cout << "PRINTING PARENTS...\n";
    for (int i = 0; i < numNodes; i++) {
        std::cout << "node " << i << ": parent = " << parents[i] << "\n";
    }
    std::cout << "DONE PRINTING PARENTS!\n";
}

void mod_residual_graph(int flow, int parents[], int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {
    int curr = sinkNode;
    int parent = parents[sinkNode];
    while (curr != -1) {
        parent = parents[curr];
        if (parent == -1) break;
        nodes[parent].neighbors[curr].capacity -= flow;
        nodes[curr].neighbors[parent].capacity += flow;
        curr = parent;
    }
}

int get_path_flow(int parents[], int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {
    int curr = sinkNode;
    int parent = parents[sinkNode];
    int flow = nodes[parent].neighbors[curr].capacity;
    curr = parent;
    while (curr != -1) {
        parent = parents[curr];
        if (parent == -1) break;
        flow = std::min(flow, nodes[parent].neighbors[curr].capacity);
        curr = parent;
    }
    return flow;
}

void print_all(int numNodes, node_t *nodes) {
    for (int i = 0; i < numNodes; i++) {
        printf("node %d: height = %d, excess = %d\n", i, nodes[i].height, nodes[i].excess);
    }
}

void preflow(int numNodes, int sourceNode, node_t *nodes) {
    for (int i = 0; i < numNodes; i++) {
        nodes[i].height = 0;
    }
    node_t src = nodes[sourceNode];
    src.height = numNodes;

    for (int i = 0; i < src.numNeighbors; i++) {
        int neighborInd = src.neighs[i];
        edge_t* e = &(src.neighbors[neighborInd]);
        e->flow = e->capacity;
        printf("pushed flow = %d\n", e->flow);
        node_t* neigh = &(nodes[neighborInd]);
        neigh->excess += e->flow;
        printf("neighbor %d has excess: %d\n", neighborInd, neigh->excess);
        e->twin->capacity = 0;
    }
}

int getOverflowVertex(int numNodes, node_t *nodes, int sinkNode, int avoid) {
    for (int i = numNodes - 1; i >= 0; i--) {
        if (nodes[i].excess > 0) {
            if (i != avoid) {
                return i;
            }
            // printf("overflow vertex found: %d\n", i);
        }
    }
    printf("no overflow vertex found\n");
    return -1;
}

bool push(int currInd, int numNodes, node_t *nodes) {
    printf("called push on %d\n", currInd);
    node_t* currNode = &(nodes[currInd]);
    for (int i = 0; i < currNode->numNeighbors; i++) {
        // printf("checking %d neighbors\n", currNode->numNeighbors);
        int neighborInd = currNode->neighs[i];
        edge_t* e = &(currNode->neighbors[neighborInd]);
        if (e->capacity != 0 && e->capacity <= e->flow) {
            continue;
        }
        node_t* neigh = &(nodes[neighborInd]);
        if (currNode->height > neigh->height) {
            int flow = min(e->capacity - e->flow, currNode->excess);
            currNode->excess -= flow;
            neigh->excess += flow;
            e->flow += flow;
            e->twin->flow -= flow;
            printf("successfully pushed %d flow from %d to %d\n", flow, currInd, neighborInd);
            return true;
        }
    }
    return false;
}

void relabel(int currInd, int numNodes, node_t *nodes) {
    printf("called relabel on %d\n", currInd);
    int minAdjacentHeight = 10000;
    node_t* currNode = &(nodes[currInd]);
    printf("%d\n", currNode->numNeighbors);
    for (int i = 0; i < currNode->numNeighbors; i++) {
        int neighborInd = currNode->neighs[i];
        edge_t *e = &(currNode->neighbors[neighborInd]);
        printf("neighborInd: %d, capacity: %d, flow: %d\n", neighborInd, e->capacity, e->flow);
        if (e->capacity <= e->flow) {
            continue;
        }

        node_t neigh = nodes[neighborInd];
        if (neigh.height < minAdjacentHeight) {
            minAdjacentHeight = neigh.height;
            printf("neigh %d: height = %d\n", neighborInd, neigh.height);
            currNode->height = minAdjacentHeight + 1;
            printf("new height: %d\n", currNode->height);
        }
    }
}

int pr(int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {
    printf("Sequential pr called\n");
    preflow(numNodes, sourceNode, nodes);
    printf("did preflow\n");
    print_all(numNodes, nodes);
    int avoid = -1;

    while (getOverflowVertex(numNodes, nodes, sinkNode, avoid) != -1) {
        int currInd = getOverflowVertex(numNodes, nodes, sinkNode, avoid);
        if (!push(currInd, numNodes, nodes)) {
            relabel(currInd, numNodes, nodes);
            printf("after relabel:\n");
            push(currInd, numNodes, nodes);
            if (avoid != currInd) avoid = currInd;
            print_all(numNodes, nodes);
        }
    }

    return nodes[sinkNode].excess;
} 