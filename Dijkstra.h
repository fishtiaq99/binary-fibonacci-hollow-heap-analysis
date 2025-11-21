#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"
#include "PriorityQueue.h"
#include <vector>
#include <limits>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>

struct DijkstraResult {
    std::vector<double> distances;
    std::vector<int> parent;
    int insertCount = 0;
    int extractMinCount = 0;
    int decreaseKeyCount = 0;
    int relaxationCount = 0;
    int nodesProcessed = 0;
};

class Dijkstra {
private:
    const Graph& graph;

public:
    Dijkstra(const Graph& g) : graph(g) {}

    std::vector<double> runFromSource(int source, PriorityQueue& pq);
    std::vector<std::vector<double>> runAllPairs(PriorityQueue& pq);
    DijkstraResult runFromSourceWithStats(int source, PriorityQueue& pq);

    static void printPaths(int source,
        const std::vector<double>& distances,
        const std::vector<int>& parent);

    static void printDistances(const std::vector<double>& distances, int source);

    // ADD THESE: Verification and distance saving methods
    static bool verifyHeapConsistency(const Graph& graph, int testSource = 0);
    static void saveDistancesToFile(const std::vector<std::vector<double>>& allDistances,
        const std::string& filename);
    static void saveSingleSourceDistances(const std::vector<double>& distances,
        const std::string& filename, int source);
};

#endif // DIJKSTRA_H