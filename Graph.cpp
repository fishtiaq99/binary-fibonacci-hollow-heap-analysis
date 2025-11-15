#include "Graph.h"
#include <fstream>
#include <iostream>
#include <sstream>

Graph::Graph() : numVertices(0), numEdges(0) {}

bool Graph::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    // Reset
    adjacencyList.clear();
    numVertices = 0;
    numEdges = 0;

    std::string line;
    int maxNodeId = 0;

    // First pass: find the maximum node ID to size our adjacency list
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        int u, v;
        double weight;
        if (iss >> u >> v >> weight) {
            maxNodeId = std::max(maxNodeId, std::max(u, v));
            numEdges++;
        }
    }

    numVertices = maxNodeId + 1;
    adjacencyList.resize(numVertices);

    // Second pass: build the adjacency list
    file.clear();
    file.seekg(0);

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        int u, v;
        double weight;
        if (iss >> u >> v >> weight) {
            adjacencyList[u].emplace_back(v, weight);
            // For undirected graphs, we'd add both directions
            // adjacencyList[v].emplace_back(u, weight);
        }
    }

    file.close();
    std::cout << "Loaded graph: " << numVertices << " vertices, " << numEdges << " edges" << std::endl;
    return true;
}

const std::vector<std::pair<int, double>>& Graph::getNeighbors(int node) const {
    static const std::vector<std::pair<int, double>> empty;
    if (node < 0 || node >= numVertices) return empty;
    return adjacencyList[node];
}

void Graph::printGraphInfo() const {
    std::cout << "Graph Info: " << numVertices << " vertices, " << numEdges << " edges" << std::endl;
}