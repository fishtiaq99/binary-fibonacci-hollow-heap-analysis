#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

class PriorityQueue {
public:
    virtual ~PriorityQueue() = default;

    virtual void insert(int node, double priority) = 0;
    virtual int extractMin() = 0;
    virtual void decreaseKey(int node, double newPriority) = 0;
    virtual bool isEmpty() const = 0;
    virtual bool contains(int node) const = 0;

    // Required methods from your implementation
    virtual void clear() = 0;
    virtual int getSize() const = 0;
    virtual int getNumTrees() const = 0;
    virtual int getHeight() const = 0;
    virtual double estimateMemoryUsage() const = 0;

    // Remove these methods - they don't belong in the base interface
    // virtual long getCascadingCutCount() const = 0;  // REMOVE THIS
    // virtual int getHollowCount() const = 0;         // REMOVE THIS
};

#endif