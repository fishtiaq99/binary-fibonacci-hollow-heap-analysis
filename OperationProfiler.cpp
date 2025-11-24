#include "OperationProfiler.h"
#include "Graph.h"
#include "Dijkstra.h"
#include "Timer.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <algorithm>
#include <stdexcept>

// Operation sequence generator
std::vector<std::tuple<std::string, int, double>> generateOperationSequence(int numOperations) {
    std::vector<std::tuple<std::string, int, double>> operations;
    std::vector<int> activeNodes;
    int nextNodeId = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> opDist(0.0, 1.0);
    std::uniform_int_distribution<> keyDist(1, 10000);

    for (int i = 0; i < numOperations; ++i) {
        double randVal = opDist(gen);

        if (activeNodes.empty() || randVal < 0.5) {
            double key = keyDist(gen);
            operations.emplace_back("insert", nextNodeId, key);
            activeNodes.push_back(nextNodeId);
            nextNodeId++;
        }
        else if (randVal < 0.8) {
            operations.emplace_back("extract_min", -1, 0.0);
            if (!activeNodes.empty()) {
                activeNodes.erase(activeNodes.begin());
            }
        }
        else {
            if (!activeNodes.empty()) {
                int randomIndex = std::uniform_int_distribution<>(0, static_cast<int>(activeNodes.size()) - 1)(gen);
                int node = activeNodes[randomIndex];
                double newKey = keyDist(gen) / 2.0;
                operations.emplace_back("decrease_key", node, newKey);
            }
            else {
                double key = keyDist(gen);
                operations.emplace_back("insert", nextNodeId, key);
                activeNodes.push_back(nextNodeId);
                nextNodeId++;
            }
        }
    }

    return operations;
}

OperationProfile profileBinaryHeap(int numOperations) {
    OperationProfile profile;
    profile.heapType = "Binary Heap";

    BinaryHeap heap;
    auto operations = generateOperationSequence(numOperations);

    double totalInsertTime = 0.0;
    double totalExtractMinTime = 0.0;
    double totalDecreaseKeyTime = 0.0;
    int insertCount = 0;
    int extractMinCount = 0;
    int decreaseKeyCount = 0;

    int maxHeight = 0;

    Timer opTimer;
    Timer totalTimer;

    totalTimer.start();

    for (const auto& op : operations) {
        std::string opType = std::get<0>(op);
        int nodeId = std::get<1>(op);
        double key = std::get<2>(op);

        try {
            if (opType == "insert") {
                opTimer.start();
                heap.insert(nodeId, key);
                opTimer.stop();

                totalInsertTime += opTimer.elapsedMicroseconds();
                insertCount++;
            }
            else if (opType == "extract_min") {
                opTimer.start();
                int result = heap.extractMin();
                opTimer.stop();

                totalExtractMinTime += opTimer.elapsedMicroseconds();
                extractMinCount++;
            }
            else if (opType == "decrease_key") {
                opTimer.start();
                heap.decreaseKey(nodeId, key);
                opTimer.stop();

                totalDecreaseKeyTime += opTimer.elapsedMicroseconds();
                decreaseKeyCount++;
            }

            maxHeight = std::max(maxHeight, heap.getHeight());

        }
        catch (const std::exception& e) {
            continue;
        }
    }

    totalTimer.stop();

    profile.totalTimeMs = totalTimer.elapsedMilliseconds();
    profile.avgInsertTimeUs = insertCount > 0 ? totalInsertTime / insertCount : 0.0;
    profile.avgExtractMinTimeUs = extractMinCount > 0 ? totalExtractMinTime / extractMinCount : 0.0;
    profile.avgDecreaseKeyTimeUs = decreaseKeyCount > 0 ? totalDecreaseKeyTime / decreaseKeyCount : 0.0;
    profile.insertCount = insertCount;
    profile.extractMinCount = extractMinCount;
    profile.decreaseKeyCount = decreaseKeyCount;
    profile.maxHeight = maxHeight;
    profile.maxTrees = heap.getNumTrees();
    profile.maxHollowNodes = 0;
    profile.cascadingCuts = 0;
    profile.memoryUsageMB = heap.estimateMemoryUsage();

    return profile;
}

