#include "ExperimentRunner.h"
#include "Graph.h"
#include "Dijkstra.h"
#include "BinaryHeap.h"
#include "FibonacciHeap.h"
#include "HollowHeap.h"
#include "Timer.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <algorithm>
#include <memory>
#include <cmath>

// Forward declarations for the instrumented heap classes
class InstrumentedBinaryHeap;
class InstrumentedFibonacciHeap;
class InstrumentedHollowHeap;

// Instrumented Binary Heap
class InstrumentedBinaryHeap : public BinaryHeap {
private:
    mutable long insertCounter = 0;
    mutable long extractMinCounter = 0;
    mutable long decreaseKeyCounter = 0;

    mutable double totalInsertTime = 0.0;
    mutable double totalExtractMinTime = 0.0;
    mutable double totalDecreaseKeyTime = 0.0;

    mutable int maxHeightDuringOps = 0;

    Timer timer;

    void updateMaxHeight() {
        int currentHeight = BinaryHeap::getHeight();
        if (currentHeight > maxHeightDuringOps) {
            maxHeightDuringOps = currentHeight;
        }
    }

public:
    void insert(int node, double priority) override {
        timer.start();
        BinaryHeap::insert(node, priority);
        timer.stop();

        updateMaxHeight();
        totalInsertTime += timer.elapsedMicroseconds();
        insertCounter++;
    }

    int extractMin() override {
        timer.start();
        int result = BinaryHeap::extractMin();
        timer.stop();

        updateMaxHeight();
        totalExtractMinTime += timer.elapsedMicroseconds();
        extractMinCounter++;
        return result;
    }

    void decreaseKey(int node, double newPriority) override {
        timer.start();
        BinaryHeap::decreaseKey(node, newPriority);
        timer.stop();

        updateMaxHeight();
        totalDecreaseKeyTime += timer.elapsedMicroseconds();
        decreaseKeyCounter++;
    }

    void clear() override {
        // Reset all counters
        insertCounter = 0;
        extractMinCounter = 0;
        decreaseKeyCounter = 0;
        totalInsertTime = 0.0;
        totalExtractMinTime = 0.0;
        totalDecreaseKeyTime = 0.0;
        maxHeightDuringOps = 0;

        // Clear the actual heap data
        BinaryHeap::clear();
    }

    // Getters for statistics
    long getInsertCount() const { return insertCounter; }
    long getExtractMinCount() const { return extractMinCounter; }
    long getDecreaseKeyCount() const { return decreaseKeyCounter; }

    double getAvgInsertTime() const {
        return insertCounter > 0 ? totalInsertTime / insertCounter : 0.0;
    }

    double getAvgExtractMinTime() const {
        return extractMinCounter > 0 ? totalExtractMinTime / extractMinCounter : 0.0;
    }

    double getAvgDecreaseKeyTime() const {
        return decreaseKeyCounter > 0 ? totalDecreaseKeyTime / decreaseKeyCounter : 0.0;
    }

    int getMaxHeightDuringOps() const { return maxHeightDuringOps; }
};

// Instrumented Fibonacci Heap
class InstrumentedFibonacciHeap : public FibonacciHeap {
private:
    mutable long insertCounter = 0;
    mutable long extractMinCounter = 0;
    mutable long decreaseKeyCounter = 0;

    mutable double totalInsertTime = 0.0;
    mutable double totalExtractMinTime = 0.0;
    mutable double totalDecreaseKeyTime = 0.0;

    mutable int maxHeightDuringOps = 0;
    mutable int maxTreesDuringOps = 0;

    Timer timer;

    void updateMaxStats() {
        int currentHeight = FibonacciHeap::getHeight();
        if (currentHeight > maxHeightDuringOps) {
            maxHeightDuringOps = currentHeight;
        }

        int currentTrees = FibonacciHeap::getNumTrees();
        if (currentTrees > maxTreesDuringOps) {
            maxTreesDuringOps = currentTrees;
        }
    }

public:
    void insert(int node, double priority) override {
        timer.start();
        FibonacciHeap::insert(node, priority);
        timer.stop();

        updateMaxStats();
        totalInsertTime += timer.elapsedMicroseconds();
        insertCounter++;
    }

    int extractMin() override {
        timer.start();
        int result = FibonacciHeap::extractMin();
        timer.stop();

        updateMaxStats();
        totalExtractMinTime += timer.elapsedMicroseconds();
        extractMinCounter++;
        return result;
    }

    void decreaseKey(int node, double newPriority) override {
        timer.start();
        FibonacciHeap::decreaseKey(node, newPriority);
        timer.stop();

        updateMaxStats();
        totalDecreaseKeyTime += timer.elapsedMicroseconds();
        decreaseKeyCounter++;
    }

