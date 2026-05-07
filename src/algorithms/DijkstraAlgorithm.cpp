#include <algorithms/DijkstraAlgorithm.hpp>
#include <common/Constants.hpp>

namespace apsp::algorithms
{

DijkstraAlgorithm::DijkstraAlgorithm(std::uint32_t n) : graph(n)
{
    shortestPaths.reserve(n);
    for (common::Vertex u{0u}; u < n; ++u)
    {
        std::vector<std::shared_ptr<common::Path>> row{};
        row.reserve(n);
        for (common::Vertex v{0u}; v < n; ++v)
        {
            row.push_back(std::make_shared<common::Path>(u, v));
        }
        shortestPaths.push_back(row);
    }
}

double DijkstraAlgorithm::distance(common::Vertex from, common::Vertex to)
{
    const auto n{graph.getN()};
    if (from >= n or to >= n)
    {
        return common::UNREACHABLE;
    }
    return shortestPaths[from][to]->weight;
}

std::shared_ptr<common::Path> DijkstraAlgorithm::path(common::Vertex from, common::Vertex to)
{
    const auto n{graph.getN()};
    if (from >= n or to >= n)
    {
        return nullptr;
    }
    return shortestPaths[from][to];
}

void DijkstraAlgorithm::update(common::Vertex v, const common::WeightUpdateMap& in, const common::WeightUpdateMap& out)
{
    const auto n{graph.getN()};
    if (v >= n)
    {
        return;
    }

    graph.updateVertex(v, in, out);

    for (common::Vertex v{0u}; v < n; ++v)
    {
        calculateSingleSourceShortestPaths(v);
    }
}

void DijkstraAlgorithm::calculateSingleSourceShortestPaths(common::Vertex source)
{
    // TODO: całe mięsko
    // - pierw użyć std::priority_queue
    // - kiedyś może doklepać FIBONACCI HEAP
}

} // namespace apsp::algorithms
