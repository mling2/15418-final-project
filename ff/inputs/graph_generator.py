import random

def directed_graph_generator(minNumNodes, maxNumNodes, minNumEdges, maxNumEdges):
    numNodes = random.randint(minNumNodes, maxNumNodes)
    numEdges = random.randint(minNumEdges, min(maxNumEdges, numNodes * (numNodes - 1)/2))
    M = [ ([0] * numNodes) for i in range(numNodes) ]
    edgeCount = 0
    while (edgeCount < numEdges):
        outNode = random.randint(0, numNodes - 1)
        inNode = random.randint(0, numNodes - 1)
        if (inNode != outNode) and M[outNode][inNode] != 1:
            M[outNode][inNode] = random.randint(1, 1000)
            edgeCount += 1
    return numNodes, numEdges, M

def find_source_and_sink(graph):
    numNodes = len(graph)
    source = random.randint(0, numNodes - 1)
    sink = random.randint(0, numNodes - 1)
    return source, sink
    

def network_flow_generator(minNumNodes, maxNumNodes, minNumEdges, maxNumEdges):
    numNodes, numEdges, graph = directed_graph_generator(minNumNodes, maxNumNodes, minNumEdges, maxNumEdges)
    print("generated graph")
    source, sink = find_source_and_sink(graph)
    
    with open('graph_x.txt', 'w') as f:
        f.write(f'{numNodes} {numEdges} {source} {sink}\n')
        for i in range(numNodes):
            for j in range(numNodes):
                if graph[i][j] > 0: 
                    f.write(f'{i} {j} {graph[i][j]}\n')
    
minNumNodes = 4000
maxNumNodes = 4000
minNumEdges = 1000000
maxNumEdges = 1000000
network_flow_generator(minNumNodes, maxNumNodes, minNumEdges, maxNumEdges)