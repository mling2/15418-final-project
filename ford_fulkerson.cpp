#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include <iostream>
#include <string>
#include <utility>
#include <unistd.h>
#include "ford_fulkerson.h"
#include <queue>
#include <map>

static int numNodes;
static int numEdges;
static int sourceNode;
static int sinkNode;
static node_t *nodes;

bool bfs(int *parents) {
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

void print_node(int i) {
    node_t nod = nodes[i];
    std::cout << "Node " << i << ": ";
    std::cout << "neighbors = ";
    for (auto const& x : nod.neighbors) {
        std::cout << "(node = " << x.first << ", cap = " << x.second.capacity << "), ";
    }
    std::cout << "\n";
}

void print_graph() {
    std::cout << "PRINTING GRAPH...\n";
    for (int i = 0; i < numNodes; i++) {
        print_node(i);
    }
    std::cout << "DONE PRINTING GRAPH!\n";
}

void print_parents(int parents[]) {
    std::cout << "PRINTING PARENTS...\n";
    for (int i = 0; i < numNodes; i++) {
        std::cout << "node " << i << ": parent = " << parents[i] << "\n";
    }
    std::cout << "DONE PRINTING PARENTS!\n";
}

void mod_residual_graph(int flow, int parents[]) {
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

int get_path_flow(int parents[]) {
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

int ff() {
    int parents[numNodes];
    memset(parents, -1, sizeof(parents));
    int flow = 0;
    while (bfs(parents)) {
        int add_flow = get_path_flow(parents);
        flow += add_flow;
        mod_residual_graph(add_flow, parents);
        memset(parents, -1, sizeof(parents));
    }
    return flow;
} 

int main(int argc, char *argv[]) {
    char *input_filename = argv[1];
    FILE *input = fopen(input_filename, "r");
    if (!input) {
        printf("Unable to open file: %s.\n", input_filename);
        return -1;
    }
    
    fscanf(input, "%d %d %d %d\n", &numNodes, &numEdges, &sourceNode, &sinkNode);
    int outNode, inNode, capacity;
    nodes = (node_t*)calloc(numNodes, sizeof(node_t));
    for (int i = 0; i < numNodes; i++) {
        nodes[i].ind = i;
        nodes[i].numNeighbors = 0;
        nodes[i].neighbors = std::map<int, edge_t>();
    }
    for (int i = 0; i < numEdges; i++) {
        fscanf(input, "%d %d %d\n", &outNode, &inNode, &capacity);
        edge_t *newEdge = (edge_t*)calloc(1, sizeof(edge_t));
        edge_t *twinEdge = (edge_t*)calloc(1, sizeof(edge_t));
        newEdge->u = &(nodes[outNode]);
        newEdge->v = &(nodes[inNode]);
        newEdge->capacity = capacity;
        newEdge->twin = twinEdge;
        ((&(nodes[outNode]))->neighbors)[inNode] = *newEdge;
        (&(nodes[outNode]))->numNeighbors++;
        twinEdge->u = &(nodes[inNode]);
        twinEdge->v = &(nodes[outNode]);
        twinEdge->capacity = -1 * capacity;
        twinEdge->twin = newEdge;
        ((&(nodes[inNode]))->neighbors)[outNode] = *twinEdge;
        (&(nodes[inNode]))->numNeighbors++;
    }
    int res = ff();
    std::cout << "res = " << res << "\n";
    return res;
}