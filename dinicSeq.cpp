#include <list>
#include <vector>
#include <iostream>
#include <climits>
#include <chrono>
using namespace std;

// struct representing an edge from u -> v
struct edge 
{
    int v; // end vertex
    int flow;
    int cap;
    long unsigned int back; // index of backwards edge in adj
};

int N; // total number of vertices in graph
int *layer;
vector<edge> *adj;

// adds an edge to the graph by inserting into adjacency list
void createEdge(int u, int v, int cap)
{
    edge f{v, 0, cap, adj[v].size()}; // forward edge
    edge b{u, 0, 0, adj[u].size()};   // back edge starts out with capacity 0
    adj[u].push_back(f);
    adj[v].push_back(b); 
}

/* runs BFS: for each iteration, determines if additional flow can be sent
* from source -> sink. Also assigns levels to the vertices */
bool search(int s, int t)
{
    for (int i = 0; i < N; i++)
    {
        layer[i] = -1;
    }

    layer[s] = 0;
    list<int> queue;
    queue.push_back(s);

    vector<edge>::iterator curr;
    while (!queue.empty())
    {
        int parent = queue.front();
        queue.pop_front();
        for (curr = adj[parent].begin(); curr != adj[parent].end(); curr++)
        {
            edge &e = *curr;
            if (e.cap > e.flow && layer[e.v] < 0)
            {
                layer[e.v] = layer[parent] + 1;
                queue.push_back(e.v);
            }
        }
    }
    return !(layer[t] < 0);
}

// updates edge flows after BFS returns that flow is still possible
int addFlow(int u, int t, int v[], int flow)
{
    if (u == t) return flow;

    while (v[u] < adj[u].size())
    {
        edge &e = adj[u][v[u]];

        if (e.cap > e.flow && (layer[e.v] == layer[u] + 1))
        {
            int curr = min(flow, e.cap - e.flow);
            int newFlow = addFlow(e.v, t, v, curr);

            if (newFlow > 0)
            {
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
int maxFlow(int s, int t)
{
    int totalFlow = 0;
    if (s == t) return 0;

    while (search(s, t))
    {
        int *v = new int[N + 1];
        for (int i = 0; i < N + 1; i++)
        {
            v[i] = 0;
        }

        while (int f = addFlow(s, t, v, INT_MAX))
        {
            totalFlow += f;
        }
    }
    return totalFlow;
}

int main()
{
    adj = new vector<edge>[N];
    layer = new int[N];

    /*
    CODE HERE TO PARSE GRAPH INPUT AND INITIALIZE GRAPH
    */

    auto start = chrono::steady_clock::now();
    // if there are n vertices:
    // source vertex is 0
    // sink vertex is n - 1
    int flow = maxFlow(0, n - 1);
    auto end = chrono::steady_clock::now();
    auto diff = end - start;

    cout << "Max flow: " << flow << "\n Sequential Dinic time: " << diff << "\n";

    return 0;
}