    void clear() override {
        // Reset all counters
        insertCounter = 0;
        extractMinCounter = 0;
        decreaseKeyCounter = 0;
        totalInsertTime = 0.0;
        totalExtractMinTime = 0.0;
        totalDecreaseKeyTime = 0.0;
        maxHeightDuringOps = 0;
        maxTreesDuringOps = 0;

        // Clear the actual heap data
        FibonacciHeap::clear();
    }

    // Getters for statistics
    long getInsertCount() const { return insertCounter; }
    long getExtractMinCount() const { return extractMinCounter; }
    long getDecreaseKeyCount() const { return decreaseKeyCounter; }

    double getAvgInsertTime() const {
        return insertCounter > 0 ? totalInsertTime / insertCounter : 0.0;
    }

    double getAvgExtractMinTime() const {
        return extractMinCounter > 0 ? totalExtractMinTime / extractMinCounter : 0.0;
    }

    double getAvgDecreaseKeyTime() const {
        return decreaseKeyCounter > 0 ? totalDecreaseKeyTime / decreaseKeyCounter : 0.0;
    }

    int getMaxHeightDuringOps() const { return maxHeightDuringOps; }
    int getMaxTreesDuringOps() const { return maxTreesDuringOps; }
};

// Instrumented Hollow Heap
class InstrumentedHollowHeap : public HollowHeap {
private:
    mutable long insertCounter = 0;
    mutable long extractMinCounter = 0;
    mutable long decreaseKeyCounter = 0;

    mutable double totalInsertTime = 0.0;
    mutable double totalExtractMinTime = 0.0;
    mutable double totalDecreaseKeyTime = 0.0;

    mutable int maxHeightDuringOps = 0;
    mutable int maxTreesDuringOps = 0;
    mutable int maxHollowNodesDuringOps = 0;

    Timer timer;

    void updateMaxStats() {
        int currentHeight = HollowHeap::getHeight();
        if (currentHeight > maxHeightDuringOps) {
            maxHeightDuringOps = currentHeight;
        }

        int currentTrees = HollowHeap::getNumTrees();
        if (currentTrees > maxTreesDuringOps) {
            maxTreesDuringOps = currentTrees;
        }

        int currentHollow = HollowHeap::getHollowCount();
        if (currentHollow > maxHollowNodesDuringOps) {
            maxHollowNodesDuringOps = currentHollow;
        }
    }

public:
    void insert(int node, double priority) override {
        timer.start();
        HollowHeap::insert(node, priority);
        timer.stop();

        updateMaxStats();
        totalInsertTime += timer.elapsedMicroseconds();
        insertCounter++;
    }

    int extractMin() override {
        timer.start();
        int result = HollowHeap::extractMin();
        timer.stop();

        updateMaxStats();
        totalExtractMinTime += timer.elapsedMicroseconds();
        extractMinCounter++;
        return result;
    }

    void decreaseKey(int node, double newPriority) override {
        timer.start();
        HollowHeap::decreaseKey(node, newPriority);
        timer.stop();

        updateMaxStats();
        totalDecreaseKeyTime += timer.elapsedMicroseconds();
        decreaseKeyCounter++;
    }

    void clear() override {
        // Reset all counters
        insertCounter = 0;
        extractMinCounter = 0;
        decreaseKeyCounter = 0;
        totalInsertTime = 0.0;
        totalExtractMinTime = 0.0;
        totalDecreaseKeyTime = 0.0;
        maxHeightDuringOps = 0;
        maxTreesDuringOps = 0;
        maxHollowNodesDuringOps = 0;

        // Clear the actual heap data
        HollowHeap::clear();
    }

    // Getters for statistics
    long getInsertCount() const { return insertCounter; }
    long getExtractMinCount() const { return extractMinCounter; }
    long getDecreaseKeyCount() const { return decreaseKeyCounter; }

    double getAvgInsertTime() const {
        return insertCounter > 0 ? totalInsertTime / insertCounter : 0.0;
    }

    double getAvgExtractMinTime() const {
        return extractMinCounter > 0 ? totalExtractMinTime / extractMinCounter : 0.0;
    }

    double getAvgDecreaseKeyTime() const {
        return decreaseKeyCounter > 0 ? totalDecreaseKeyTime / decreaseKeyCounter : 0.0;
    }

    int getMaxHeightDuringOps() const { return maxHeightDuringOps; }
    int getMaxTreesDuringOps() const { return maxTreesDuringOps; }
    int getMaxHollowNodesDuringOps() const { return maxHollowNodesDuringOps; }
};