OperationProfile profileFibonacciHeap(int numOperations) {
    OperationProfile profile;
    profile.heapType = "Fibonacci Heap";

    FibonacciHeap heap;
    auto operations = generateOperationSequence(numOperations);

    double totalInsertTime = 0.0;
    double totalExtractMinTime = 0.0;
    double totalDecreaseKeyTime = 0.0;
    int insertCount = 0;
    int extractMinCount = 0;
    int decreaseKeyCount = 0;

    int maxHeight = 0;
    int maxTrees = 0;
    long cascadingCuts = 0;

    Timer opTimer;
    Timer totalTimer;

    totalTimer.start();

    for (const auto& op : operations) {
        std::string opType = std::get<0>(op);
        int nodeId = std::get<1>(op);
        double key = std::get<2>(op);

        try {
            if (opType == "insert") {
                opTimer.start();
                heap.insert(nodeId, key);
                opTimer.stop();

                totalInsertTime += opTimer.elapsedMicroseconds();
                insertCount++;
            }
            else if (opType == "extract_min") {
                opTimer.start();
                int result = heap.extractMin();
                opTimer.stop();

                totalExtractMinTime += opTimer.elapsedMicroseconds();
                extractMinCount++;
            }
            else if (opType == "decrease_key") {
                opTimer.start();
                heap.decreaseKey(nodeId, key);
                opTimer.stop();

                totalDecreaseKeyTime += opTimer.elapsedMicroseconds();
                decreaseKeyCount++;
            }

            maxHeight = std::max(maxHeight, heap.getHeight());
            maxTrees = std::max(maxTrees, heap.getNumTrees());
            cascadingCuts = std::max(cascadingCuts, heap.getCascadingCutCount());

        }
        catch (const std::exception& e) {
            continue;
        }
    }

    totalTimer.stop();

    profile.totalTimeMs = totalTimer.elapsedMilliseconds();
    profile.avgInsertTimeUs = insertCount > 0 ? totalInsertTime / insertCount : 0.0;
    profile.avgExtractMinTimeUs = extractMinCount > 0 ? totalExtractMinTime / extractMinCount : 0.0;
    profile.avgDecreaseKeyTimeUs = decreaseKeyCount > 0 ? totalDecreaseKeyTime / decreaseKeyCount : 0.0;
    profile.insertCount = insertCount;
    profile.extractMinCount = extractMinCount;
    profile.decreaseKeyCount = decreaseKeyCount;
    profile.maxHeight = maxHeight;
    profile.maxTrees = maxTrees;
    profile.maxHollowNodes = 0;
    profile.cascadingCuts = cascadingCuts;
    profile.memoryUsageMB = heap.estimateMemoryUsage();

    return profile;
}

