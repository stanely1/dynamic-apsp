#include <algorithms/FloydWarshallAlgorithm.hpp>
#include <common/Constants.hpp>

namespace apsp::algorithms
{

FloydWarshallAlgorithm::FloydWarshallAlgorithm(std::uint32_t n) : graph(n)
{
    initializeShortestPaths();
}

FloydWarshallAlgorithm::FloydWarshallAlgorithm(const common::Graph& graph) : graph{graph}
{
    initializeShortestPaths();
    update(0u, {}, {});
}

double FloydWarshallAlgorithm::distance(common::Vertex from, common::Vertex to)
{
    const auto n{graph.getN()};
    if (from >= n or to >= n)
    {
        return common::UNREACHABLE;
    }
    return shortestPaths[from][to]->weight;
}

std::shared_ptr<common::Path> FloydWarshallAlgorithm::path(common::Vertex from, common::Vertex to)
{
    const auto n{graph.getN()};
    if (from >= n or to >= n)
    {
        return nullptr;
    }
    return shortestPaths[from][to];
}

void FloydWarshallAlgorithm::update(common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out)
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
            path.weight = graph.getEdgeWeight(x, y);
            if (graph.hasEdge(x, y))
            {
                path.leftSubpath = shortestPaths[x][x];
                path.rightSubpath = shortestPaths[y][y];
            }
            else
            {
                path.leftSubpath = nullptr;
                path.rightSubpath = nullptr;
            }
        }
    }

    for (common::Vertex k{0u}; k < n; ++k)
    {
        for (common::Vertex i{0u}; i < n; ++i)
        {
            for (common::Vertex j{0u}; j < n; ++j)
            {
                auto &path{*shortestPaths[i][j]};
                const auto newWeight{shortestPaths[i][k]->weight + shortestPaths[k][j]->weight};
                if (newWeight < path.weight)
                {
                    path.weight = newWeight;
                    const auto i1{shortestPaths[i][k]->rightSubpath->start};
                    const auto j1{shortestPaths[k][j]->leftSubpath->end};
                    path.leftSubpath = shortestPaths[i][j1];
                    path.rightSubpath = shortestPaths[i1][j];
                }
            }
        }
    }
}

void FloydWarshallAlgorithm::initializeShortestPaths()
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

} //namespace apsp::algorithms
