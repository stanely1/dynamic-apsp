#pragma once

#include <algorithms/structures/Path.hpp>
#include <memory>
#include <set>
#include <unordered_map>

namespace apsp::algorithms::structures
{

class PathPriorityQueue
{
public:
    PathPriorityQueue() = default;

    bool empty() const;
    bool contains(const std::shared_ptr<Path>&) const;
    std::shared_ptr<Path> top() const;

    void pop();
    void push(const std::shared_ptr<Path>&);
    void erase(const std::shared_ptr<Path>&);

private:
    struct PathComparator
    {
        bool operator()(const std::shared_ptr<Path>&, const std::shared_ptr<Path>&) const;
    };

    std::set<std::shared_ptr<Path>, PathComparator> container;
};

} // namespace apsp::algorithms::structures
