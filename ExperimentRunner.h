#ifndef EXPERIMENTRUNNER_H
#define EXPERIMENTRUNNER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

// Forward declarations
class Graph;
class PriorityQueue;
class BinaryHeap;
class FibonacciHeap;
class HollowHeap;

struct ExperimentResults {
    std::string heapType;
    int numNodes;
    int numEdges;

    // Timing results
    double totalRuntimeMs;
    double avgInsertTimeUs;
    double avgExtractMinTimeUs;
    double avgDecreaseKeyTimeUs;

    // Heap statistics
    int heapHeight;
    int numTrees;
    long cascadingCutCount;  // Fibonacci only
    int hollowCount;        // Hollow only
    double memoryUsageMB;

    // Operation counts
    long insertCount;
    long extractMinCount;
    long decreaseKeyCount;
};

class ExperimentRunner {
private:
    Graph& graph;

public:
    ExperimentRunner(Graph& g) : graph(g) {}

    // Run experiments with all three heaps
    ExperimentResults runWithBinaryHeap();
    ExperimentResults runWithFibonacciHeap();
    ExperimentResults runWithHollowHeap();

    // Run all experiments and return comprehensive results
    std::vector<ExperimentResults> runAllExperiments();

    // Save results to file
    static void saveResultsToFile(const std::vector<ExperimentResults>& results,
        const std::string& filename);

    // Print results table
    static void printResultsTable(const std::vector<ExperimentResults>& results);

    // Generate comparison table in the exact format from the project PDF
    static void printProjectFormatTable(const std::vector<ExperimentResults>& results);
    static bool verifyAllHeaps(const Graph& graph, int numTestSources = 3);

    // Distance saving methods
    void saveBinaryHeapDistances(const std::string& filename);
    void saveFibonacciHeapDistances(const std::string& filename);
    void saveHollowHeapDistances(const std::string& filename);
    void saveAllHeapDistances();

    // Visualization methods
    void visualizeHeapStructures(int source = 0);
    void captureHeapEvolution(int source = 0, int max_steps = 50);
};

#endif // EXPERIMENTRUNNER_H