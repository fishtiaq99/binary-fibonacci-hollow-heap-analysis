#include "Dijkstra.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "BinaryHeap.h"
#include "FibonacciHeap.h"
#include "HollowHeap.h"

static std::vector<int> reconstructPath(int target, const std::vector<int>& parent) {
    std::vector<int> path;
    for (int v = target; v != -1; v = parent[v])
        path.push_back(v);

    std::reverse(path.begin(), path.end());
    return path;
}

// -------------------------------------------------------------
// Print full shortest paths
// -------------------------------------------------------------
void Dijkstra::printPaths(int source,
    const std::vector<double>& distances,
    const std::vector<int>& parent)
{
    std::cout << "\nShortest paths from node " << source << ":\n";

    for (int i = 0; i < distances.size(); i++) {

        if (distances[i] == std::numeric_limits<double>::infinity()) {
            std::cout << "  to node " << i << ": unreachable\n";
            continue;
        }

        std::vector<int> path = reconstructPath(i, parent);

        std::cout << "  to node " << i << " ("
            << std::fixed << std::setprecision(2)
            << distances[i] << "): ";

        for (int j = 0; j < path.size(); j++) {
            std::cout << path[j];
            if (j < path.size() - 1) std::cout << " -> ";
        }
        std::cout << "\n";
    }
}

// -------------------------------------------------------------
// Original single-source Dijkstra (used by runAllPairs)
// -------------------------------------------------------------
std::vector<double> Dijkstra::runFromSource(int source, PriorityQueue& pq) {
    int numVertices = graph.getNumVertices();
    std::vector<double> distances(numVertices, std::numeric_limits<double>::infinity());
    std::vector<bool> visited(numVertices, false);

    int decreaseKeysThisRun = 0;
    int insertsThisRun = 0;
    int extractsThisRun = 0;

    distances[source] = 0.0;
    pq.insert(source, 0.0);
    insertsThisRun++;

    while (!pq.isEmpty()) {
        int current = pq.extractMin();
        extractsThisRun++;

        if (visited[current]) continue;
        visited[current] = true;

        const auto& neighbors = graph.getNeighbors(current);
        for (const auto& neighbor : neighbors) {
            int neighborNode = neighbor.first;
            double edgeWeight = neighbor.second;

            if (visited[neighborNode]) continue;

            double newDist = distances[current] + edgeWeight;

            if (newDist < distances[neighborNode]) {
                distances[neighborNode] = newDist;

                if (pq.contains(neighborNode)) {
                    pq.decreaseKey(neighborNode, newDist);
                    decreaseKeysThisRun++;
                }
                else {
                    pq.insert(neighborNode, newDist);
                    insertsThisRun++;
                }
            }
        }
    }

    if (source == 0) {
        std::cout << "DEBUG Dijkstra from " << source << ": inserts=" << insertsThisRun
            << ", extracts=" << extractsThisRun
            << ", decreaseKeys=" << decreaseKeysThisRun
            << ", totalOps=" << (insertsThisRun + extractsThisRun + decreaseKeysThisRun)
            << std::endl;
    }

    return distances;
}


// -------------------------------------------------------------
// All-pairs Dijkstra
// -------------------------------------------------------------
std::vector<std::vector<double>> Dijkstra::runAllPairs(PriorityQueue& pq) {
    int numVertices = graph.getNumVertices();
    std::vector<std::vector<double>> allDistances(numVertices);

    std::cout << "Running All-Pairs Dijkstra on " << numVertices << " vertices..." << std::endl;

    for (int source = 0; source < numVertices; source++) {
        allDistances[source] = runFromSource(source, pq);
    }

    return allDistances;
}



// -------------------------------------------------------------
// NEW: Single-source Dijkstra returning stats + parent array
// -------------------------------------------------------------
DijkstraResult Dijkstra::runFromSourceWithStats(int source, PriorityQueue& pq) {
    DijkstraResult result;
    int numVertices = graph.getNumVertices();

    std::vector<double> distances(numVertices, std::numeric_limits<double>::infinity());
    std::vector<bool> visited(numVertices, false);
    std::vector<int> parent(numVertices, -1);  // <-- Needed for path building

    distances[source] = 0.0;
    pq.insert(source, 0.0);
    result.insertCount++;

    while (!pq.isEmpty()) {
        int current = pq.extractMin();
        result.extractMinCount++;

        if (visited[current]) continue;
        visited[current] = true;
        result.nodesProcessed++;

        const auto& neighbors = graph.getNeighbors(current);
        for (const auto& neighbor : neighbors) {
            int neighborNode = neighbor.first;
            double edgeWeight = neighbor.second;

            if (visited[neighborNode]) continue;

            double newDist = distances[current] + edgeWeight;

            if (newDist < distances[neighborNode]) {
                distances[neighborNode] = newDist;
                parent[neighborNode] = current;  // <-- Track path
                result.relaxationCount++;

                if (pq.contains(neighborNode)) {
                    pq.decreaseKey(neighborNode, newDist);
                    result.decreaseKeyCount++;
                }
                else {
                    pq.insert(neighborNode, newDist);
                    result.insertCount++;
                }
            }
        }
    }

    result.distances = distances;
    result.parent = parent;  // <-- Return parents for printing full path

    return result;
}

// Add this function to your existing Dijkstra.cpp file
void Dijkstra::printDistances(const std::vector<double>& distances, int source) {
    std::cout << "\nShortest distances from node " << source << ":\n";
    for (int i = 0; i < distances.size(); i++) {
        if (distances[i] == std::numeric_limits<double>::infinity()) {
            std::cout << "  to node " << i << ": INF\n";
        } else {
            std::cout << "  to node " << i << ": " 
                      << std::fixed << std::setprecision(2) << distances[i] << "\n";
        }
    }
}



