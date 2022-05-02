#include <list>
#include <vector>
#include <iostream>
#include <climits>
#include <chrono>
#include <omp.h>
using namespace std;

// struct representing an edge from u -> v
struct edge {
    int v; // end vertex
    int flow;
    int cap;
    unsigned long back; // index of backwards edge in adj
};

int N = 0; // total number of vertices in graph
int M = 0; // total number of edges in graph
int sinkNode;
int startNode;
int *layer;
vector<edge> *adj;

// adds an edge to the graph by inserting into adjacency list
void createEdge(int u, int v, int cap) {
    edge f{v, 0, cap, (unsigned long)adj[v].size()}; // forward edge
    edge b{u, 0, 0, (unsigned long)adj[u].size()};   // back edge starts out with capacity 0
    adj[u].push_back(f);
    adj[v].push_back(b); 
}

/* Runs BFS: for each iteration, determines if additional flow can be sent
*  from source -> sink. Also assigns levels to the vertices.
*  This version of BFS looks at all the neighbors in parallel rather than 
*  iterating through them sequentially.
*/
bool search(int s, int t) {
    #pragma parallel for private(N)
    for (int i = 0; i < N; i++) {
        layer[i] = -1;
    }
    #pragma omp barrier

    layer[s] = 0;
    list<int> queue;
    queue.push_back(s);

    while (!queue.empty()) {
        int parent = queue.front();
        queue.pop_front();

        std::vector<edge> neighbors = adj[parent];
        int currentLevel = layer[parent];

        #pragma parallel
        for (unsigned int i = 0; i < neighbors.size(); i++){
            edge child = neighbors[i];
        
            if (layer[child.v] < 0  && child.cap > child.flow){
                layer[child.v] = currentLevel + 1;
                #pragma omp critical
                {
                    queue.push_back(child.v);
                }
            }
        }
    }
    return !(layer[t] < 0);
}

// updates edge flows after BFS returns that flow is still possible
int addFlow(int u, int t, int v[], int flow) {
    if (u == t) return flow;

    while ((unsigned int)v[u] < adj[u].size()) {
        edge &e = adj[u][v[u]];

        if ((layer[e.v] == layer[u] + 1) && e.cap > e.flow ) {
            int curr = min(flow, e.cap - e.flow);
            int newFlow = addFlow(e.v, t, v, curr);

            if (newFlow > 0) {
                e.flow += newFlow;
                adj[e.v][e.back].flow -= newFlow;
                return newFlow;
            }
        }
        v[u]++;
    }
    return 0;
}

// returns max flow possible in graph
int maxFlow(int s, int t) {
    int totalFlow = 0;
    if (s == t) return 0;
    int *v = new int[N + 1];

    while (search(s, t)) {
        for (int i = 0; i < N + 1; i++) {
            v[i] = 0;
        }

        while (int f = addFlow(s, t, v, INT_MAX)) {
            totalFlow += f;
        }
    }
    return totalFlow;
}

void printAdjList() {
    cout << "num nodes: " << N << "\n";
    cout << "num edges: " << M << "\n";
    cout << "start node: " << startNode << "\n";
    cout << "sink node: " << sinkNode << "\n";

    for (int i = 0; i < N; i++) {
        for (auto curr = adj[i].begin(); curr != adj[i].end(); ++curr) {
            cout << "edge: (" << i << ", " << curr->v << "), cap: " << curr->cap << "\n";
        }
    }
}

int main(int argc, char *argv[]) {
    omp_set_num_threads(omp_get_max_threads());
    char *input_filename = argv[1];
    FILE *input = fopen(input_filename, "r");
    if (!input) {
        printf("Unable to open file: %s.\n", input_filename);
        return -1;
    }

    fscanf(input, "%d %d %d %d\n", &N, &M, &startNode, &sinkNode);

    adj = new vector<edge>[N];
    layer = (int*)calloc(N, sizeof(int));

    for (int i = 0; i < M; i++) {
        int u, v, cap;
        fscanf(input, "%d %d %d\n", &u, &v, &cap);
        createEdge(u, v, cap);
    } 

    //printAdjList();

    using namespace std::chrono;
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::duration<double> dsec;

    auto compute_start = Clock::now();
    double compute_time = 0;

    int flow = maxFlow(startNode, sinkNode);

    compute_time += duration_cast<dsec>(Clock::now() - compute_start).count();

    cout << "Max flow: " << flow << "\nSequential Dinic time: " << compute_time << "\n";

    return 0;
}

