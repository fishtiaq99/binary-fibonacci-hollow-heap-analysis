#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include "PriorityQueue.h"
#include <vector>
#include <unordered_map>
#include <stdexcept>

struct HeapElement {
    int node;
    double priority;
    HeapElement(int n, double p) : node(n), priority(p) {}
};

class BinaryHeap : public PriorityQueue {
private:
    std::vector<HeapElement> heap;
    std::unordered_map<int, int> nodeToIndex;

    void heapifyUp(int index);
    void heapifyDown(int index);
    void swapElements(int index1, int index2);

public:
    BinaryHeap() = default;

    void insert(int node, double priority) override;
    int extractMin() override;
    void decreaseKey(int node, double newPriority) override;
    bool isEmpty() const override;
    bool contains(int node) const override;
    void clear() override;

    // Required methods from PriorityQueue
    int getSize() const override { return static_cast<int>(heap.size()); }
    int getNumTrees() const override { return 1; }
    int getHeight() const override;
    double estimateMemoryUsage() const override;

    void printHeap() const;

    // Add validateHeap method here if needed
    void validateHeap() const;
};

#endif