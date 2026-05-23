#include <algorithms/structures/PathPriorityQueue.hpp>
#include <cassert>

namespace apsp::algorithms::structures
{

bool PathPriorityQueue::empty() const
{
    return container.empty();
}

bool PathPriorityQueue::contains(const std::shared_ptr<Path>& path) const
{
    return container.contains(path);
}

std::shared_ptr<Path> PathPriorityQueue::top() const
{
    assert(not container.empty());
    return *container.begin();
}

void PathPriorityQueue::pop()
{
    assert(not container.empty());
    container.erase(container.begin());
}

void PathPriorityQueue::push(const std::shared_ptr<Path>& path)
{
    container.insert(path);
}

void PathPriorityQueue::erase(const std::shared_ptr<Path>& path)
{
    container.erase(path);
}

bool PathPriorityQueue::PathComparator::operator()(const std::shared_ptr<Path>& x, const std::shared_ptr<Path>& y) const
{
    if (x->weight != y->weight)
    {
        return x->weight < y->weight;
    }
    else if (x->id != y->id)
    {
        return x->id < y->id;
    }
    else
    {
        // to be able to distinguish different paths with the same extended weight
        return x.get() < y.get();
    }
}

} // namespace apsp::algorithms::structures
