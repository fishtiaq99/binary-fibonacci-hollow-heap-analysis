#include "FibonacciHeap.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_set>


void FibonacciHeap::mergeWithRootList(FibonacciNode* node) {
    if (minNode == nullptr) {
        minNode = node;
        node->left = node;
        node->right = node;
    }
    else {
        node->left = minNode->left;
        node->right = minNode;
        minNode->left->right = node;
        minNode->left = node;
    }
    node->parent = nullptr;
}

FibonacciHeap::FibonacciHeap() : minNode(nullptr), numNodes(0), cascadingCutCount(0) {}

FibonacciHeap::~FibonacciHeap() {
    clear();
}

void FibonacciHeap::clear() {
    if (minNode == nullptr) return;

    std::vector<FibonacciNode*> nodesToDelete;
    std::unordered_set<FibonacciNode*> visited;

    // Use iterative approach to avoid stack overflow
    std::vector<FibonacciNode*> stack;
    stack.push_back(minNode);

    while (!stack.empty()) {
        FibonacciNode* node = stack.back();
        stack.pop_back();

        if (visited.count(node)) continue;
        visited.insert(node);
        nodesToDelete.push_back(node);

        // Add all children to stack
        if (node->child) {
            FibonacciNode* child = node->child;
            FibonacciNode* firstChild = child;
            do {
                if (!visited.count(child)) {
                    stack.push_back(child);
                }
                child = child->right;
            } while (child != firstChild);
        }

        // Add all siblings to stack
        FibonacciNode* sibling = node->right;
        while (sibling != node && !visited.count(sibling)) {
            stack.push_back(sibling);
            sibling = sibling->right;
        }
    }

    for (FibonacciNode* node : nodesToDelete) {
        delete node;
    }

    minNode = nullptr;
    numNodes = 0;
    nodeMap.clear();
}

void FibonacciHeap::addToRootList(FibonacciNode* node) {
    node->parent = nullptr;

    if (minNode == nullptr) {
        minNode = node;
        node->left = node;
        node->right = node;
    }
    else {
        // Insert to the right of minNode
        node->right = minNode->right;
        node->left = minNode;
        minNode->right->left = node;
        minNode->right = node;

        if (node->priority < minNode->priority) {
            minNode = node;
        }
    }
}

void FibonacciHeap::removeFromRootList(FibonacciNode* node) {
    if (node->right == node) {
        minNode = nullptr;
    }
    else {
        node->left->right = node->right;
        node->right->left = node->left;
        if (minNode == node) {
            minNode = node->right; // Temporary
        }
    }
}

void FibonacciHeap::insert(int node, double priority) {
    if (nodeMap.count(node)) {
        throw std::invalid_argument("Node already exists");
    }

    FibonacciNode* newNode = new FibonacciNode(node, priority);
    nodeMap[node] = newNode;
    addToRootList(newNode);
    numNodes++;
}

void FibonacciHeap::link(FibonacciNode* y, FibonacciNode* x) {
    // Remove y from root list
    removeFromRootList(y);

    // Make y a child of x
    y->parent = x;

    if (x->child == nullptr) {
        x->child = y;
        y->left = y;
        y->right = y;
    }
    else {
        // Insert y into x's child list
        y->left = x->child->left;
        y->right = x->child;
        x->child->left->right = y;
        x->child->left = y;
    }

    x->degree++;
    y->marked = false;
}

void FibonacciHeap::consolidate() {
    if (!minNode) return;

    int maxDegree = static_cast<int>(std::log2(numNodes)) + 2;
    std::vector<FibonacciNode*> degreeTable(maxDegree + 1, nullptr);

    std::vector<FibonacciNode*> roots;
    FibonacciNode* current = minNode;

    // Collect all root nodes
    do {
        roots.push_back(current);
        current = current->right;
    } while (current != minNode);

    for (FibonacciNode* node : roots) {
        int deg = node->degree;

        while (degreeTable[deg] != nullptr) {
            FibonacciNode* other = degreeTable[deg];

            // Ensure node is the one with smaller key
            if (node->priority > other->priority) {
                std::swap(node, other);
            }

            link(other, node);
            degreeTable[deg] = nullptr;
            deg++;

            if (deg >= degreeTable.size()) {
                degreeTable.resize(deg + 1, nullptr);
            }
        }
        degreeTable[deg] = node;
    }

    // Rebuild root list and find new min
    minNode = nullptr;
    for (FibonacciNode* node : degreeTable) {
        if (node != nullptr) {
            // Reset node's connections for clean insertion
            node->left = node;
            node->right = node;
            node->parent = nullptr;
            addToRootList(node);
        }
    }
}

