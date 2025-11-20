#include "HollowHeap.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_set>
#include <cmath>
#include <queue>

HollowHeap::HollowHeap() : minNode(nullptr), numNodes(0), hollowNodeCount(0) {}

HollowHeap::~HollowHeap() {
    clear();
}

void HollowHeap::clear() {
    if (!minNode) {
        nodeMap.clear();
        return;
    }

    // Efficient iterative deletion using BFS
    std::vector<HollowNode*> toDelete;
    std::unordered_set<HollowNode*> visited;

    std::queue<HollowNode*> q;
    q.push(minNode);

    while (!q.empty()) {
        HollowNode* node = q.front();
        q.pop();

        if (visited.count(node)) continue;
        visited.insert(node);
        toDelete.push_back(node);

        // Add all children to queue
        if (node->child) {
            HollowNode* child = node->child;
            HollowNode* firstChild = child;
            do {
                if (!visited.count(child)) {
                    q.push(child);
                }
                child = child->next;
            } while (child && child != firstChild);
        }

        // Add all siblings to queue
        if (node->next && !visited.count(node->next)) {
            q.push(node->next);
        }
    }

    for (HollowNode* node : toDelete) {
        delete node;
    }

    minNode = nullptr;
    numNodes = 0;
    hollowNodeCount = 0;
    nodeMap.clear();
}

void HollowHeap::deleteSubtree(HollowNode* node) {
    if (!node) return;

    std::vector<HollowNode*> toDelete;
    std::queue<HollowNode*> q;
    q.push(node);

    while (!q.empty()) {
        HollowNode* current = q.front();
        q.pop();
        toDelete.push_back(current);

        if (current->child) {
            HollowNode* child = current->child;
            HollowNode* firstChild = child;
            do {
                q.push(child);
                child = child->next;
            } while (child && child != firstChild);
        }
    }

    for (HollowNode* n : toDelete) {
        delete n;
    }
}

void HollowHeap::addToRootList(HollowNode* node) {
    if (!node) return;

    if (!minNode) {
        minNode = node;
        node->next = node;
        node->prev = node;
    }
    else {
        // Insert at the front of the circular root list (right after minNode)
        node->next = minNode->next;
        node->prev = minNode;
        minNode->next->prev = node;
        minNode->next = node;

        // Update min if necessary
        if (node->priority < minNode->priority) {
            minNode = node;
        }
    }
    node->child = nullptr;
}

void HollowHeap::removeFromRootList(HollowNode* node) {
    if (!node || !minNode) return;

    if (node->next == node) {
        // Only node in the list
        minNode = nullptr;
    }
    else {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if (minNode == node) {
            minNode = node->next;  // Temporary min
        }
    }

    node->next = nullptr;
    node->prev = nullptr;
}

void HollowHeap::link(HollowNode* child, HollowNode* parent) {
    if (!child || !parent) return;

    removeFromRootList(child);

    // Make child the first child of parent
    if (!parent->child) {
        parent->child = child;
        child->next = child;
        child->prev = child;
    }
    else {
        child->next = parent->child;
        child->prev = parent->child->prev;
        parent->child->prev->next = child;
        parent->child->prev = child;
    }
}

void HollowHeap::cleanup() {
    if (!minNode) return;

    // Simple cleanup: remove hollow nodes from root list
    std::vector<HollowNode*> nonHollowRoots;
    HollowNode* current = minNode;

    if (current) {
        HollowNode* start = current;
        do {
            HollowNode* next = current->next;
            if (current->isHollow) {
                removeFromRootList(current);
                deleteSubtree(current);
                hollowNodeCount--;
            }
            else {
                nonHollowRoots.push_back(current);
            }
            current = next;
        } while (current && current != start);
    }
}

void HollowHeap::insert(int node, double priority) {
    if (nodeMap.find(node) != nodeMap.end()) {
        throw std::invalid_argument("Node already exists");
    }

    HollowNode* newNode = new HollowNode(node, priority);
    nodeMap[node] = newNode;
    numNodes++;

    addToRootList(newNode);
}