// Implementation of ExperimentRunner methods
ExperimentResults ExperimentRunner::runWithBinaryHeap() {
    ExperimentResults results;
    results.heapType = "Binary Heap";
    results.numNodes = graph.getNumVertices();
    results.numEdges = graph.getNumEdges();

    InstrumentedBinaryHeap heap;
    Dijkstra dijkstra(graph);

    std::cout << "DEBUG: Initial Binary Heap - isEmpty: " << heap.isEmpty()
        << ", size: " << heap.getSize() << std::endl;

    Timer totalTimer;
    totalTimer.start();
    auto allDistances = dijkstra.runAllPairs(heap);
    totalTimer.stop();

    std::cout << "DEBUG: Final Binary Heap - isEmpty: " << heap.isEmpty()
        << ", size: " << heap.getSize() << std::endl;

    results.totalRuntimeMs = totalTimer.elapsedMilliseconds();
    results.insertCount = heap.getInsertCount();
    results.extractMinCount = heap.getExtractMinCount();
    results.decreaseKeyCount = heap.getDecreaseKeyCount();

    results.avgInsertTimeUs = heap.getAvgInsertTime();
    results.avgExtractMinTimeUs = heap.getAvgExtractMinTime();
    results.avgDecreaseKeyTimeUs = heap.getAvgDecreaseKeyTime();

    results.heapHeight = heap.getMaxHeightDuringOps();
    results.numTrees = heap.getNumTrees();
    results.cascadingCutCount = 0;
    results.hollowCount = 0;
    results.memoryUsageMB = heap.estimateMemoryUsage();

    return results;
}

ExperimentResults ExperimentRunner::runWithFibonacciHeap() {
    ExperimentResults results;
    results.heapType = "Fibonacci Heap";
    results.numNodes = graph.getNumVertices();
    results.numEdges = graph.getNumEdges();

    InstrumentedFibonacciHeap heap;
    Dijkstra dijkstra(graph);

    std::cout << "DEBUG: Initial Fibonacci Heap - isEmpty: " << heap.isEmpty()
        << ", numTrees: " << heap.getNumTrees()
        << ", size: " << heap.getSize() << std::endl;

    Timer totalTimer;
    totalTimer.start();
    auto allDistances = dijkstra.runAllPairs(heap);
    totalTimer.stop();

    std::cout << "DEBUG: Final Fibonacci Heap - isEmpty: " << heap.isEmpty()
        << ", numTrees: " << heap.getNumTrees()
        << ", size: " << heap.getSize() << std::endl;

    results.totalRuntimeMs = totalTimer.elapsedMilliseconds();
    results.insertCount = heap.getInsertCount();
    results.extractMinCount = heap.getExtractMinCount();
    results.decreaseKeyCount = heap.getDecreaseKeyCount();

    results.avgInsertTimeUs = heap.getAvgInsertTime();
    results.avgExtractMinTimeUs = heap.getAvgExtractMinTime();
    results.avgDecreaseKeyTimeUs = heap.getAvgDecreaseKeyTime();

    results.heapHeight = heap.getMaxHeightDuringOps();
    results.numTrees = heap.getMaxTreesDuringOps();
    results.cascadingCutCount = heap.getCascadingCutCount();
    results.hollowCount = 0;
    results.memoryUsageMB = heap.estimateMemoryUsage();

    return results;
}

ExperimentResults ExperimentRunner::runWithHollowHeap() {
    ExperimentResults results;
    results.heapType = "Hollow Heap";
    results.numNodes = graph.getNumVertices();
    results.numEdges = graph.getNumEdges();

    InstrumentedHollowHeap heap;
    Dijkstra dijkstra(graph);

    std::cout << "DEBUG: Initial Hollow Heap - isEmpty: " << heap.isEmpty()
        << ", numTrees: " << heap.getNumTrees()
        << ", hollowNodes: " << heap.getHollowCount()
        << ", size: " << heap.getSize() << std::endl;

    Timer totalTimer;
    totalTimer.start();
    auto allDistances = dijkstra.runAllPairs(heap);
    totalTimer.stop();

    std::cout << "DEBUG: Final Hollow Heap - isEmpty: " << heap.isEmpty()
        << ", numTrees: " << heap.getNumTrees()
        << ", hollowNodes: " << heap.getHollowCount()
        << ", size: " << heap.getSize() << std::endl;

    results.totalRuntimeMs = totalTimer.elapsedMilliseconds();
    results.insertCount = heap.getInsertCount();
    results.extractMinCount = heap.getExtractMinCount();
    results.decreaseKeyCount = heap.getDecreaseKeyCount();

    results.avgInsertTimeUs = heap.getAvgInsertTime();
    results.avgExtractMinTimeUs = heap.getAvgExtractMinTime();
    results.avgDecreaseKeyTimeUs = heap.getAvgDecreaseKeyTime();

    results.heapHeight = heap.getMaxHeightDuringOps();
    results.numTrees = heap.getMaxTreesDuringOps();
    results.cascadingCutCount = 0;
    results.hollowCount = heap.getMaxHollowNodesDuringOps();
    results.memoryUsageMB = heap.estimateMemoryUsage();

    return results;
}

