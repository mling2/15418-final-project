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
#include "ford_fulkerson.h"
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

bool bfs_omp(int *parents, int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {

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

        int num_of_threads = 4;
        omp_set_num_threads(num_of_threads);
        bool foundSink = false;
        #pragma omp parallel for default(shared) shared(foundSink) schedule(dynamic)
        for (int i = omp_get_thread_num(); i < currNode.numNeighbors; i += num_of_threads) {
            printf("threads: %d\n", omp_get_num_threads());
            int neighborInd = currNode.neighs.at(i);
            edge_t e = currNode.neighbors[neighborInd];
            if (visited[neighborInd] == false && e.capacity > 0) {
                if (neighborInd == sinkNode) {
                    parents[sinkNode] = currInd;
                    foundSink = true;
                }
                q.push(neighborInd);
                parents[neighborInd] = currInd;
                visited[neighborInd] = true;
            }
        }
        
        if (foundSink) return true;
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

int ff(int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {
    printf("Sequential ff called\n");
    int parents[numNodes];
    memset(parents, -1, sizeof(parents));
    int flow = 0;
    while (bfs(parents, numNodes, numEdges, sourceNode, sinkNode, nodes)) {
        printf("found more flow\n");
        int add_flow = get_path_flow(parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        flow += add_flow;
        mod_residual_graph(add_flow, parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        memset(parents, -1, sizeof(parents));
    }
    return flow;
} 

int ff_omp(int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes) {
    printf("OpenMP ff called\n");
    int parents[numNodes];
    memset(parents, -1, sizeof(parents));
    int flow = 0;
    while (bfs_omp(parents, numNodes, numEdges, sourceNode, sinkNode, nodes)) {
        printf("found more flow\n");
        int add_flow = get_path_flow(parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        flow += add_flow;
        mod_residual_graph(add_flow, parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        memset(parents, -1, sizeof(parents));
    }
    return flow;
} 