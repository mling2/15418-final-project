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
#include <math.h> 
using namespace std;

// bool is_done_visiting(bool visited[], int numNodes) {
//     for (int i = 0; i < numNodes; i++) {
//         if (visited[i] == false) return false;
//     }
//     return true;
// }

// void subservientThread(int procID, int nproc, int numNodes, int numEdges, node_t *nodes) {
//     bool finish = false;
//     int numReceive = 0;
//     int send_data_max = numNodes * 2;
//     int* send_data = (int*)(calloc(send_data_max, sizeof(int)));
//     while (!finish) {
//         MPI_Bcast(&numReceive, 1, MPI_INT, 0, MPI_COMM_WORLD);
//         int recv_data[numReceive];
//         for (int i = 0; i < numReceive; i++) {
//             recv_data[i] = -1;
//         }
//         MPI_Bcast(&recv_data, numReceive, MPI_INT, 0, MPI_COMM_WORLD);
//         int send_data_count = 0;
//         for (int i = 0; i < numReceive; i++) {
//             int currInd = recv_data[i];
//             if ((currInd % (nproc - 1)) == (procID - 1)) {
//                 node_t currNode = nodes[currInd];
//                 for (auto const& x : currNode.neighbors) {
//                     int neighborInd = x.first;
//                     if (x.second.capacity > 0) {
//                         send_data[2*send_data_count] = neighborInd;
//                         send_data[2*send_data_count + 1] = currInd;
//                         send_data_count++;
//                     }
//                 }
//             }
//         }
//         MPI_Send((void *)send_data, send_data_count * 2, MPI_INT, 0, procID, MPI_COMM_WORLD);
        
//         float completelyFinish = -1;
//         MPI_Bcast(&completelyFinish, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
//         if (completelyFinish == 2) finish = true;
//     }
// }

// void compute(int procID, int nproc, int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes, int *parents) {
//     bool visited[numNodes];
//     for (int i = 0; i < numNodes; i++) {
//         visited[i] = false;
//     }
//     std::queue<int> q;
//     q.push(sourceNode);
//     visited[sourceNode] = true;
//     parents[sourceNode] = -1;
//     bool foundSink = false;
//     while ((!is_done_visiting(visited, numNodes)) && (!foundSink) && q.size() > 0) {
//         int qSize = q.size();
//         int updatedQueue[qSize];
//         for (int i = 0; i < qSize; i++) {
//             int elem = q.front();
//             q.pop();
//             updatedQueue[i] = elem;
//         }
        
//         MPI_Bcast(&qSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
//         MPI_Bcast(&updatedQueue, qSize, MPI_INT, 0, MPI_COMM_WORLD);

//         int* recv_data = (int*)(calloc(numNodes * 2, sizeof(int)));
//         for (int i = 0; i < numNodes * 2; i++) {
//             recv_data[i] = -1;
//         }
//         int recv_count = 0;

//         while (recv_count < nproc - 1) {
//             int ready = 0; MPI_Status status; MPI_Request request;
//             MPI_Irecv((void*)recv_data, numNodes * 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
//             while (!ready) {
//                 MPI_Test(&request, &ready, &status);
//             }
//             int count = 0;
            
//             MPI_Get_count(&status, MPI_INT, &count);

//             for (int i = 0; i < count; i += 2) {
//                 int nodeInd = recv_data[i];
//                 int parentInd = recv_data[i + 1];
//                 if (visited[nodeInd] == false) {
//                     parents[nodeInd] = parentInd; 
//                     visited[nodeInd] = true;
//                     q.push(nodeInd);
//                 }
//                 if (nodeInd == sinkNode) {
//                     foundSink = true;
//                 }
//             }
//             recv_count++;
//         }
//     }
// }

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

bool bfs_omp(int *parents, int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes, int num_of_threads) {

    bool visited[numNodes];
    for (int i = 0; i < numNodes; i++) {
        visited[i] = false;
    }
    std::queue<int> q;
    q.push(sourceNode);
    visited[sourceNode] = true;
    parents[sourceNode] = -1;

    omp_set_num_threads(num_of_threads);

    while (!q.empty()) {
        int currInd = q.front();
        q.pop();
        node_t currNode = nodes[currInd];
        
        bool foundSink = false;
        #pragma omp parallel for shared(foundSink, parents, visited, q)
        for (int i = 0; i < currNode.numNeighbors; i++) {
            int neighborInd = currNode.neighs.at(i);
            edge_t e = currNode.neighbors[neighborInd];
            if (visited[neighborInd] == false && e.capacity > 0) {
                if (neighborInd == sinkNode) {
                    parents[sinkNode] = currInd;
                    foundSink = true;
                }
                #pragma omp critical
                {
                    q.push(neighborInd);
                }
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
        int add_flow = get_path_flow(parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        flow += add_flow;
        mod_residual_graph(add_flow, parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        memset(parents, -1, sizeof(parents));
    }
    return flow;
} 

int ff_omp(int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes, int num_of_threads) {
    int parents[numNodes];
    memset(parents, -1, sizeof(parents));
    int flow = 0;
    while (bfs_omp(parents, numNodes, numEdges, sourceNode, sinkNode, nodes, num_of_threads)) {
        int add_flow = get_path_flow(parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        flow += add_flow;
        mod_residual_graph(add_flow, parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        memset(parents, -1, sizeof(parents));
    }
    return flow;
} 

int ff_mpi(int procID, int nproc, int numNodes, int numEdges, int sourceNode, int sinkNode, node_t *nodes, int num_of_threads) {
    if (procID == 0) {
        // int parents[numNodes];
        // memset(parents, -1, sizeof(parents));
        // int flow = 0;
        // bool stillFound = true;
        // while (stillFound) {
        //     compute(procID, nproc, numNodes, numEdges, sourceNode, sinkNode, nodes, parents);
        //     float found = -1.99;
        //     if (parents[sinkNode] != -1) {
        //         found = 100;
        //         int add_flow = get_path_flow(parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        //         flow += add_flow;
        //         mod_residual_graph(add_flow, parents, numNodes, numEdges, sourceNode, sinkNode, nodes);
        //         memset(parents, -1, sizeof(parents));
        //     }
        //     else {
        //         stillFound = false;
        //     }
        //     MPI_Bcast(&found, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        // }
    }
    else {
        // bool isDone = false;
        // while (!isDone) {
        //     subservientThread(procID, nproc, numNodes, numEdges, nodes);
        //     float isDoneFloat = -1;
        //     MPI_Bcast(&isDoneFloat, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        //     if (isDoneFloat == 1) isDone = true;
        // }
    }
    return 0;
} 