std::vector<ExperimentResults> ExperimentRunner::runAllExperiments() {
    std::vector<ExperimentResults> allResults;

    std::cout << "Running Binary Heap experiment..." << std::endl;
    allResults.push_back(runWithBinaryHeap());

    std::cout << "Running Fibonacci Heap experiment..." << std::endl;
    allResults.push_back(runWithFibonacciHeap());

    std::cout << "Running Hollow Heap experiment..." << std::endl;
    allResults.push_back(runWithHollowHeap());

    return allResults;
}

void ExperimentRunner::saveResultsToFile(const std::vector<ExperimentResults>& results,
    const std::string& filename) {

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing!" << std::endl;
        return;
    }

    // Write header
    file << "Heap Performance Analysis Results\n";
    file << "==================================\n\n";

    // Write table in project format
    file << "| Heap Type       | Nodes | Edges | Insert Time (µs) | Extract-Min Time (µs) | Decrease-Key Time (µs) | Total Runtime (ms) | Height | #Trees | Memory Usage (MB) |\n";
    file << "|-----------------|-------|-------|------------------|----------------------|-----------------------|-------------------|--------|--------|-------------------|\n";

    for (const auto& result : results) {
        file << "| " << std::left << std::setw(15) << result.heapType
            << "| " << std::setw(5) << result.numNodes
            << "| " << std::setw(5) << result.numEdges
            << "| " << std::setw(16) << std::fixed << std::setprecision(2) << result.avgInsertTimeUs
            << "| " << std::setw(20) << result.avgExtractMinTimeUs
            << "| " << std::setw(21) << result.avgDecreaseKeyTimeUs
            << "| " << std::setw(17) << std::setprecision(3) << result.totalRuntimeMs
            << "| " << std::setw(6) << result.heapHeight
            << "| " << std::setw(6) << result.numTrees
            << "| " << std::setw(17) << std::setprecision(3) << result.memoryUsageMB
            << "|\n";
    }

    // Write operation counts
    file << "\nOperation Counts:\n";
    for (const auto& result : results) {
        file << result.heapType << ": "
            << "Insert=" << result.insertCount
            << ", ExtractMin=" << result.extractMinCount
            << ", DecreaseKey=" << result.decreaseKeyCount
            << "\n";
    }

    // Write Fibonacci-specific stats if available
    for (const auto& result : results) {
        if (result.heapType == "Fibonacci Heap") {
            file << "\nFibonacci Heap Specific:\n";
            file << "Cascading Cuts: " << result.cascadingCutCount << "\n";
        }
        if (result.heapType == "Hollow Heap") {
            file << "\nHollow Heap Specific:\n";
            file << "Hollow Nodes: " << result.hollowCount << "\n";
        }
    }

    file.close();
    std::cout << "Results saved to: " << filename << std::endl;
}

void ExperimentRunner::printResultsTable(const std::vector<ExperimentResults>& results) {
    std::cout << "\n" << std::string(140, '=') << std::endl;
    std::cout << "EXPERIMENT RESULTS - COMPREHENSIVE" << std::endl;
    std::cout << std::string(140, '=') << std::endl;

    // Table header
    std::cout << std::left << std::setw(15) << "Heap Type"
        << std::setw(8) << "Nodes"
        << std::setw(8) << "Edges"
        << std::setw(12) << "Total(ms)"
        << std::setw(12) << "Ins(µs)"
        << std::setw(12) << "ExMin(µs)"
        << std::setw(12) << "DecKey(µs)"
        << std::setw(8) << "Height"
        << std::setw(8) << "#Trees"
        << std::setw(10) << "Mem(MB)"
        << std::endl;

    std::cout << std::string(140, '-') << std::endl;

    // Table data
    for (const auto& result : results) {
        std::cout << std::left << std::setw(15) << result.heapType
            << std::setw(8) << result.numNodes
            << std::setw(8) << result.numEdges
            << std::setw(12) << std::fixed << std::setprecision(3) << result.totalRuntimeMs
            << std::setw(12) << std::setprecision(2) << result.avgInsertTimeUs
            << std::setw(12) << result.avgExtractMinTimeUs
            << std::setw(12) << result.avgDecreaseKeyTimeUs
            << std::setw(8) << result.heapHeight
            << std::setw(8) << result.numTrees
            << std::setw(10) << std::setprecision(3) << result.memoryUsageMB
            << std::endl;
    }

    // Operation counts
    std::cout << "\nOperation Counts:" << std::endl;
    for (const auto& result : results) {
        std::cout << "  " << result.heapType << ": "
            << "Insert=" << result.insertCount
            << ", ExtractMin=" << result.extractMinCount
            << ", DecreaseKey=" << result.decreaseKeyCount
            << std::endl;
    }

    // Special statistics
    for (const auto& result : results) {
        if (result.heapType == "Fibonacci Heap") {
            std::cout << "  Fibonacci Heap - Cascading Cuts: " << result.cascadingCutCount << std::endl;
        }
        if (result.heapType == "Hollow Heap") {
            std::cout << "  Hollow Heap - Hollow Nodes: " << result.hollowCount << std::endl;
        }
    }

    std::cout << std::string(140, '=') << std::endl;
}

