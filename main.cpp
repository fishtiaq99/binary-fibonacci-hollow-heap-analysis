#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <limits>
#include <ctime>
#include <algorithm>
#include <stdexcept>  // Add this for std::runtime_error
#include <cstdlib>  // Add this for rand()

// Forward declarations
class Graph;
class PriorityQueue;
class BinaryHeap;
class FibonacciHeap;
class HollowHeap;

// Include headers
#include "Graph.h"
#include "PriorityQueue.h"
#include "BinaryHeap.h"
#include "FibonacciHeap.h"
#include "HollowHeap.h"
#include "Dijkstra.h"
#include "ExperimentRunner.h"
#include "OperationProfiler.h"

// Simple JSON implementation
namespace simple_json {
    class Value {
    private:
        std::string data_;
        bool is_string_;

    public:
        Value(const std::string& str, bool is_string = false) : data_(str), is_string_(is_string) {}
        Value(int val) : data_(std::to_string(val)), is_string_(false) {}
        Value(double val) : data_(std::to_string(val)), is_string_(false) {}

        std::string toString() const {
            if (is_string_) {
                return "\"" + data_ + "\"";
            }
            return data_;
        }
    };

    class Object {
    private:
        std::vector<std::pair<std::string, Value>> values_;

    public:
        void add(const std::string& key, const Value& value) {
            values_.push_back({ key, value });
        }

        std::string toString() const {
            std::string result = "{\n";
            for (size_t i = 0; i < values_.size(); ++i) {
                result += "    \"" + values_[i].first + "\": " + values_[i].second.toString();
                if (i < values_.size() - 1) result += ",";
                result += "\n";
            }
            result += "  }";
            return result;
        }
    };
}

// Streaming Dijkstra result structure
struct DijkstraStreamResult {
    double total_time = 0;
    double insert_time = 0;
    double extract_time = 0;
    double decrease_time = 0;
    int insert_count = 0;
    int extract_count = 0;
    int decrease_count = 0;
};

// Efficient Dijkstra for streaming (put this in main.cpp)
DijkstraStreamResult runEfficientDijkstra(const Graph& graph, int source, PriorityQueue& pq) {
    DijkstraStreamResult result;
    int numVertices = graph.getNumVertices();
    std::vector<double> dist(numVertices, std::numeric_limits<double>::infinity());
    std::vector<bool> visited(numVertices, false);

    dist[source] = 0.0;

    auto start_total = clock();

    std::cout << "    Starting Dijkstra from source " << source << std::endl;

    try {
        clock_t start_op = clock();
        pq.insert(source, 0.0);
        result.insert_time += (clock() - start_op) * 1000000.0 / CLOCKS_PER_SEC;
        result.insert_count++;
    }
    catch (const std::exception& e) {
        std::cerr << "    ERROR in insert: " << e.what() << std::endl;
        return result;
    }

    int iteration = 0;
    int max_iterations = numVertices; // Allow full graph exploration

    while (!pq.isEmpty() && iteration < max_iterations) {
        iteration++;

        /*if (iteration % 1000 == 0) {
            std::cout << "    Iteration " << iteration << ", heap size: " << pq.getSize()
                << ", visited: " << std::count(visited.begin(), visited.end(), true) << std::endl;
        }*/

        int u;
        try {
            clock_t start_op = clock();
            u = pq.extractMin();
            result.extract_time += (clock() - start_op) * 1000000.0 / CLOCKS_PER_SEC;
            result.extract_count++;
        }
        catch (const std::exception& e) {
            std::cerr << "    ERROR in extractMin at iteration " << iteration << ": " << e.what() << std::endl;
            break;
        }

        // Skip if already visited with a better distance
        if (visited[u]) {
            continue;
        }

        visited[u] = true;

        const auto& neighbors = graph.getNeighbors(u);
        for (const auto& neighbor : neighbors) {
            int v = neighbor.first;
            double weight = neighbor.second;

            // Skip if already visited with final distance
            if (visited[v]) continue;

            double new_dist = dist[u] + weight;

            // Use tolerance for floating-point comparison
            if (new_dist < dist[v] - 1e-10) {  // Added tolerance
                double old_dist = dist[v];
                dist[v] = new_dist;

                try {
                    if (pq.contains(v)) {
                        clock_t start_op = clock();
                        // FIX: Only call decreaseKey if the new distance is actually smaller
                        if (new_dist < old_dist - 1e-10) {
                            pq.decreaseKey(v, new_dist);
                            result.decrease_time += (clock() - start_op) * 1000000.0 / CLOCKS_PER_SEC;
                            result.decrease_count++;
                        }
                    }
                    else {
                        clock_t start_op = clock();
                        pq.insert(v, new_dist);
                        result.insert_time += (clock() - start_op) * 1000000.0 / CLOCKS_PER_SEC;
                        result.insert_count++;
                    }
                }
                catch (const std::exception& e) {
                    // Log the error but continue - don't break the entire algorithm
                    std::cerr << "    WARNING in heap operation for node " << v
                        << ": " << e.what()
                        << " (old=" << old_dist << ", new=" << new_dist << ")" << std::endl;
                }
            }
        }
    }

    result.total_time = (clock() - start_total) * 1000.0 / CLOCKS_PER_SEC;

    int visited_count = std::count(visited.begin(), visited.end(), true);
    std::cout << "    Completed: " << visited_count << " nodes visited, "
        << result.insert_count << " inserts, "
        << result.extract_count << " extracts, "
        << result.decrease_count << " decrease-keys" << std::endl;

    return result;
}