OperationProfile profileHollowHeap(int numOperations) {
    OperationProfile profile;
    profile.heapType = "Hollow Heap";

    HollowHeap heap;
    auto operations = generateOperationSequence(numOperations);

    double totalInsertTime = 0.0;
    double totalExtractMinTime = 0.0;
    double totalDecreaseKeyTime = 0.0;
    int insertCount = 0;
    int extractMinCount = 0;
    int decreaseKeyCount = 0;

    int maxHeight = 0;
    int maxTrees = 0;
    int maxHollowNodes = 0;

    Timer opTimer;
    Timer totalTimer;

    totalTimer.start();

    for (const auto& op : operations) {
        std::string opType = std::get<0>(op);
        int nodeId = std::get<1>(op);
        double key = std::get<2>(op);

        try {
            if (opType == "insert") {
                opTimer.start();
                heap.insert(nodeId, key);
                opTimer.stop();

                totalInsertTime += opTimer.elapsedMicroseconds();
                insertCount++;
            }
            else if (opType == "extract_min") {
                opTimer.start();
                int result = heap.extractMin();
                opTimer.stop();

                totalExtractMinTime += opTimer.elapsedMicroseconds();
                extractMinCount++;
            }
            else if (opType == "decrease_key") {
                opTimer.start();
                heap.decreaseKey(nodeId, key);
                opTimer.stop();

                totalDecreaseKeyTime += opTimer.elapsedMicroseconds();
                decreaseKeyCount++;
            }

            maxHeight = std::max(maxHeight, heap.getHeight());
            maxTrees = std::max(maxTrees, heap.getNumTrees());
            maxHollowNodes = std::max(maxHollowNodes, heap.getHollowCount());

        }
        catch (const std::exception& e) {
            continue;
        }
    }

    totalTimer.stop();

    profile.totalTimeMs = totalTimer.elapsedMilliseconds();
    profile.avgInsertTimeUs = insertCount > 0 ? totalInsertTime / insertCount : 0.0;
    profile.avgExtractMinTimeUs = extractMinCount > 0 ? totalExtractMinTime / extractMinCount : 0.0;
    profile.avgDecreaseKeyTimeUs = decreaseKeyCount > 0 ? totalDecreaseKeyTime / decreaseKeyCount : 0.0;
    profile.insertCount = insertCount;
    profile.extractMinCount = extractMinCount;
    profile.decreaseKeyCount = decreaseKeyCount;
    profile.maxHeight = maxHeight;
    profile.maxTrees = maxTrees;
    profile.maxHollowNodes = maxHollowNodes;
    profile.cascadingCuts = 0;
    profile.memoryUsageMB = heap.estimateMemoryUsage();

    return profile;
}

std::vector<OperationProfile> OperationProfiler::runProfilingExperiment() {
    std::vector<OperationProfile> results;

    std::cout << "\n=== Operation Profiling Experiment ===" << std::endl;
    std::cout << "Generating " << numOperations << " random heap operations..." << std::endl;

    std::cout << "\n--- Profiling Binary Heap ---" << std::endl;
    results.push_back(profileBinaryHeap(numOperations));

    std::cout << "--- Profiling Fibonacci Heap ---" << std::endl;
    results.push_back(profileFibonacciHeap(numOperations));

    std::cout << "--- Profiling Hollow Heap ---" << std::endl;
    results.push_back(profileHollowHeap(numOperations));

    return results;
}

void OperationProfiler::printResults(const std::vector<OperationProfile>& results) {
    std::cout << "\n" << std::string(120, '=') << std::endl;
    std::cout << "OPERATION PROFILING RESULTS - " << numOperations << " RANDOM OPERATIONS" << std::endl;
    std::cout << std::string(120, '=') << std::endl;

    std::cout << std::left
        << std::setw(15) << "Heap Type"
        << std::setw(15) << "Total Time (ms)"
        << std::setw(12) << "Insert (탎)"
        << std::setw(15) << "Extract-Min (탎)"
        << std::setw(15) << "Decrease-Key (탎)"
        << std::setw(10) << "Height"
        << std::setw(8) << "#Trees"
        << std::setw(12) << "Op Counts"
        << std::endl;

    std::cout << std::string(120, '-') << std::endl;

    for (const auto& result : results) {
        std::string opCounts = "I:" + std::to_string(result.insertCount) +
            " E:" + std::to_string(result.extractMinCount) +
            " D:" + std::to_string(result.decreaseKeyCount);

        std::cout << std::left
            << std::setw(15) << result.heapType
            << std::setw(15) << std::fixed << std::setprecision(2) << result.totalTimeMs
            << std::setw(12) << std::setprecision(2) << result.avgInsertTimeUs
            << std::setw(15) << result.avgExtractMinTimeUs
            << std::setw(15) << result.avgDecreaseKeyTimeUs
            << std::setw(10) << result.maxHeight
            << std::setw(8) << result.maxTrees
            << std::setw(12) << opCounts
            << std::endl;
    }

    std::cout << "\nSpecial Statistics:" << std::endl;
    for (const auto& result : results) {
        if (result.heapType == "Fibonacci Heap") {
            std::cout << "  Fibonacci Heap - Cascading Cuts: " << result.cascadingCuts << std::endl;
        }
        if (result.heapType == "Hollow Heap") {
            std::cout << "  Hollow Heap - Max Hollow Nodes: " << result.maxHollowNodes << std::endl;
        }
    }

    std::cout << std::string(120, '=') << std::endl;
}

