#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <utility>
#include <limits>
#include <iostream>

class Graph {
private:
    int numVertices;
    int numEdges;
    std::vector<std::vector<std::pair<int, double>>> adjacencyList;

public:
    Graph();
    bool loadFromFile(const std::string& filename);

    // Getters
    int getNumVertices() const { return numVertices; }
    int getNumEdges() const { return numEdges; }
    const std::vector<std::pair<int, double>>& getNeighbors(int node) const;

    // Add this method for the streaming implementation
    double getWeight(int u, int v) const {
        // Search for edge u->v in adjacency list
        for (const auto& neighbor : adjacencyList[u]) {
            if (neighbor.first == v) {
                return neighbor.second;
            }
        }
        return std::numeric_limits<double>::infinity();
    }

    // Utility functions
    void printGraphInfo() const;
};

#endif // GRAPH_H