// Add this function declaration with your other function declarations
void runGraphProfilingExperiment(const std::string& filename);

// ADD THIS FUNCTION TO main.cpp
void verifyHeapCorrectness(const std::string& filename) {
    std::cout << "\n=== VERIFYING HEAP CORRECTNESS: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    ExperimentRunner::verifyAllHeaps(graph, 3);  // Test 3 sources
}

// Streaming experiment function
void runStreamingExperiment(const std::string& config_file) {
    std::cout << "Starting streaming experiment with config: " << config_file << std::endl;

    // Simple config parsing
    std::ifstream config_stream(config_file);
    std::string graph_file, output_file;
    int sample_size = 100;  // Reduced for testing
    int batch_size = 10;    // Reduced for testing

    std::string line;
    while (std::getline(config_stream, line)) {
        if (line.find("graph_file") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            graph_file = line.substr(start, end - start);
        }
        else if (line.find("output_file") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            output_file = line.substr(start, end - start);
        }
        else if (line.find("sample_size") != std::string::npos) {
            size_t start = line.find(": ") + 2;
            sample_size = std::stoi(line.substr(start));
        }
        else if (line.find("batch_size") != std::string::npos) {
            size_t start = line.find(": ") + 2;
            batch_size = std::stoi(line.substr(start));
        }
    }

    std::cout << "Loading graph: " << graph_file << std::endl;
    Graph graph;
    if (!graph.loadFromFile(graph_file)) {
        std::cerr << "Failed to load graph: " << graph_file << std::endl;
        return;
    }

    int total_vertices = graph.getNumVertices();
    std::cout << "Graph loaded: " << total_vertices << " vertices, "
        << graph.getNumEdges() << " edges" << std::endl;

    // Use sample size, but cap it at actual number of vertices
    int actual_sample_size = std::min(sample_size, total_vertices);
    std::cout << "Processing ALL-PAIRS Dijkstra for " << actual_sample_size << " vertices" << std::endl;
    std::cout << "Processing in batches of " << batch_size << " sources" << std::endl;

    std::ofstream outfile(output_file);
    outfile << "[\n";

    bool first_entry = true;

    // Test all three heap types
    for (int heap_type = 0; heap_type < 3; heap_type++) {
        std::unique_ptr<PriorityQueue> heap;
        std::string heap_name;

        // In runStreamingExperiment function, change this:
        switch (heap_type) {
        case 0:
            heap = std::make_unique<BinaryHeap>();  // NOT InstrumentedBinaryHeap
            heap_name = "Binary";
            break;
        case 1:
            heap = std::make_unique<FibonacciHeap>();  // NOT InstrumentedFibonacciHeap
            heap_name = "Fibonacci";
            break;
        case 2:
            heap = std::make_unique<HollowHeap>();  // NOT InstrumentedHollowHeap
            heap_name = "Hollow";
            break;
        }

        std::cout << "\nTesting " << heap_name << " Heap..." << std::endl;

        // Process ALL sources (all-pairs), but in batches
        for (int batch_start = 0; batch_start < actual_sample_size; batch_start += batch_size) {
            int batch_end = std::min(batch_start + batch_size, actual_sample_size);

            for (int source = batch_start; source < batch_end; source++) {
                std::cout << "  Processing source " << source << "/" << actual_sample_size << std::endl;

                // Run single-source Dijkstra for this source
                auto result = runEfficientDijkstra(graph, source, *heap);

                // Create JSON entry
                simple_json::Object entry;
                entry.add("heap", simple_json::Value(heap_name, true));
                entry.add("source", simple_json::Value(source));
                entry.add("total_time", simple_json::Value(result.total_time));
                entry.add("insert_time", simple_json::Value(result.insert_time));
                entry.add("extract_time", simple_json::Value(result.extract_time));
                entry.add("decrease_time", simple_json::Value(result.decrease_time));
                entry.add("insert_count", simple_json::Value(result.insert_count));
                entry.add("extract_count", simple_json::Value(result.extract_count));
                entry.add("decrease_count", simple_json::Value(result.decrease_count));

                // Stream result to file immediately
                if (!first_entry) {
                    outfile << ",\n";
                }
                first_entry = false;

                outfile << entry.toString();
                outfile.flush(); // Critical for streaming
            }

            // Clear heap memory between batches
            heap->clear();
            std::cout << "  Batch completed: " << batch_end << "/" << actual_sample_size << " sources" << std::endl;
        }

        std::cout << heap_name << " Heap completed!" << std::endl;
    }

    outfile << "\n]";
    std::cout << "\nStreaming ALL-PAIRS experiment completed!" << std::endl;
    std::cout << "Results saved to: " << output_file << std::endl;
}