int FibonacciHeap::extractMin() {
    if (!minNode) throw std::runtime_error("Heap is empty");

    FibonacciNode* z = minNode;
    int minValue = z->node;
    nodeMap.erase(minValue);

    // Add all children to root list
    if (z->child) {
        FibonacciNode* child = z->child;
        do {
            FibonacciNode* nextChild = child->right;
            addToRootList(child);
            child = nextChild;
        } while (child != z->child);
    }

    // Remove z from root list
    removeFromRootList(z);

    if (z->right == z) {
        minNode = nullptr;
    }
    else {
        consolidate();
    }

    delete z;
    numNodes--;

    return minValue;
}

void FibonacciHeap::cut(FibonacciNode* x, FibonacciNode* y) {
    // Remove x from y's child list
    if (x->right == x) {
        y->child = nullptr;
    }
    else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        }
    }
    y->degree--;

    addToRootList(x);
    x->parent = nullptr;
    x->marked = false;
}

void FibonacciHeap::cascadingCut(FibonacciNode* y) {
    FibonacciNode* z = y->parent;
    if (z) {
        if (!y->marked) {
            y->marked = true;
        }
        else {
            cut(y, z);
            cascadingCut(z);
            cascadingCutCount++;
        }
    }
}

void FibonacciHeap::decreaseKey(int node, double newPriority) {
    auto it = nodeMap.find(node);
    if (it == nodeMap.end()) {
        throw std::invalid_argument("Node not found");
    }

    FibonacciNode* x = it->second;
    if (newPriority > x->priority) {
        throw std::invalid_argument("New priority is greater");
    }

    x->priority = newPriority;
    FibonacciNode* parent = x->parent;

    if (parent && x->priority < parent->priority) {
        cut(x, parent);
        cascadingCut(parent);
    }

    // Update min pointer if needed (more efficient check)
    if (x->priority < minNode->priority) {
        minNode = x;
    }
}

int FibonacciHeap::getNumTrees() const {
    if (!minNode) return 0;

    int count = 0;
    FibonacciNode* current = minNode;
    do {
        count++;
        current = current->right;
    } while (current != minNode);

    return count;
}

bool FibonacciHeap::isEmpty() const {
    return minNode == nullptr;
}
bool FibonacciHeap::contains(int node) const {
    return nodeMap.find(node) != nodeMap.end();
}

int FibonacciHeap::calculateHeight(FibonacciNode* node) const {
    if (node == nullptr) return 0;

    int maxHeight = 0;
    FibonacciNode* child = node->child;
    if (child != nullptr) {
        FibonacciNode* current = child;
        do {
            int childHeight = calculateHeight(current);
            maxHeight = std::max(maxHeight, childHeight);
            current = current->right;
        } while (current != child);
    }

    return maxHeight + 1;
}

int FibonacciHeap::getHeight() const {
    if (minNode == nullptr) return 0;

    int maxHeight = 0;
    FibonacciNode* current = minNode;
    do {
        int treeHeight = calculateHeight(current);
        maxHeight = std::max(maxHeight, treeHeight);
        current = current->right;
    } while (current != minNode);

    return maxHeight;
}

double FibonacciHeap::estimateMemoryUsage() const {
    if (numNodes == 0) return 0.001;  // At least show 0.001 MB for empty heap

    // More realistic estimation
    size_t nodeMemory = numNodes * sizeof(FibonacciNode);
    size_t mapMemory = nodeMap.size() * (sizeof(int) + sizeof(FibonacciNode*) + sizeof(void*) * 2);

    double totalMB = (nodeMemory + mapMemory) / (1024.0 * 1024.0);
    return std::max(totalMB, 0.001);  // Ensure at least 0.001 MB is shown
}

void FibonacciHeap::printHeap() const {
    if (minNode == nullptr) {
        std::cout << "Fibonacci Heap is empty" << std::endl;
        return;
    }

    std::cout << "Fibonacci Heap - Min: (" << minNode->node << "," << minNode->priority
        << "), Nodes: " << numNodes << ", Trees: " << getNumTrees() << std::endl;
}