void ExperimentRunner::printProjectFormatTable(const std::vector<ExperimentResults>& results) {
    std::cout << "\n" << std::string(120, '=') << std::endl;
    std::cout << "PROJECT FORMAT RESULTS TABLE" << std::endl;
    std::cout << std::string(120, '=') << std::endl;

    std::cout << "| Heap Type       | Nodes | Edges | Insert Time (µs) | Extract-Min Time (µs) | Decrease-Key Time (µs) | Total Runtime (ms) | Height | #Trees | Memory Usage (MB) |\n";
    std::cout << "|-----------------|-------|-------|------------------|----------------------|-----------------------|-------------------|--------|--------|-------------------|\n";

    for (const auto& result : results) {
        std::cout << "| " << std::left << std::setw(15) << result.heapType
            << "| " << std::setw(5) << result.numNodes
            << "| " << std::setw(5) << result.numEdges
            << "| " << std::setw(16) << std::fixed << std::setprecision(2) << result.avgInsertTimeUs
            << "| " << std::setw(20) << result.avgExtractMinTimeUs
            << "| " << std::setw(21) << result.avgDecreaseKeyTimeUs
            << "| " << std::setw(17) << std::setprecision(3) << result.totalRuntimeMs
            << "| " << std::setw(6) << result.heapHeight
            << "| " << std::setw(6) << result.numTrees
            << "| " << std::setw(17) << std::setprecision(3) << result.memoryUsageMB
            << "|\n";
    }
}

bool ExperimentRunner::verifyAllHeaps(const Graph& graph, int numTestSources) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "COMPREHENSIVE HEAP CORRECTNESS VERIFICATION" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    bool allTestsPassed = true;

    // Test multiple sources
    for (int source = 0; source < numTestSources && source < graph.getNumVertices(); source++) {
        std::cout << "\n--- Testing Source " << source << " ---" << std::endl;

        if (!Dijkstra::verifyHeapConsistency(graph, source)) {
            allTestsPassed = false;
        }
    }

    std::cout << "\n" << std::string(60, '=') << std::endl;
    if (allTestsPassed) {
        std::cout << "✓ ALL VERIFICATION TESTS PASSED" << std::endl;
    }
    else {
        std::cout << "✗ SOME TESTS FAILED - HEAP IMPLEMENTATIONS MAY BE INCORRECT" << std::endl;
    }
    std::cout << std::string(60, '=') << std::endl;

    return allTestsPassed;
}

// Save Binary Heap distances
void ExperimentRunner::saveBinaryHeapDistances(const std::string& filename) {
    std::cout << "\n=== SAVING BINARY HEAP DISTANCES ===" << std::endl;

    BinaryHeap heap;
    Dijkstra dijkstra(graph);

    std::cout << "Computing all-pairs shortest paths with Binary Heap..." << std::endl;
    auto allDistances = dijkstra.runAllPairs(heap);

    Dijkstra::saveDistancesToFile(allDistances, filename);

    // Also save a sample single source for verification
    if (allDistances.size() > 0) {
        std::string sampleFile = "binary_heap_source_0_distances.txt";
        Dijkstra::saveSingleSourceDistances(allDistances[0], sampleFile, 0);
    }
}

// Save Fibonacci Heap distances  
void ExperimentRunner::saveFibonacciHeapDistances(const std::string& filename) {
    std::cout << "\n=== SAVING FIBONACCI HEAP DISTANCES ===" << std::endl;

    FibonacciHeap heap;
    Dijkstra dijkstra(graph);

    std::cout << "Computing all-pairs shortest paths with Fibonacci Heap..." << std::endl;
    auto allDistances = dijkstra.runAllPairs(heap);

    Dijkstra::saveDistancesToFile(allDistances, filename);

    // Also save a sample single source for verification
    if (allDistances.size() > 0) {
        std::string sampleFile = "fibonacci_heap_source_0_distances.txt";
        Dijkstra::saveSingleSourceDistances(allDistances[0], sampleFile, 0);
    }
}

// Save Hollow Heap distances
void ExperimentRunner::saveHollowHeapDistances(const std::string& filename) {
    std::cout << "\n=== SAVING HOLLOW HEAP DISTANCES ===" << std::endl;

    HollowHeap heap;
    Dijkstra dijkstra(graph);

    std::cout << "Computing all-pairs shortest paths with Hollow Heap..." << std::endl;
    auto allDistances = dijkstra.runAllPairs(heap);

    Dijkstra::saveDistancesToFile(allDistances, filename);

    // Also save a sample single source for verification
    if (allDistances.size() > 0) {
        std::string sampleFile = "hollow_heap_source_0_distances.txt";
        Dijkstra::saveSingleSourceDistances(allDistances[0], sampleFile, 0);
    }
}