void runBasicTests() {
    std::cout << "\n=== Running Basic Tests ===" << std::endl;

    // Test Binary Heap
    BinaryHeap heap;
    heap.insert(0, 5.0);
    heap.insert(1, 3.0);
    heap.insert(2, 8.0);
    heap.insert(3, 1.0);
    heap.printHeap();

    // Test Dijkstra on small graph
    Graph graph;
    if (graph.loadFromFile("data/large_graph_1000.txt")) {
        Dijkstra dijkstra(graph);
        BinaryHeap heap2;
        auto distances = dijkstra.runFromSource(0, heap2);
        Dijkstra::printDistances(distances, 0);
    }
}

void runFibonacciExperiment(const std::string& filename) {
    std::cout << "\n=== Running Experiment with Fibonacci Heap: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    ExperimentRunner experiment(graph);
    auto results = experiment.runWithFibonacciHeap();

    // Print and save results
    std::vector<ExperimentResults> allResults = { results };
    ExperimentRunner::printResultsTable(allResults);

    // Save to output file
    std::string outputFile = "results_fibonacci_heap.txt";
    ExperimentRunner::saveResultsToFile(allResults, outputFile);
    std::cout << "Results saved to: " << outputFile << std::endl;
}

void runHollowExperiment(const std::string& filename) {
    std::cout << "\n=== Running Experiment with Hollow Heap: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    ExperimentRunner experiment(graph);
    auto results = experiment.runWithHollowHeap();

    // Print and save results
    std::vector<ExperimentResults> allResults = { results };
    ExperimentRunner::printResultsTable(allResults);

    // Save to output file
    std::string outputFile = "results_hollow_heap.txt";
    ExperimentRunner::saveResultsToFile(allResults, outputFile);
    std::cout << "Results saved to: " << outputFile << std::endl;
}

void runExperiment(const std::string& filename) {
    std::cout << "\n=== Running Experiment: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    ExperimentRunner experiment(graph);
    auto results = experiment.runWithBinaryHeap();

    // Print and save results
    std::vector<ExperimentResults> allResults = { results };
    ExperimentRunner::printResultsTable(allResults);

    // Save to output file
    std::string outputFile = "results_binary_heap.txt";
    ExperimentRunner::saveResultsToFile(allResults, outputFile);
    std::cout << "Results saved to: " << outputFile << std::endl;
}

void runOperationProfilingExperiment() {
    std::cout << "\n=== Running Operation Profiling Experiment ===" << std::endl;
    OperationProfiler profiler(100000); // 100,000 operations as per PDF
    auto results = profiler.runProfilingExperiment();

    profiler.printResults(results);

    // Save to file
    std::string filename = "operation_profiling_results.txt";
    profiler.saveResultsToFile(results, filename);
}