void OperationProfiler::saveResultsToFile(const std::vector<OperationProfile>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing!" << std::endl;
        return;
    }

    file << "OPERATION PROFILING RESULTS - " << numOperations << " RANDOM OPERATIONS\n";
    file << "======================================================================\n\n";

    file << std::left
        << std::setw(15) << "Heap Type"
        << std::setw(15) << "Total Time (ms)"
        << std::setw(12) << "Insert (탎)"
        << std::setw(15) << "Extract-Min (탎)"
        << std::setw(15) << "Decrease-Key (탎)"
        << std::setw(10) << "Height"
        << std::setw(8) << "#Trees"
        << std::setw(8) << "Memory (MB)"
        << "\n";

    file << std::string(100, '-') << "\n";

    for (const auto& result : results) {
        file << std::left
            << std::setw(15) << result.heapType
            << std::setw(15) << std::fixed << std::setprecision(2) << result.totalTimeMs
            << std::setw(12) << std::setprecision(2) << result.avgInsertTimeUs
            << std::setw(15) << result.avgExtractMinTimeUs
            << std::setw(15) << result.avgDecreaseKeyTimeUs
            << std::setw(10) << result.maxHeight
            << std::setw(8) << result.maxTrees
            << std::setw(8) << std::setprecision(3) << result.memoryUsageMB
            << "\n";
    }

    file << "\nOperation Counts:\n";
    for (const auto& result : results) {
        file << result.heapType << ": "
            << "Insert=" << result.insertCount
            << ", ExtractMin=" << result.extractMinCount
            << ", DecreaseKey=" << result.decreaseKeyCount
            << "\n";
    }

    file.close();
    std::cout << "Results saved to: " << filename << std::endl;
}

// ==================== GRAPH PROFILING IMPLEMENTATION ====================

// Instrumented Dijkstra that tracks detailed timing
struct InstrumentedDijkstraResult {
    double totalTimeMs = 0;
    double insertTimeUs = 0;
    double extractTimeUs = 0;
    double decreaseTimeUs = 0;
    int insertCount = 0;
    int extractCount = 0;
    int decreaseCount = 0;
    int maxHeight = 0;
    int maxTrees = 0;
};

InstrumentedDijkstraResult runInstrumentedDijkstra(const Graph& graph, int source, PriorityQueue& pq) {
    InstrumentedDijkstraResult result;
    int numVertices = graph.getNumVertices();
    std::vector<double> dist(numVertices, std::numeric_limits<double>::infinity());
    std::vector<bool> visited(numVertices, false);

    dist[source] = 0.0;

    Timer totalTimer, opTimer;
    totalTimer.start();

    try {
        opTimer.start();
        pq.insert(source, 0.0);
        opTimer.stop();
        result.insertTimeUs += opTimer.elapsedMicroseconds();
        result.insertCount++;
    }
    catch (const std::exception& e) {
        return result;
    }

    while (!pq.isEmpty()) {
        int u;
        try {
            opTimer.start();
            u = pq.extractMin();
            opTimer.stop();
            result.extractTimeUs += opTimer.elapsedMicroseconds();
            result.extractCount++;
        }
        catch (const std::exception& e) {
            break;
        }

        if (visited[u]) continue;
        visited[u] = true;

        // Update heap statistics
        result.maxHeight = std::max(result.maxHeight, pq.getHeight());
        result.maxTrees = std::max(result.maxTrees, pq.getNumTrees());

        const auto& neighbors = graph.getNeighbors(u);
        for (const auto& neighbor : neighbors) {
            int v = neighbor.first;
            double weight = neighbor.second;

            if (visited[v]) continue;

            double new_dist = dist[u] + weight;

            if (new_dist < dist[v]) {
                dist[v] = new_dist;

                try {
                    if (pq.contains(v)) {
                        opTimer.start();
                        pq.decreaseKey(v, new_dist);
                        opTimer.stop();
                        result.decreaseTimeUs += opTimer.elapsedMicroseconds();
                        result.decreaseCount++;
                    }
                    else {
                        opTimer.start();
                        pq.insert(v, new_dist);
                        opTimer.stop();
                        result.insertTimeUs += opTimer.elapsedMicroseconds();
                        result.insertCount++;
                    }
                }
                catch (const std::exception& e) {
                    // Continue despite errors
                }
            }
        }
    }

    totalTimer.stop();
    result.totalTimeMs = totalTimer.elapsedMilliseconds();

    return result;
}

