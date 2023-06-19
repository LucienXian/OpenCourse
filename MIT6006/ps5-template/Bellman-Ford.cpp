
#include <bits/stdc++.h>

struct Edge {
    int src, dst, weight;
};

struct Graph {
    int v, e; // number of vertices, number of edges
    std::vector<Edge> edges; 
};

Graph CreateGraph(int v, int e) {
    Graph g;
    g.v = v;
    g.e = e;
    g.edges.resize(e);
    return g;
}

// No negative Cycle
void InitGraph1(Graph &g) {
    g.edges[0] = {0, 1, -1};
    g.edges[1] = {0, 2, 4};
    g.edges[2] = {1, 2, 3};
    g.edges[3] = {1, 3, 2};
    g.edges[4] = {1, 4, 2};
    g.edges[5] = {3, 2, 5};
    g.edges[6] = {3, 1, 1};
    g.edges[7] = {4, 3, -3};
}

// With negative Cycle
void InitGraph2(Graph &g) {
    g.edges[0] = {0, 1, -1};
    g.edges[1] = {0, 2, 4};
    g.edges[2] = {1, 2, 3};
    g.edges[3] = {1, 3, 2};
    g.edges[4] = {1, 4, 2};
    g.edges[5] = {3, 2, 5};
    g.edges[6] = {3, 1, 1};
    g.edges[7] = {4, 3, -4};
}

std::vector<int> BellmanFord(Graph &g, int src) {
    // 1. Init
    auto v = g.v;
    // auto e = g.e;
    std::vector<int> dists(v, std::numeric_limits<int>::max());
    dists[src] = 0;
    // 2. Relax all edges |V| - 1 times
    const auto& edges = g.edges;
    for (int i = 0; i < v - 1; i++) {
        for (const auto& edge : edges) {
            if (dists[edge.src] + edge.weight < dists[edge.dst]) dists[edge.dst] = dists[edge.src] + edge.weight;
        }
    }
    // 3. Check for negative-weight cycles.
    for (const auto& edge : edges) {
        if (dists[edge.src] != std::numeric_limits<int>::max() && dists[edge.src] + edge.weight < dists[edge.dst]) {
            printf("Graph contains negative weight cycle\n");
            break;
        }
    }
    return dists;
}

void PrintDists(int src, const std::vector<int>& dists) {
    printf("Vertex   Distance from Source %d\n", src);
    int n = dists.size();
    for (int i = 0; i < n; ++i) {
        printf("%d \t\t %d\n", i, dists[i]);
    }
}

int main() {
    auto graph = CreateGraph(5, 8);
    // InitGraph1(graph);
    InitGraph2(graph);
    auto dists = BellmanFord(graph, 0);
    PrintDists(0, dists);
    return 0;
}