void runAllExperimentsComparison(const std::string& filename) {
    std::cout << "\n=== Running All Experiments Comparison: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    ExperimentRunner experiment(graph);
    auto results = experiment.runAllExperiments();

    // Print all result formats
    ExperimentRunner::printResultsTable(results);
    ExperimentRunner::printProjectFormatTable(results);

    // Save to file
    std::string outputFile = "comprehensive_results.txt";
    ExperimentRunner::saveResultsToFile(results, outputFile);
    std::cout << "Results saved to: " << outputFile << std::endl;
}

// ADD THESE FUNCTIONS TO main.cpp

void saveBinaryHeapDistanceMatrix(const std::string& filename) {
    std::cout << "\n=== SAVING BINARY HEAP DISTANCE MATRIX: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    ExperimentRunner experiment(graph);
    experiment.saveBinaryHeapDistances("binary_heap_distances_complete.txt");
}

void saveFibonacciHeapDistanceMatrix(const std::string& filename) {
    std::cout << "\n=== SAVING FIBONACCI HEAP DISTANCE MATRIX: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    ExperimentRunner experiment(graph);
    experiment.saveFibonacciHeapDistances("fibonacci_heap_distances_complete.txt");
}

void saveHollowHeapDistanceMatrix(const std::string& filename) {
    std::cout << "\n=== SAVING HOLLOW HEAP DISTANCE MATRIX: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    ExperimentRunner experiment(graph);
    experiment.saveHollowHeapDistances("hollow_heap_distances_complete.txt");
}

void saveAllDistanceMatrices(const std::string& filename) {
    std::cout << "\n=== SAVING ALL HEAP DISTANCE MATRICES: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    ExperimentRunner experiment(graph);
    experiment.saveAllHeapDistances();
}

// Visualization functions
void visualizeHeapStructures(const std::string& filename) {
    std::cout << "\n=== VISUALIZING HEAP STRUCTURES ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    int source;
    std::cout << "Enter source node (0-" << (graph.getNumVertices() - 1) << "): ";
    std::cin >> source;

    ExperimentRunner experiment(graph);
    experiment.visualizeHeapStructures(source);
}

void captureHeapEvolution(const std::string& filename) {
    std::cout << "\n=== CAPTURING HEAP EVOLUTION ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    int source, max_steps;
    std::cout << "Enter source node (0-" << (graph.getNumVertices() - 1) << "): ";
    std::cin >> source;
    std::cout << "Enter maximum steps to capture: ";
    std::cin >> max_steps;

    ExperimentRunner experiment(graph);
    experiment.captureHeapEvolution(source, max_steps);
}