// Save all heap distances
void ExperimentRunner::saveAllHeapDistances() {
    std::cout << "\n=== SAVING ALL HEAP DISTANCE MATRICES ===" << std::endl;

    saveBinaryHeapDistances("binary_heap_all_distances.txt");
    saveFibonacciHeapDistances("fibonacci_heap_all_distances.txt");
    saveHollowHeapDistances("hollow_heap_all_distances.txt");

    std::cout << "\n✓ All distance matrices saved successfully!" << std::endl;
    std::cout << "Files created:" << std::endl;
    std::cout << "  - binary_heap_all_distances.txt" << std::endl;
    std::cout << "  - fibonacci_heap_all_distances.txt" << std::endl;
    std::cout << "  - hollow_heap_all_distances.txt" << std::endl;
    std::cout << "  - binary_heap_source_0_distances.txt" << std::endl;
    std::cout << "  - fibonacci_heap_source_0_distances.txt" << std::endl;
    std::cout << "  - hollow_heap_source_0_distances.txt" << std::endl;
}

// ============================================================================
// HEAP VISUALIZATION IMPLEMENTATION
// ============================================================================

void ExperimentRunner::visualizeHeapStructures(int source) {
    std::cout << "\n=== VISUALIZING HEAP STRUCTURES ===" << std::endl;
    std::cout << "Source node: " << source << std::endl;

    // Binary Heap Structure
    {
        std::cout << "\n--- Binary Heap Structure ---" << std::endl;
        BinaryHeap heap;
        Dijkstra dijkstra(graph);
        auto distances = dijkstra.runFromSource(source, heap);

        std::cout << "Final State:" << std::endl;
        std::cout << "  Size: " << heap.getSize() << " nodes" << std::endl;
        std::cout << "  Height: " << heap.getHeight() << " levels" << std::endl;
        std::cout << "  Memory: " << std::fixed << std::setprecision(3) << heap.estimateMemoryUsage() << " MB" << std::endl;
        std::cout << "  Structure: Complete binary tree stored in array" << std::endl;

        // Save to file
        std::ofstream file("binary_heap_structure.txt");
        file << "BINARY HEAP STRUCTURE - Source: " << source << "\n";
        file << "================================\n";
        file << "Size: " << heap.getSize() << " nodes\n";
        file << "Height: " << heap.getHeight() << " levels\n";
        file << "Memory: " << std::fixed << std::setprecision(3) << heap.estimateMemoryUsage() << " MB\n";
        file << "Structure: Complete binary tree stored in array\n";
        file.close();
    }

    // Fibonacci Heap Structure
    {
        std::cout << "\n--- Fibonacci Heap Structure ---" << std::endl;
        FibonacciHeap heap;
        Dijkstra dijkstra(graph);
        auto distances = dijkstra.runFromSource(source, heap);

        std::cout << "Final State:" << std::endl;
        std::cout << "  Active Nodes: " << heap.getSize() << std::endl;
        std::cout << "  Root List Trees: " << heap.getNumTrees() << std::endl;
        std::cout << "  Height: " << heap.getHeight() << " levels" << std::endl;
        std::cout << "  Cascading Cuts: " << heap.getCascadingCutCount() << std::endl;
        std::cout << "  Memory: " << std::fixed << std::setprecision(3) << heap.estimateMemoryUsage() << " MB" << std::endl;
        std::cout << "  Structure: Collection of heap-ordered trees" << std::endl;

        // Save to file
        std::ofstream file("fibonacci_heap_structure.txt");
        file << "FIBONACCI HEAP STRUCTURE - Source: " << source << "\n";
        file << "====================================\n";
        file << "Active Nodes: " << heap.getSize() << "\n";
        file << "Root List Trees: " << heap.getNumTrees() << "\n";
        file << "Height: " << heap.getHeight() << " levels\n";
        file << "Cascading Cuts: " << heap.getCascadingCutCount() << "\n";
        file << "Memory: " << std::fixed << std::setprecision(3) << heap.estimateMemoryUsage() << " MB\n";
        file << "Structure: Collection of heap-ordered trees\n";
        file.close();
    }

    // Hollow Heap Structure
    {
        std::cout << "\n--- Hollow Heap Structure ---" << std::endl;
        HollowHeap heap;
        Dijkstra dijkstra(graph);
        auto distances = dijkstra.runFromSource(source, heap);

        std::cout << "Final State:" << std::endl;
        std::cout << "  Active Nodes: " << heap.getSize() << std::endl;
        std::cout << "  Hollow Nodes: " << heap.getHollowCount() << std::endl;
        std::cout << "  Root List Trees: " << heap.getNumTrees() << std::endl;
        std::cout << "  Height: " << heap.getHeight() << " levels" << std::endl;
        std::cout << "  Memory: " << std::fixed << std::setprecision(3) << heap.estimateMemoryUsage() << " MB" << std::endl;
        std::cout << "  Structure: Mixed active/hollow nodes with lazy cleanup" << std::endl;

        // Save to file
        std::ofstream file("hollow_heap_structure.txt");
        file << "HOLLOW HEAP STRUCTURE - Source: " << source << "\n";
        file << "================================\n";
        file << "Active Nodes: " << heap.getSize() << "\n";
        file << "Hollow Nodes: " << heap.getHollowCount() << "\n";
        file << "Root List Trees: " << heap.getNumTrees() << "\n";
        file << "Height: " << heap.getHeight() << " levels\n";
        file << "Memory: " << std::fixed << std::setprecision(3) << heap.estimateMemoryUsage() << " MB\n";
        file << "Structure: Mixed active/hollow nodes with lazy cleanup\n";
        file.close();
    }

    std::cout << "\nHeap structure visualizations saved to:" << std::endl;
    std::cout << "  - binary_heap_structure.txt" << std::endl;
    std::cout << "  - fibonacci_heap_structure.txt" << std::endl;
    std::cout << "  - hollow_heap_structure.txt" << std::endl;
}

