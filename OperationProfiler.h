#pragma once
#ifndef OPERATIONPROFILER_H
#define OPERATIONPROFILER_H

#include "PriorityQueue.h"
#include "BinaryHeap.h"
#include "FibonacciHeap.h"
#include "HollowHeap.h"
#include "Timer.h"
#include "Graph.h"
#include "Dijkstra.h"
#include <vector>
#include <string>

struct OperationProfile {
    std::string heapType;
    double totalTimeMs;
    double avgInsertTimeUs;
    double avgExtractMinTimeUs;
    double avgDecreaseKeyTimeUs;
    int insertCount;
    int extractMinCount;
    int decreaseKeyCount;
    int maxHeight;
    int maxTrees;
    int maxHollowNodes;
    long cascadingCuts;
    double memoryUsageMB;
};

struct GraphProfile {
    std::string heapType;
    std::string graphName;
    int vertices;
    int edges;
    double totalTimeMs;
    double avgInsertTimeUs;
    double avgExtractMinTimeUs;
    double avgDecreaseKeyTimeUs;
    int insertCount;
    int extractMinCount;
    int decreaseKeyCount;
    int maxHeight;
    int maxTrees;
    double memoryUsageMB;
    int sourcesProcessed;
};

class OperationProfiler {
private:
    int numOperations;

public:
    OperationProfiler(int numOps = 100000) : numOperations(numOps) {}

    std::vector<OperationProfile> runProfilingExperiment();
    void printResults(const std::vector<OperationProfile>& results);
    void saveResultsToFile(const std::vector<OperationProfile>& results, const std::string& filename);

    // NEW: Graph-based profiling methods
    std::vector<GraphProfile> runGraphProfilingExperiment(const Graph& graph, int numSources = 10);
    void printGraphResults(const std::vector<GraphProfile>& results);
    void saveGraphResultsToFile(const std::vector<GraphProfile>& results, const std::string& filename);
};

#endif