int main(int argc, char* argv[]) {
    // Handle command line arguments for streaming mode
    if (argc > 1 && std::string(argv[1]) == "--stream-config") {
        if (argc < 3) {
            std::cerr << "Usage: HEAP.exe --stream-config <config_file.json>" << std::endl;
            return 1;
        }
        runStreamingExperiment(argv[2]);
        return 0;
    }

    // Original interactive menu
    int choice;
    std::string filename;

    do {
        std::cout << "\n=== Heap Performance Analysis ===" << std::endl;
        std::cout << "1. Run Basic Tests" << std::endl;
        std::cout << "2. Run Experiment with Binary Heap" << std::endl;
        std::cout << "3. Select Dataset File" << std::endl;
        std::cout << "4. Run Experiment with Fibonacci Heap" << std::endl;
        std::cout << "5. Run Experiment with Hollow Heap" << std::endl;
        std::cout << "6. Run Operation Profiling Experiment" << std::endl;
        std::cout << "7. Run All Experiments Comparison" << std::endl;
        std::cout << "8. Run Streaming Experiment (Large Graphs)" << std::endl;
        std::cout << "9. Run Graph Profiling Experiment" << std::endl;
        std::cout << "10. Verify Heap Correctness" << std::endl;
        std::cout << "11. Save Binary Heap Distance Matrix" << std::endl;
        std::cout << "12. Save Fibonacci Heap Distance Matrix" << std::endl;
        std::cout << "13. Save Hollow Heap Distance Matrix" << std::endl;
        std::cout << "14. Save All Distance Matrices" << std::endl;
        std::cout << "15. Visualize Heap Structures" << std::endl;
        std::cout << "16. Capture Heap Evolution (Step-by-step)" << std::endl;
        std::cout << "17. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            runBasicTests();
            break;
        case 2:
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                runExperiment(filename);
            }
            break;
        case 3:
            std::cout << "Available datasets:" << std::endl;
            std::cout << "1. data/large_graph_1000.txt" << std::endl;
            std::cout << "2. data/Hongkong.road-d" << std::endl;
            std::cout << "3. data/Shanghai.road-d" << std::endl;
            std::cout << "4. data/Chongqing.road-d" << std::endl;
            std::cout << "Enter dataset number: ";
            int datasetChoice;
            std::cin >> datasetChoice;

            switch (datasetChoice) {
            case 1: filename = "data/large_graph_1000.txt"; break;
            case 2: filename = "data/Hongkong.road-d"; break;
            case 3: filename = "data/Shanghai.road-d"; break;
            case 4: filename = "data/Chongqing.road-d"; break;
            default:
                std::cout << "Invalid choice!" << std::endl;
                filename = "";
            }
            if (!filename.empty()) {
                std::cout << "Selected: " << filename << std::endl;
            }
            break;
        case 4:
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                runFibonacciExperiment(filename);
            }
            break;
        case 5:
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                runHollowExperiment(filename);
            }
            break;
        case 6:
            runOperationProfilingExperiment();
            break;
        case 7:
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                runAllExperimentsComparison(filename);
            }
            break;
        case 8:
        {
            std::cout << "Available large datasets:" << std::endl;
            std::cout << "1. Hongkong.road-d" << std::endl;
            std::cout << "2. Shanghai.road-d" << std::endl;
            std::cout << "3. Chongqing.road-d" << std::endl;
            std::cout << "Enter dataset number: ";
            int streamChoice;
            std::cin >> streamChoice;

            std::string dataset_name;
            switch (streamChoice) {
            case 1: dataset_name = "Hongkong"; break;
            case 2: dataset_name = "Shanghai"; break;
            case 3: dataset_name = "Chongqing"; break;
            default:
                std::cout << "Invalid choice!" << std::endl;
                break;
            }

            if (!dataset_name.empty()) {
                std::string config_file = "scripts/configs/" + dataset_name + "_config.json";
                runStreamingExperiment(config_file);
            }
        }
        break;
        case 9:  // Graph Profiling Experiment
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                runGraphProfilingExperiment(filename);
            }
            break;
        case 10:  // Verify Heap Correctness
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                verifyHeapCorrectness(filename);
            }
            break;
        case 11:  // Save Binary Heap Distance Matrix
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                saveBinaryHeapDistanceMatrix(filename);
            }
            break;
        case 12:  // Save Fibonacci Heap Distance Matrix
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                saveFibonacciHeapDistanceMatrix(filename);
            }
            break;
        case 13:  // Save Hollow Heap Distance Matrix
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                saveHollowHeapDistanceMatrix(filename);
            }
            break;
        case 14:  // Save All Distance Matrices
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                saveAllDistanceMatrices(filename);
            }
            break;
        case 15:  // Visualize Heap Structures
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                visualizeHeapStructures(filename);
            }
            break;
        case 16:  // Capture Heap Evolution
            if (filename.empty()) {
                std::cout << "No dataset selected. Please select a dataset first." << std::endl;
            }
            else {
                captureHeapEvolution(filename);
            }
            break;
        case 17:  // Exit
            std::cout << "Exiting..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice!" << std::endl;
        }
    } while (choice != 17);

    return 0;
}

void runGraphProfilingExperiment(const std::string& filename) {
    std::cout << "\n=== Running Graph Profiling Experiment: " << filename << " ===" << std::endl;

    Graph graph;
    if (!graph.loadFromFile(filename)) {
        std::cerr << "Failed to load graph: " << filename << std::endl;
        return;
    }

    std::cout << "DEBUG: Graph loaded successfully" << std::endl;
    std::cout << "DEBUG: Creating OperationProfiler" << std::endl;

    OperationProfiler profiler;

    std::cout << "DEBUG: Starting graph profiling experiment" << std::endl;
    auto results = profiler.runGraphProfilingExperiment(graph, 20);  // Test with 20 sources

    std::cout << "DEBUG: Experiment completed, printing results" << std::endl;
    profiler.printGraphResults(results);

    // Save to file
    std::string outputFile = "graph_profiling_results.txt";
    profiler.saveGraphResultsToFile(results, outputFile);

    std::cout << "DEBUG: Graph profiling experiment completed!" << std::endl;
}