void ExperimentRunner::captureHeapEvolution(int source, int max_steps) {
    std::cout << "\n=== CAPTURING HEAP EVOLUTION ===" << std::endl;
    std::cout << "Source: " << source << ", Max Steps: " << max_steps << std::endl;

    // Binary Heap Evolution
    {
        std::cout << "\n--- Capturing Binary Heap Evolution ---" << std::endl;
        BinaryHeap heap;

        std::ofstream evolutionFile("binary_heap_evolution.txt");
        evolutionFile << "BINARY HEAP EVOLUTION - Source: " << source << "\n";
        evolutionFile << "==========================================\n\n";

        int numVertices = graph.getNumVertices();
        std::vector<double> dist(numVertices, std::numeric_limits<double>::infinity());
        std::vector<bool> visited(numVertices, false);

        dist[source] = 0.0;
        heap.insert(source, 0.0);

        evolutionFile << "Initial: Insert source node " << source << "\n";
        evolutionFile << "  Heap size: " << heap.getSize() << ", height: " << heap.getHeight() << "\n\n";

        int step = 0;
        while (!heap.isEmpty() && step < max_steps) {
            evolutionFile << "Step " << step << ":\n";
            evolutionFile << "  Before extract-min: size=" << heap.getSize() << ", height=" << heap.getHeight() << "\n";

            int u = heap.extractMin();
            if (visited[u]) {
                evolutionFile << "  Skipped already visited node " << u << "\n";
                continue;
            }

            visited[u] = true;
            evolutionFile << "  Extracted node " << u << " with distance " << dist[u] << "\n";

            int updates = 0;
            const auto& neighbors = graph.getNeighbors(u);
            for (const auto& neighbor : neighbors) {
                int v = neighbor.first;
                double weight = neighbor.second;

                if (visited[v]) continue;

                double new_dist = dist[u] + weight;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;

                    if (heap.contains(v)) {
                        heap.decreaseKey(v, new_dist);
                        evolutionFile << "    Decrease-key: node " << v << " -> " << new_dist << "\n";
                    }
                    else {
                        heap.insert(v, new_dist);
                        evolutionFile << "    Insert: node " << v << " with distance " << new_dist << "\n";
                    }
                    updates++;
                }
            }

            evolutionFile << "  Total updates: " << updates << "\n";
            evolutionFile << "  After operations: size=" << heap.getSize() << ", height=" << heap.getHeight() << "\n";
            evolutionFile << "  -----------------------------------------\n";

            step++;
        }

        evolutionFile << "\nEvolution completed after " << step << " steps\n";
        evolutionFile << "Final heap size: " << heap.getSize() << "\n";
        evolutionFile.close();

        std::cout << "Saved to: binary_heap_evolution.txt" << std::endl;
    }

    // Fibonacci Heap Evolution
    {
        std::cout << "\n--- Capturing Fibonacci Heap Evolution ---" << std::endl;
        FibonacciHeap heap;

        std::ofstream evolutionFile("fibonacci_heap_evolution.txt");
        evolutionFile << "FIBONACCI HEAP EVOLUTION - Source: " << source << "\n";
        evolutionFile << "=============================================\n\n";

        int numVertices = graph.getNumVertices();
        std::vector<double> dist(numVertices, std::numeric_limits<double>::infinity());
        std::vector<bool> visited(numVertices, false);

        dist[source] = 0.0;
        heap.insert(source, 0.0);

        evolutionFile << "Initial: Insert source node " << source << "\n";
        evolutionFile << "  Trees: " << heap.getNumTrees() << ", nodes: " << heap.getSize() << "\n\n";

        int step = 0;
        while (!heap.isEmpty() && step < max_steps) {
            evolutionFile << "Step " << step << ":\n";
            evolutionFile << "  Before extract-min: trees=" << heap.getNumTrees() << ", nodes=" << heap.getSize() << "\n";

            int u = heap.extractMin();
            if (visited[u]) {
                evolutionFile << "  Skipped already visited node " << u << "\n";
                continue;
            }

            visited[u] = true;
            evolutionFile << "  Extracted node " << u << " with distance " << dist[u] << "\n";
            evolutionFile << "  After extract-min: trees=" << heap.getNumTrees() << " (consolidation)\n";

            int updates = 0;
            const auto& neighbors = graph.getNeighbors(u);
            for (const auto& neighbor : neighbors) {
                int v = neighbor.first;
                double weight = neighbor.second;

                if (visited[v]) continue;

                double new_dist = dist[u] + weight;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;

                    if (heap.contains(v)) {
                        heap.decreaseKey(v, new_dist);
                        evolutionFile << "    Decrease-key: node " << v << " -> " << new_dist << "\n";
                    }
                    else {
                        heap.insert(v, new_dist);
                        evolutionFile << "    Insert: node " << v << " with distance " << new_dist << "\n";
                    }
                    updates++;
                }
            }

            evolutionFile << "  Total updates: " << updates << "\n";
            evolutionFile << "  Final state: trees=" << heap.getNumTrees() << ", nodes=" << heap.getSize() << "\n";
            evolutionFile << "  Cascading cuts so far: " << heap.getCascadingCutCount() << "\n";
            evolutionFile << "  -----------------------------------------\n";

            step++;
        }

        evolutionFile << "\nEvolution completed after " << step << " steps\n";
        evolutionFile << "Final trees: " << heap.getNumTrees() << ", total cascading cuts: " << heap.getCascadingCutCount() << "\n";
        evolutionFile.close();

        std::cout << "Saved to: fibonacci_heap_evolution.txt" << std::endl;
    }

    // Hollow Heap Evolution
    {
        std::cout << "\n--- Capturing Hollow Heap Evolution ---" << std::endl;
        HollowHeap heap;

        std::ofstream evolutionFile("hollow_heap_evolution.txt");
        evolutionFile << "HOLLOW HEAP EVOLUTION - Source: " << source << "\n";
        evolutionFile << "==========================================\n\n";

        int numVertices = graph.getNumVertices();
        std::vector<double> dist(numVertices, std::numeric_limits<double>::infinity());
        std::vector<bool> visited(numVertices, false);

        dist[source] = 0.0;
        heap.insert(source, 0.0);

        evolutionFile << "Initial: Insert source node " << source << "\n";
        evolutionFile << "  Active: " << heap.getSize() << ", hollow: " << heap.getHollowCount() << "\n\n";

        int step = 0;
        while (!heap.isEmpty() && step < max_steps) {
            evolutionFile << "Step " << step << ":\n";
            evolutionFile << "  Before extract-min: active=" << heap.getSize() << ", hollow=" << heap.getHollowCount() << "\n";

            int u = heap.extractMin();
            if (visited[u]) {
                evolutionFile << "  Skipped already visited node " << u << "\n";
                continue;
            }

            visited[u] = true;
            evolutionFile << "  Extracted node " << u << " with distance " << dist[u] << "\n";
            evolutionFile << "  After extract-min: active=" << heap.getSize() << ", hollow=" << heap.getHollowCount() << " (cleanup)\n";

            int updates = 0;
            const auto& neighbors = graph.getNeighbors(u);
            for (const auto& neighbor : neighbors) {
                int v = neighbor.first;
                double weight = neighbor.second;

                if (visited[v]) continue;

                double new_dist = dist[u] + weight;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;

                    if (heap.contains(v)) {
                        heap.decreaseKey(v, new_dist);
                        evolutionFile << "    Decrease-key: node " << v << " -> " << new_dist << " (creates hollow)\n";
                    }
                    else {
                        heap.insert(v, new_dist);
                        evolutionFile << "    Insert: node " << v << " with distance " << new_dist << "\n";
                    }
                    updates++;
                }
            }

            evolutionFile << "  Total updates: " << updates << "\n";
            evolutionFile << "  Final state: active=" << heap.getSize() << ", hollow=" << heap.getHollowCount() << "\n";
            evolutionFile << "  -----------------------------------------\n";

            step++;
        }

        evolutionFile << "\nEvolution completed after " << step << " steps\n";
        evolutionFile << "Final active: " << heap.getSize() << ", final hollow: " << heap.getHollowCount() << "\n";
        evolutionFile.close();

        std::cout << "Saved to: hollow_heap_evolution.txt" << std::endl;
    }

    std::cout << "\n=== HEAP EVOLUTION CAPTURE COMPLETED ===" << std::endl;
    std::cout << "Check the generated .txt files for detailed step-by-step evolution." << std::endl;
}