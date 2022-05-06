#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include <iostream>
#include <string>
#include <utility>
#include <unistd.h>
#include "pr.h"
#include <queue>
#include <map>
#include <omp.h>
#include <chrono>

static int numNodes;
static int numEdges;
static int sourceNode;
static int sinkNode;
static node_t *nodes;

int main(int argc, char *argv[]) {
    using namespace std::chrono;
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::duration<double> dsec;

    auto compute_start = Clock::now();
    double compute_time = 0;

    char *input_filename = argv[1];
    FILE *input = fopen(input_filename, "r");
    if (!input) {
        printf("Unable to open file: %s.\n", input_filename);
        return -1;
    }

    std::string mode (argv[2]);
    
    fscanf(input, "%d %d %d %d\n", &numNodes, &numEdges, &sourceNode, &sinkNode);
    int outNode, inNode, capacity;


    nodes = (node_t*)calloc(numNodes, sizeof(node_t));

    int res = 0; 

    if (mode == "s") {

        for (int i = 0; i < numNodes; i++) {
            nodes[i].ind = i;
            nodes[i].numNeighbors = 0;
            nodes[i].neighs = std::vector<int>();
            nodes[i].neighbors = std::map<int, edge_t>();
            nodes[i].height = 0;
            nodes[i].excess = 0;
        }

        for (int i = 0; i < numEdges; i++) {
            fscanf(input, "%d %d %d\n", &outNode, &inNode, &capacity);
            edge_t *newEdge = (edge_t*)calloc(1, sizeof(edge_t));
            edge_t *twinEdge = (edge_t*)calloc(1, sizeof(edge_t));
            newEdge->u = &(nodes[outNode]);
            newEdge->v = &(nodes[inNode]);
            newEdge->capacity = capacity;
            newEdge->twin = twinEdge;
            newEdge->flow = 0;
            ((&(nodes[outNode]))->neighbors)[inNode] = *newEdge;
            ((&(nodes[outNode]))->neighs).push_back(inNode);
            (&(nodes[outNode]))->numNeighbors++;
            twinEdge->u = &(nodes[inNode]);
            twinEdge->v = &(nodes[outNode]);
            twinEdge->capacity = -1 * capacity;
            twinEdge->twin = newEdge;
            twinEdge->flow = 0;
            ((&(nodes[inNode]))->neighbors)[outNode] = *twinEdge;
            ((&(nodes[inNode]))->neighs).push_back(outNode);
            (&(nodes[inNode]))->numNeighbors++;
        }
        res = pr(numNodes, numEdges, sourceNode, sinkNode, nodes);
    }
    else if (mode == "o") {
        ;
    }
        
    printf("Result: %d\n", res);

    compute_time += duration_cast<dsec>(Clock::now() - compute_start).count();
    printf("Computation Time: %lf.\n", compute_time);

    return res;
}