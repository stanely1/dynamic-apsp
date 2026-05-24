#include <algorithms/DijkstraAlgorithm.hpp>
#include <common/Constants.hpp>
#include <queue>
#include <tuple>

namespace apsp::algorithms
{

DijkstraAlgorithm::DijkstraAlgorithm(std::uint32_t n) : graph(n)
{
    initializeShortestPaths();
}

DijkstraAlgorithm::DijkstraAlgorithm(const common::Graph& graph) : graph{graph}
{
    initializeShortestPaths();
    update(0u, {}, {});
}

std::string DijkstraAlgorithm::name()
{
    return "Dijkstra";
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

void DijkstraAlgorithm::update(common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out)
{
    const auto n{graph.getN()};
    if (v >= n)
    {
        return;
    }

    graph.updateVertex(v, in, out);

    for (common::Vertex x{0u}; x < n; ++x)
    {
        for (common::Vertex y{0u}; y < n; ++y)
        {
            auto& path{*shortestPaths[x][y]};
            path.weight = x == y ? 0.0 : common::UNREACHABLE;
            path.leftSubpath = nullptr;
            path.rightSubpath = nullptr;
        }
    }

    for (common::Vertex u{0u}; u < n; ++u)
    {
        calculateSingleSourceShortestPaths(u);
    }
}

void DijkstraAlgorithm::initializeShortestPaths()
{
    const auto n{graph.getN()};
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

void DijkstraAlgorithm::calculateSingleSourceShortestPaths(common::Vertex source)
{
    // TODO: FIBONACCI HEAP (?)

    using QueueItem = std::tuple<common::Vertex, common::Vertex, double>; // edge (u,v) and distance
    auto cmp = [](const auto& x, const auto& y) { return std::get<double>(x) > std::get<double>(y); };

    std::priority_queue<QueueItem, std::vector<QueueItem>, decltype(cmp)> pq(cmp);
    pq.emplace(source, source, 0.0);

    while (not pq.empty())
    {
        const auto [u, v, dist] = pq.top(); // path: source ->...-> u -> v
        pq.pop();

        auto &path{*shortestPaths[source][v]};
        // TODO: which version is correct? if both correct, which one is faster?
        // if (path.weight < dist)
        if (path.weight <= dist and v != source)
        {
            continue;
        }

        path.weight = dist;

        if (source != v)
        {
            // source -> x ->...-> u -> v
            // ^^^^^^^^^^^^^^^^^^^^^      - left
            //           ^^^^^^^^^^^^^^^^ - right
            //
            // edge case: source -> v (source == u, x == v)

            path.leftSubpath = shortestPaths[source][u];
            const auto x{source == u ? v : path.leftSubpath->rightSubpath->start};
            path.rightSubpath = shortestPaths[x][v];
        }

        // source ->...-> v -(w)-> y
        for (const auto [y, w] : graph.getOutEdges(v))
        {
            if (dist + w < shortestPaths[source][y]->weight)
            {
                pq.emplace(v, y, dist + w);
            }
        }
    }
}

} // namespace apsp::algorithms