std::vector<GraphProfile> OperationProfiler::runGraphProfilingExperiment(const Graph& graph, int numSources) {
    std::vector<GraphProfile> results;
    int totalVertices = graph.getNumVertices();

    // Limit numSources to available vertices
    numSources = std::min(numSources, totalVertices);

    std::cout << "\n=== Graph-Based Profiling Experiment ===" << std::endl;
    std::cout << "Graph: " << totalVertices << " vertices, " << graph.getNumEdges() << " edges" << std::endl;
    std::cout << "Testing with " << numSources << " random sources" << std::endl;

    // Test Binary Heap
    std::cout << "\n--- Profiling Binary Heap on Graph ---" << std::endl;
    {
        GraphProfile profile;
        profile.heapType = "Binary Heap";
        profile.graphName = "Road Network";
        profile.vertices = totalVertices;
        profile.edges = graph.getNumEdges();
        profile.sourcesProcessed = 0;

        double totalInsertTime = 0, totalExtractTime = 0, totalDecreaseTime = 0;
        int totalInserts = 0, totalExtracts = 0, totalDecreases = 0;
        int maxHeight = 0, maxTrees = 0;
        double totalTime = 0;

        for (int i = 0; i < numSources; i++) {
            int source = rand() % totalVertices;  // Random source
            BinaryHeap heap;

            auto result = runInstrumentedDijkstra(graph, source, heap);

            totalInsertTime += result.insertTimeUs;
            totalExtractTime += result.extractTimeUs;
            totalDecreaseTime += result.decreaseTimeUs;
            totalInserts += result.insertCount;
            totalExtracts += result.extractCount;
            totalDecreases += result.decreaseCount;
            maxHeight = std::max(maxHeight, result.maxHeight);
            maxTrees = std::max(maxTrees, result.maxTrees);
            totalTime += result.totalTimeMs;
            profile.sourcesProcessed++;

            if ((i + 1) % 10 == 0) {
                std::cout << "  Processed " << (i + 1) << "/" << numSources << " sources" << std::endl;
            }
        }

        profile.totalTimeMs = totalTime;
        profile.avgInsertTimeUs = totalInserts > 0 ? totalInsertTime / totalInserts : 0;
        profile.avgExtractMinTimeUs = totalExtracts > 0 ? totalExtractTime / totalExtracts : 0;
        profile.avgDecreaseKeyTimeUs = totalDecreases > 0 ? totalDecreaseTime / totalDecreases : 0;
        profile.insertCount = totalInserts;
        profile.extractMinCount = totalExtracts;
        profile.decreaseKeyCount = totalDecreases;
        profile.maxHeight = maxHeight;
        profile.maxTrees = maxTrees;

        // Estimate memory for one heap instance
        BinaryHeap sampleHeap;
        profile.memoryUsageMB = sampleHeap.estimateMemoryUsage();

        results.push_back(profile);
    }

    // Test Fibonacci Heap
    std::cout << "\n--- Profiling Fibonacci Heap on Graph ---" << std::endl;
    {
        GraphProfile profile;
        profile.heapType = "Fibonacci Heap";
        profile.graphName = "Road Network";
        profile.vertices = totalVertices;
        profile.edges = graph.getNumEdges();
        profile.sourcesProcessed = 0;

        double totalInsertTime = 0, totalExtractTime = 0, totalDecreaseTime = 0;
        int totalInserts = 0, totalExtracts = 0, totalDecreases = 0;
        int maxHeight = 0, maxTrees = 0;
        double totalTime = 0;

        for (int i = 0; i < numSources; i++) {
            int source = rand() % totalVertices;
            FibonacciHeap heap;

            auto result = runInstrumentedDijkstra(graph, source, heap);

            totalInsertTime += result.insertTimeUs;
            totalExtractTime += result.extractTimeUs;
            totalDecreaseTime += result.decreaseTimeUs;
            totalInserts += result.insertCount;
            totalExtracts += result.extractCount;
            totalDecreases += result.decreaseCount;
            maxHeight = std::max(maxHeight, result.maxHeight);
            maxTrees = std::max(maxTrees, result.maxTrees);
            totalTime += result.totalTimeMs;
            profile.sourcesProcessed++;

            if ((i + 1) % 10 == 0) {
                std::cout << "  Processed " << (i + 1) << "/" << numSources << " sources" << std::endl;
            }
        }

        profile.totalTimeMs = totalTime;
        profile.avgInsertTimeUs = totalInserts > 0 ? totalInsertTime / totalInserts : 0;
        profile.avgExtractMinTimeUs = totalExtracts > 0 ? totalExtractTime / totalExtracts : 0;
        profile.avgDecreaseKeyTimeUs = totalDecreases > 0 ? totalDecreaseTime / totalDecreases : 0;
        profile.insertCount = totalInserts;
        profile.extractMinCount = totalExtracts;
        profile.decreaseKeyCount = totalDecreases;
        profile.maxHeight = maxHeight;
        profile.maxTrees = maxTrees;

        FibonacciHeap sampleHeap;
        profile.memoryUsageMB = sampleHeap.estimateMemoryUsage();

        results.push_back(profile);
    }

    // Test Hollow Heap
    std::cout << "\n--- Profiling Hollow Heap on Graph ---" << std::endl;
    {
        GraphProfile profile;
        profile.heapType = "Hollow Heap";
        profile.graphName = "Road Network";
        profile.vertices = totalVertices;
        profile.edges = graph.getNumEdges();
        profile.sourcesProcessed = 0;

        double totalInsertTime = 0, totalExtractTime = 0, totalDecreaseTime = 0;
        int totalInserts = 0, totalExtracts = 0, totalDecreases = 0;
        int maxHeight = 0, maxTrees = 0;
        double totalTime = 0;

        for (int i = 0; i < numSources; i++) {
            int source = rand() % totalVertices;
            HollowHeap heap;

            auto result = runInstrumentedDijkstra(graph, source, heap);

            totalInsertTime += result.insertTimeUs;
            totalExtractTime += result.extractTimeUs;
            totalDecreaseTime += result.decreaseTimeUs;
            totalInserts += result.insertCount;
            totalExtracts += result.extractCount;
            totalDecreases += result.decreaseCount;
            maxHeight = std::max(maxHeight, result.maxHeight);
            maxTrees = std::max(maxTrees, result.maxTrees);
            totalTime += result.totalTimeMs;
            profile.sourcesProcessed++;

            if ((i + 1) % 10 == 0) {
                std::cout << "  Processed " << (i + 1) << "/" << numSources << " sources" << std::endl;
            }
        }

        profile.totalTimeMs = totalTime;
        profile.avgInsertTimeUs = totalInserts > 0 ? totalInsertTime / totalInserts : 0;
        profile.avgExtractMinTimeUs = totalExtracts > 0 ? totalExtractTime / totalExtracts : 0;
        profile.avgDecreaseKeyTimeUs = totalDecreases > 0 ? totalDecreaseTime / totalDecreases : 0;
        profile.insertCount = totalInserts;
        profile.extractMinCount = totalExtracts;
        profile.decreaseKeyCount = totalDecreases;
        profile.maxHeight = maxHeight;
        profile.maxTrees = maxTrees;

        HollowHeap sampleHeap;
        profile.memoryUsageMB = sampleHeap.estimateMemoryUsage();

        results.push_back(profile);
    }

    return results;
}

