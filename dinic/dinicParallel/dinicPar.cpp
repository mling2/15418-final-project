#include <list>
#include <vector>
#include <iostream>
#include <climits>
#include <chrono>
using namespace std;

// struct representing an edge from u -> v
struct edge {
    int v; // end vertex
    int flow;
    int cap;
    unsigned long back; // index of backwards edge in adj
};

int N; // total number of vertices in graph
int M; // total number of edges in graph
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

/* runs BFS: for each iteration, determines if additional flow can be sent
* from source -> sink. Also assigns levels to the vertices */
bool search(int s, int t) {
    for (int i = 0; i < N; i++) {
        layer[i] = -1;
    }

    layer[s] = 0;
    list<int> queue;
    queue.push_back(s);

    while (!queue.empty()) {
        int parent = queue.front();
        queue.pop_front();
        cout << "searching parent: " << parent << "\n";
        for (auto curr = adj[parent].begin(); curr != adj[parent].end(); curr++) {
            if (layer[curr->v] < 0 && curr->cap > curr->flow) {
                layer[curr->v] = layer[parent] + 1;
                queue.push_back(curr->v);
            }
        }
    }
    return !(layer[t] < 0);
}

// updates edge flows after BFS returns that flow is still possible
int addFlow(int u, int t, int v[], int flow) {
    if (u == t) return flow;

    while (v[u] < adj[u].size()) {
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

    while (search(s, t)) {
        int *v = new int[N + 1];
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
    char *input_filename = argv[1];
    FILE *input = fopen(input_filename, "r");
    if (!input) {
        printf("Unable to open file: %s.\n", input_filename);
        return -1;
    }

    fscanf(input, "%d %d %d %d\n", &N, &M, &startNode, &sinkNode);

    adj = new vector<edge>[N];
    layer = new int[N];

    for (int i = 0; i < M; i++) {
        int u, v, cap;
        fscanf(input, "%d %d %d\n", &u, &v, &cap);
        createEdge(u, v, cap);
    } 

    //printAdjList();

    //auto startTime = chrono::steady_clock::now();
    int flow = maxFlow(startNode, sinkNode);
    //auto endTime = chrono::steady_clock::now();
    //auto diff = endTime - startTime;

    //cout << "Max flow: " << flow << "\n Sequential Dinic time: " << diff << "\n";
    cout << "Max flow: " << flow << "\n";

    return 0;
}