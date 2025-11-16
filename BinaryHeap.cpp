#include "BinaryHeap.h"
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <string>  // Add this for std::to_string if needed


void BinaryHeap::swapElements(int index1, int index2) {
    std::swap(heap[index1], heap[index2]);
    nodeToIndex[heap[index1].node] = index1;
    nodeToIndex[heap[index2].node] = index2;
}

void BinaryHeap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[parent].priority <= heap[index].priority) break;
        swapElements(parent, index);
        index = parent;
    }
}

void BinaryHeap::heapifyDown(int index) {
    int size = heap.size();
    if (index >= size) return;  // Safety check

    while (true) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int smallest = index;

        if (leftChild < size && heap[leftChild].priority < heap[smallest].priority)
            smallest = leftChild;
        if (rightChild < size && heap[rightChild].priority < heap[smallest].priority)
            smallest = rightChild;

        if (smallest == index) break;

        swapElements(index, smallest);
        index = smallest;

        // Additional safety check
        if (index >= size) break;
    }
}

void BinaryHeap::insert(int node, double priority) {
    if (contains(node))
        throw std::invalid_argument("Node already exists");

    heap.push_back(HeapElement(node, priority));
    int index = heap.size() - 1;
    nodeToIndex[node] = index;
    heapifyUp(index);
}

int BinaryHeap::extractMin() {
    if (isEmpty())
        throw std::runtime_error("Cannot extract from empty heap");

    if (heap.empty()) {
        throw std::runtime_error("Heap is empty but isEmpty() returned false");
    }

    int minNode = heap[0].node;

    if (heap.size() == 1) {
        // Only one element - just remove it
        heap.pop_back();
        nodeToIndex.erase(minNode);
        return minNode;
    }

    // For size > 1, move last element to root and heapify down
    heap[0] = heap.back();
    nodeToIndex[heap[0].node] = 0;

    heap.pop_back();
    nodeToIndex.erase(minNode);

    if (!heap.empty()) {
        heapifyDown(0);
    }

    return minNode;
}

// In BinaryHeap::decreaseKey
void BinaryHeap::decreaseKey(int node, double newPriority) {
    if (!contains(node))
        throw std::invalid_argument("Node " + std::to_string(node) + " not found");

    int index = nodeToIndex[node];
    if (newPriority > heap[index].priority + 1e-10) {  // Added tolerance
        throw std::invalid_argument(
            "New priority " + std::to_string(newPriority) +
            " is greater than current " + std::to_string(heap[index].priority) +
            " for node " + std::to_string(node)
        );
    }

    heap[index].priority = newPriority;
    heapifyUp(index);
}

bool BinaryHeap::isEmpty() const {
    return heap.empty();
}

bool BinaryHeap::contains(int node) const {
    return nodeToIndex.find(node) != nodeToIndex.end();
}

// ADD THIS METHOD - FIXED VERSION
void BinaryHeap::clear() {
    heap.clear();
    nodeToIndex.clear();
}

int BinaryHeap::getHeight() const {
    if (heap.empty()) return 0;

    // Calculate height using formula: floor(log2(n)) + 1
    int n = heap.size();
    int height = 0;
    while (n > 0) {
        height++;
        n >>= 1;  // Equivalent to n = n / 2
    }
    return height;
}

double BinaryHeap::estimateMemoryUsage() const {
    // Calculate actual memory usage
    size_t heapMemory = heap.capacity() * sizeof(HeapElement);
    size_t mapMemory = nodeToIndex.size() * (sizeof(int) + sizeof(int));
    return (heapMemory + mapMemory) / (1024.0 * 1024.0); // Convert to MB
}

void BinaryHeap::printHeap() const {
    std::cout << "Binary Heap contents: ";
    for (const auto& element : heap) {
        std::cout << "(" << element.node << "," << element.priority << ") ";
    }
    std::cout << std::endl;
}

void BinaryHeap::validateHeap() const {
    for (int i = 0; i < heap.size(); i++) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < heap.size() && heap[left].priority < heap[i].priority) {
            throw std::runtime_error("Heap violation at index " + std::to_string(i));
        }
        if (right < heap.size() && heap[right].priority < heap[i].priority) {
            throw std::runtime_error("Heap violation at index " + std::to_string(i));
        }

        // Check nodeToIndex consistency
        if (nodeToIndex.at(heap[i].node) != i) {
            throw std::runtime_error("Index map inconsistency for node " + std::to_string(heap[i].node));
        }
    }
}