void OperationProfiler::printGraphResults(const std::vector<GraphProfile>& results) {
    std::cout << "\n" << std::string(140, '=') << std::endl;
    std::cout << "GRAPH-BASED PROFILING RESULTS" << std::endl;
    std::cout << std::string(140, '=') << std::endl;

    std::cout << std::left
        << std::setw(15) << "Heap Type"
        << std::setw(10) << "Vertices"
        << std::setw(8) << "Edges"
        << std::setw(12) << "Sources"
        << std::setw(15) << "Total Time (ms)"
        << std::setw(12) << "Insert (탎)"
        << std::setw(15) << "Extract-Min (탎)"
        << std::setw(15) << "Decrease-Key (탎)"
        << std::setw(8) << "Height"
        << std::setw(8) << "#Trees"
        << std::setw(10) << "Memory (MB)"
        << std::endl;

    std::cout << std::string(140, '-') << std::endl;

    for (const auto& result : results) {
        std::cout << std::left
            << std::setw(15) << result.heapType
            << std::setw(10) << result.vertices
            << std::setw(8) << result.edges
            << std::setw(12) << result.sourcesProcessed
            << std::setw(15) << std::fixed << std::setprecision(2) << result.totalTimeMs
            << std::setw(12) << std::setprecision(2) << result.avgInsertTimeUs
            << std::setw(15) << result.avgExtractMinTimeUs
            << std::setw(15) << result.avgDecreaseKeyTimeUs
            << std::setw(8) << result.maxHeight
            << std::setw(8) << result.maxTrees
            << std::setw(10) << std::setprecision(3) << result.memoryUsageMB
            << std::endl;
    }

    std::cout << "\nOperation Counts Summary:" << std::endl;
    for (const auto& result : results) {
        std::cout << "  " << result.heapType << ": "
            << "Insert=" << result.insertCount
            << ", ExtractMin=" << result.extractMinCount
            << ", DecreaseKey=" << result.decreaseKeyCount
            << std::endl;
    }

    std::cout << std::string(140, '=') << std::endl;
}

