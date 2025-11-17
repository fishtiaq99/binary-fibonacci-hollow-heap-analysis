#ifndef FIBONACCIHEAP_H
#define FIBONACCIHEAP_H

#include "PriorityQueue.h"
#include <vector>
#include <unordered_map>

struct FibonacciNode {
    int node;
    double priority;
    FibonacciNode* parent;
    FibonacciNode* child;
    FibonacciNode* left;
    FibonacciNode* right;
    int degree;
    bool marked;

    FibonacciNode(int n, double p)
        : node(n), priority(p), parent(nullptr), child(nullptr),
        left(this), right(this), degree(0), marked(false) {
    }
};

class FibonacciHeap : public PriorityQueue {
private:
    FibonacciNode* minNode;
    int numNodes;
    long cascadingCutCount;
    std::unordered_map<int, FibonacciNode*> nodeMap;

    void link(FibonacciNode* y, FibonacciNode* x);
    void consolidate();
    void cut(FibonacciNode* x, FibonacciNode* y);
    void cascadingCut(FibonacciNode* y);
    void addToRootList(FibonacciNode* node);
    void removeFromRootList(FibonacciNode* node);
    void mergeWithRootList(FibonacciNode* node);

public:
    FibonacciHeap();
    ~FibonacciHeap();

    void insert(int node, double priority) override;
    int extractMin() override;
    void decreaseKey(int node, double newPriority) override;
    bool isEmpty() const override;
    bool contains(int node) const override;
    void clear() override;

    // Override the base class methods
    int getSize() const override { return numNodes; }
    int getNumTrees() const override;
    int getHeight() const override;
    double estimateMemoryUsage() const override;

    // Make these regular methods (not overrides)
    long getCascadingCutCount() const { return cascadingCutCount; }  // REMOVE 'override'

    void printHeap() const;

private:
    int calculateHeight(FibonacciNode* node) const;
};

#endif