bool Dijkstra::verifyHeapConsistency(const Graph& graph, int testSource) {
    std::cout << "\n=== VERIFYING HEAP CONSISTENCY ===" << std::endl;
    std::cout << "Testing with source " << testSource << std::endl;

    if (testSource >= graph.getNumVertices()) {
        std::cout << "Invalid test source!" << std::endl;
        return false;
    }

    BinaryHeap binaryHeap;
    FibonacciHeap fibonacciHeap;
    HollowHeap hollowHeap;

    Dijkstra dijkstra(graph);

    // Run Dijkstra with each heap
    auto distBinary = dijkstra.runFromSource(testSource, binaryHeap);
    auto distFibonacci = dijkstra.runFromSource(testSource, fibonacciHeap);
    auto distHollow = dijkstra.runFromSource(testSource, hollowHeap);

    // Compare results
    bool allConsistent = true;
    const double tolerance = 1e-10;

    for (int i = 0; i < distBinary.size(); i++) {
        // Check if all three distances are effectively the same
        bool binaryVsFib = std::abs(distBinary[i] - distFibonacci[i]) < tolerance;
        bool binaryVsHollow = std::abs(distBinary[i] - distHollow[i]) < tolerance;

        // Handle infinity cases
        bool allInfinity = (distBinary[i] == std::numeric_limits<double>::infinity() &&
            distFibonacci[i] == std::numeric_limits<double>::infinity() &&
            distHollow[i] == std::numeric_limits<double>::infinity());

        if (!allInfinity && (!binaryVsFib || !binaryVsHollow)) {
            std::cout << "✗ INCONSISTENCY at node " << i << ":" << std::endl;
            std::cout << "  Binary:    " << distBinary[i] << std::endl;
            std::cout << "  Fibonacci: " << distFibonacci[i] << std::endl;
            std::cout << "  Hollow:    " << distHollow[i] << std::endl;
            allConsistent = false;
        }
    }

    if (allConsistent) {
        std::cout << "✓ ALL HEAPS PRODUCE IDENTICAL SHORTEST PATHS" << std::endl;

        // Print some sample distances for verification
        std::cout << "\nSample distances from source " << testSource << ":" << std::endl;
        int samples = std::min(5, (int)distBinary.size());
        for (int i = 0; i < samples; i++) {
            if (distBinary[i] < std::numeric_limits<double>::infinity()) {
                std::cout << "  to node " << i << ": " << distBinary[i] << std::endl;
            }
        }
    }
    else {
        std::cout << "✗ HEAPS PRODUCE DIFFERENT RESULTS!" << std::endl;
    }

    return allConsistent;
}

void Dijkstra::saveDistancesToFile(const std::vector<std::vector<double>>& allDistances,
    const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing!" << std::endl;
        return;
    }

    std::cout << "Saving distance matrix to: " << filename << std::endl;

    // Write header with dimensions
    int numVertices = allDistances.size();
    file << "# Shortest Path Distance Matrix" << std::endl;
    file << "# Format: source -> [distances to all targets]" << std::endl;
    file << "# Dimensions: " << numVertices << " x " << numVertices << std::endl;
    file << "# INF represents unreachable nodes" << std::endl;
    file << std::endl;

    // Write data in a readable format
    for (int source = 0; source < numVertices; source++) {
        file << "Source " << source << ": [";
        for (int target = 0; target < numVertices; target++) {
            if (allDistances[source][target] == std::numeric_limits<double>::infinity()) {
                file << "INF";
            }
            else {
                file << std::fixed << std::setprecision(6) << allDistances[source][target];
            }

            if (target < numVertices - 1) {
                file << ", ";
            }

            // Add line breaks for readability
            if (target > 0 && target % 10 == 0) {
                file << std::endl << "          ";
            }
        }
        file << "]" << std::endl << std::endl;
    }

    file.close();
    std::cout << "Distance matrix saved to: " << filename << std::endl;
}

// Save single-source distances to file
void Dijkstra::saveSingleSourceDistances(const std::vector<double>& distances,
    const std::string& filename, int source) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing!" << std::endl;
        return;
    }

    std::cout << "Saving distances from source " << source << " to: " << filename << std::endl;

    // Write header
    file << "# Shortest Path Distances from Source " << source << std::endl;
    file << "# Format: target_node distance" << std::endl;
    file << "# Total nodes: " << distances.size() << std::endl;
    file << "# INF represents unreachable nodes" << std::endl;
    file << std::endl;

    // Write data in target-distance pairs
    int reachableCount = 0;
    double maxDistance = 0;

    for (int target = 0; target < distances.size(); target++) {
        if (distances[target] < std::numeric_limits<double>::infinity()) {
            file << target << " " << std::fixed << std::setprecision(6) << distances[target] << std::endl;
            reachableCount++;
            maxDistance = std::max(maxDistance, distances[target]);
        }
        else {
            file << target << " INF" << std::endl;
        }
    }

    // Write summary
    file << std::endl;
    file << "# Summary:" << std::endl;
    file << "# Reachable nodes: " << reachableCount << "/" << distances.size() << std::endl;
    file << "# Maximum distance: " << std::fixed << std::setprecision(6) << maxDistance << std::endl;

    file.close();
    std::cout << "Distances from source " << source << " saved to: " << filename << std::endl;
    std::cout << "  Reachable: " << reachableCount << "/" << distances.size()
        << ", Max distance: " << maxDistance << std::endl;
}