void OperationProfiler::saveGraphResultsToFile(const std::vector<GraphProfile>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing!" << std::endl;
        return;
    }

    file << "GRAPH-BASED PROFILING RESULTS\n";
    file << "=============================\n\n";

    for (const auto& result : results) {
        file << "Heap Type: " << result.heapType << "\n";
        file << "Graph: " << result.graphName << " (" << result.vertices << " vertices, " << result.edges << " edges)\n";
        file << "Sources Processed: " << result.sourcesProcessed << "\n";
        file << "Total Time: " << std::fixed << std::setprecision(2) << result.totalTimeMs << " ms\n";
        file << "Average Insert Time: " << result.avgInsertTimeUs << " 탎\n";
        file << "Average Extract-Min Time: " << result.avgExtractMinTimeUs << " 탎\n";
        file << "Average Decrease-Key Time: " << result.avgDecreaseKeyTimeUs << " 탎\n";
        file << "Max Height: " << result.maxHeight << "\n";
        file << "Max Trees: " << result.maxTrees << "\n";
        file << "Memory Usage: " << std::setprecision(3) << result.memoryUsageMB << " MB\n";
        file << "Operation Counts - Insert: " << result.insertCount
            << ", Extract-Min: " << result.extractMinCount
            << ", Decrease-Key: " << result.decreaseKeyCount << "\n";
        file << "----------------------------------------\n\n";
    }

    file.close();
    std::cout << "Graph profiling results saved to: " << filename << std::endl;
}