int HollowHeap::extractMin() {
    if (!minNode) {
        throw std::runtime_error("Cannot extract from empty heap");
    }

    // Find actual minimum (non-hollow) - much faster with circular list
    HollowNode* actualMin = nullptr;
    HollowNode* current = minNode;

    // Quick first pass - check minNode and nearby nodes
    if (current && !current->isHollow) {
        actualMin = current;
    }
    else {
        // Need to search more broadly
        if (current) {
            HollowNode* start = current;
            do {
                if (!current->isHollow && (!actualMin || current->priority < actualMin->priority)) {
                    actualMin = current;
                }
                current = current->next;
            } while (current && current != start);
        }
    }

    // If still no non-hollow found, cleanup and try again
    if (!actualMin) {
        cleanup();

        if (minNode) {
            current = minNode;
            HollowNode* start = current;
            do {
                if (!current->isHollow && (!actualMin || current->priority < actualMin->priority)) {
                    actualMin = current;
                }
                current = current->next;
            } while (current && current != start);
        }
    }

    if (!actualMin) {
        throw std::runtime_error("No valid minimum found");
    }

    int minValue = actualMin->node;
    nodeMap.erase(minValue);
    numNodes--;

    // Add children to root list efficiently
    if (actualMin->child) {
        HollowNode* child = actualMin->child;
        HollowNode* firstChild = child;

        do {
            HollowNode* nextChild = child->next;
            child->next = child;
            child->prev = child;
            addToRootList(child);
            child = nextChild;
        } while (child && child != firstChild);
    }

    // Remove the extracted node
    removeFromRootList(actualMin);
    delete actualMin;

    return minValue;
}

void HollowHeap::decreaseKey(int node, double newPriority) {
    auto it = nodeMap.find(node);
    if (it == nodeMap.end()) {
        throw std::invalid_argument("Node not found");
    }

    HollowNode* oldNode = it->second;

    if (newPriority > oldNode->priority) {
        throw std::invalid_argument("New priority is greater");
    }

    // Hollow heap strategy: insert new node, mark old as hollow
    HollowNode* newNode = new HollowNode(node, newPriority);
    nodeMap[node] = newNode;
    numNodes++;

    oldNode->isHollow = true;
    hollowNodeCount++;

    addToRootList(newNode);

    // Update min pointer efficiently
    if (newPriority < minNode->priority) {
        minNode = newNode;
    }
}

bool HollowHeap::isEmpty() const {
    if (!minNode) return true;

    // Quick check of minNode first
    if (!minNode->isHollow) return false;

    // Then check all roots
    HollowNode* current = minNode;
    HollowNode* start = current;
    do {
        if (!current->isHollow) return false;
        current = current->next;
    } while (current && current != start);

    return true;
}

bool HollowHeap::contains(int node) const {
    auto it = nodeMap.find(node);
    return it != nodeMap.end() && !it->second->isHollow;
}

int HollowHeap::getNumTrees() const {
    if (!minNode) return 0;

    int count = 0;
    HollowNode* current = minNode;

    if (current) {
        HollowNode* start = current;
        do {
            count++;
            current = current->next;
        } while (current && current != start);
    }

    return count;
}

int HollowHeap::calculateHeight(HollowNode* node) const {
    if (!node) return 0;

    int maxHeight = 0;
    if (node->child) {
        HollowNode* child = node->child;
        HollowNode* firstChild = child;
        do {
            int childHeight = calculateHeight(child);
            maxHeight = std::max(maxHeight, childHeight);
            child = child->next;
        } while (child && child != firstChild);
    }

    return maxHeight + 1;
}

int HollowHeap::getHeight() const {
    if (!minNode) return 0;

    int maxHeight = 0;
    HollowNode* current = minNode;

    if (current) {
        HollowNode* start = current;
        do {
            if (!current->isHollow) {
                int treeHeight = calculateHeight(current);
                maxHeight = std::max(maxHeight, treeHeight);
            }
            current = current->next;
        } while (current && current != start);
    }

    return maxHeight;
}

double HollowHeap::estimateMemoryUsage() const {
    if (numNodes == 0) return 0.001;

    size_t nodeMemory = numNodes * sizeof(HollowNode);
    size_t mapMemory = nodeMap.size() * (sizeof(int) + sizeof(HollowNode*) + sizeof(void*) * 2);

    double totalMB = (nodeMemory + mapMemory) / (1024.0 * 1024.0);
    return std::max(totalMB, 0.001);
}

void HollowHeap::printHeap() const {
    if (!minNode) {
        std::cout << "Hollow Heap is empty" << std::endl;
        return;
    }

    std::cout << "Hollow Heap - Min: (" << minNode->node << "," << minNode->priority
        << "), Nodes: " << numNodes << ", Trees: " << getNumTrees()
        << ", Hollow: " << hollowNodeCount << std::endl;
}