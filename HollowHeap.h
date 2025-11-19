#ifndef HOLLOWHEAP_H
#define HOLLOWHEAP_H

#include "PriorityQueue.h"
#include <unordered_map>
#include <vector>

struct HollowNode {
    int node;
    double priority;
    HollowNode* child;
    HollowNode* next;
    HollowNode* prev;  // Crucial for efficient list operations
    bool isHollow;

    HollowNode(int n, double p)
        : node(n), priority(p), child(nullptr), next(nullptr), prev(nullptr), isHollow(false) {
    }
};

class HollowHeap : public PriorityQueue {
private:
    HollowNode* minNode;
    int numNodes;
    int hollowNodeCount;
    std::unordered_map<int, HollowNode*> nodeMap;

    // Helper functions
    void addToRootList(HollowNode* node);
    void removeFromRootList(HollowNode* node);
    void link(HollowNode* child, HollowNode* parent);
    void cleanup();

public:
    HollowHeap();
    ~HollowHeap();

    void insert(int node, double priority) override;
    int extractMin() override;
    void decreaseKey(int node, double newPriority) override;
    bool isEmpty() const override;
    bool contains(int node) const override;
    void clear() override;

    // Override the base class methods
    int getSize() const override { return numNodes - hollowNodeCount; }
    int getNumTrees() const override;
    int getHeight() const override;
    double estimateMemoryUsage() const override;

    // Additional methods
    int getHollowCount() const { return hollowNodeCount; }
    void printHeap() const;

private:
    int calculateHeight(HollowNode* node) const;
    void deleteSubtree(HollowNode* node);
};

#endif