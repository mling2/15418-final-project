# Project Proposal: Parallelizing Network Flow Algorithms

```
Michelle Ling, Michelle Zhang
```
**SUMMARY**

For our project, we intend to write parallel versions of common sequential network flow
algorithms, including Dinic’s Algorithm and the Ford-Fulkerson Algorithm. These
algorithms are used to solve maximum flow problems on graphs, and when run on large
networks, they may not always have optimal performance and would thus benefit from
parallelism. To evaluate the performance of our algorithms, we will run our parallelized
algorithms on various types of graphs and calculate the speedup with respect to the
sequential implementations.

**BACKGROUND**

Network flows can be used to model a variety of graph problems and are used
heavily in operations research and related fields. Max flow has many applications,
including image segmentation, modeling traffic, and network connectivity. Flows can be
constructed on directed graphs, where each graph has a source node and a sink node.
Edges have weights indicating their capacity, or how much flow can be carried from one
node to another. In addition to capacities, edges can also have costs, and a network
flow problem often involves calculating what the maximum amount of flow that can be
carried from the source node to the sink node is while using the minimum cost.

Many problems can be reduced to a network flow problem. One popular problem
is bipartite matching, which is itself very applicable to scenarios like matching up
applicants and positions, customers and suppliers, etc. It can also be easily translated
into a network flow problem by adding dummy source and sink nodes and adjusting the
capacities.

There are several different algorithms for solving the maximum flow problem.
Perhaps the most basic is Ford-Fulkerson, which pushes the maximum flow along the
graph while it is possible to do so. The algorithm employs the concept of a residual
graph, which contains a graph with modified leftover capacities after a certain amount of
flow has been pushed through a path. For instance, if a flow amount f goes through an
edge with initial capacity c, its residual capacity in the residual graph would be c - f.
Ford-Fulkerson has a sequential runtime of O(mF), where F is the value of the
maximum flow and m is the number of edges in the graph. With larger graph inputs, F
could be a very large value, making Ford-Fulkerson pretty inefficient.

A more intuitive algorithm is Edmonds-Karp, which picks the path in the current
graph of the largest capacity and pushes that much flow through the graph. This
algorithm has a sequential runtime of O(m^2 logF). While this reduces the dependence 
of the algorithm on F from being in poly-time to logarithmic, note that Edmonds-Karp is
not parallelizable between iterations due to the dependence on maximum flow.
However, we can eliminate the dependence on the maximum flow entirely with Dinic’s
algorithm, which has a runtime of O(mn), where n is the number of nodes in the graph.
Both Dinic’s and Edmonds-Karp employ a search algorithm (BFS) as a
subroutine, which offers a lot of room for parallelization. We can also test different graph
representations (e.g. adjacency list, adjacency matrix) and examine how the data
structure impacts the speedup as well.

**CHALLENGES**
As the most common network flow algorithms are inherently sequential or
iterative by nature, there are many challenges that come with implementing parallel
versions of them. In particular, in the sequential algorithms, we iteratively generate
residual graphs (the state of the graph after pushing flow during one iteration) and
perform more rounds of computations on them in subsequent iterations. As we work on
our project, we’ll need to figure out exactly what parts of the algorithms can be safely
and correctly parallelized. We’ll also need to figure out how to synchronize between
iterations and what data should be shared.

Additionally, there could be many dependencies between nodes in the graph,
which would make it difficult to figure out how to partition the total work or graph
computation among all threads. Additionally, to improve performance, we would like to
minimize communication between threads and figure out how to decrease cache misses
and improve locality, which present additional challenges.

## RESOURCES

**Code-related:**

- Our sequential approaches will be inspired by either pseudocode (Dinic’s) or
    high-level descriptions (Ford-Fulkerson, Edmond-Karp) of the algorithm from the
    15-451 lecture notes.
       - https://www.cs.cmu.edu/~15451-s22/lectures/lec10-flow1.pdf
       - https://www.cs.cmu.edu/~15451-s22/lectures/lec11-flow2.pdf
- Our parallel approaches will be inspired by papers on previous efforts to
    parallelize the algorithms. They generally include high-level descriptions of their
    approaches and possibly some pseudocode as well.
       - https://pdfs.semanticscholar.org/c4fb/713ed6b41672dc51782513062cd
          0d979c3.pdf(Edmond-Karp)
       - [http://worldcomp-proceedings.com/proc/p2013/PDP3767.pdf](http://worldcomp-proceedings.com/proc/p2013/PDP3767.pdf)
          (Ford-Fulkerson)


**Hardware-related:**

- We will use the Gates cluster and PSC machines to run tests on our code and
    measure speedup.

**Software-related:**

- We will implement our code in C++, and use the OpenMP and MPI frameworks in
    our parallelism approaches.

**Other:**

- We will need input graphs of different sizes to test our approaches on, so we will
    either find suitable data sets online (e.g.https://snap.stanford.edu/data/)and
    convert them to the desired input formats, or create our own input data.

**GOALS AND DELIVERABLES**

**75%:** The bare minimum we’d like to complete is implementingparallel versions of
Dinic’s Algorithm and the Ford-Fulkerson Algorithm.

**100%:** If everything goes according to plan, we willimplement parallel versions of
Dinic’s Algorithm, the Ford-Fulkerson Algorithm, and the Edmonds-Karp Algorithm.

**125%:** If we have extra time, we will try to implementa parallel version of the
Push-Relabel Algorithm. We can also try optimizing our parallel versions, for example
by creating lock-free implementations, or perhaps further improving locality by
examining our cache performance.

Our overall goal is toachieve significant speedup(especially on larger input graphs)
over the sequential versions of these algorithms.

**Deliverables:** After running tests on various typesof input graphs, we’ll create
charts/graphs showing the speedup of our parallel algorithms with respect to the
sequential versions. We will also discuss practical use cases of these parallel algorithms
and what advantages each of them might have over the others (i.e. when to use which
algorithm, depending on the characteristics of the input graph). Using our results, we’ll
highlight the differences between the algorithms and analyze the parallelizability of each
one, and hypothesize why some may be easier to parallelize than others. If we have
time, it might be instructive to create a visual demo of one of our algorithms at work.

**SCHEDULE**


**Week 1 (March 27 - April 2)**
● Sequential Ford-Fulkerson implementation
● Parallelize Ford-Fulkerson with OpenMP and MPI
**Week 2 (April 3 - April 9)**
● Run tests and compare parallel Ford-Fulkerson approaches
● Sequential Dinic’s implementation
**Week 3 (April 10 - 16)**
● Parallelize Dinic’s with OpenMP and MPI
● Run tests and compare parallel Dinic’s approaches
**Week 4 (April 17 - April 23)**
● Sequential Edmonds-Karp implementation
● Parallelize Edmonds-Karp with OpenMP and MPI
● Run tests and compare parallel Edmonds-Karp approaches
**Week 5 (April 24 - April 29)**
● Run tests on all three algorithms and compare efficiency of approaches on
different types of inputs
● Complete report
● (Work on optional algorithms if time permits)
**Week 6 (April 30 - May 5)**
● Prepare for presentation


