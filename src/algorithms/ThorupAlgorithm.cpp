#include <algorithms/ThorupAlgorithm.hpp>
#include <common/Constants.hpp>
#include <algorithm>
#include <queue>

#include <format>
#include <iostream>

namespace apsp::algorithms
{

ThorupAlgorithm::ThorupAlgorithm(std::uint32_t n)
: currentTime{0u},
  insertionTime(n, 0u)
{
    initializeBaseLevel(common::Graph(n));
    initializeSingleVertexPaths(0);
    initializeDeleteVertexEdgeWeights();
}

ThorupAlgorithm::ThorupAlgorithm(const common::Graph& graph)
: currentTime{0u},
  insertionTime(graph.getN(), 0u)
{
    initializeBaseLevel(graph);
    initializeSingleVertexPaths(0);
    initializeDeleteVertexEdgeWeights();

    const auto n{graph.getN()};
    for (common::Vertex u{0u}; u < n; ++u)
    {
        for (common::Vertex v{0u}; v < n; ++v)
        {
            initializeSingleEdgePath(0, u, v);
        }
    }
    calculateLocallyHistoricalPaths(0);

    std::cerr << "ALGORITHM SHORTEST PATHS:\n";
    for (common::Vertex x{0u}; x < n; ++x)
    {
        std::cerr << x << ": ";
        for (common::Vertex y{0u}; y < n; ++y)
        {
            std::cerr << distance(x, y) << ' ';
        }
        std::cerr << '\n';
    }
    std::cerr << '\n';
}

std::string ThorupAlgorithm::name()
{
    return "Thorup";
}

double ThorupAlgorithm::distance(common::Vertex from, common::Vertex to)
{
    const auto n{graph[0].getN()};
    if (from >= n or to >= n)
    {
        return common::UNREACHABLE;
    }
    return historicalPaths[0][from][to].empty() ? common::UNREACHABLE : historicalPaths[0][from][to].top()->weight;
}

std::shared_ptr<common::Path> ThorupAlgorithm::path(common::Vertex from, common::Vertex to)
{
    const auto n{graph[0].getN()};
    if (from >= n or to >= n)
    {
        return nullptr;
    }
    return historicalPaths[0][from][to].empty() ? nullptr : historicalPaths[0][from][to].top();
}

void ThorupAlgorithm::update(common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out)
{
    updateImpl(DELETE, v, deleteVertexEdgeWeights, deleteVertexEdgeWeights);
    updateImpl(INSERT, v, in, out);
}

void ThorupAlgorithm::initializeDeleteVertexEdgeWeights()
{
    const auto n{graph[0].getN()};
    for (common::Vertex v{0u}; v < n; ++v)
    {
        deleteVertexEdgeWeights[v] = common::UNREACHABLE;
    }
}

void ThorupAlgorithm::initializeBaseLevel(const common::Graph& graph)
{
    this->graph[0] = graph;
    const auto n{graph.getN()};
    historicalPaths[0] = PathPriorityQueueMatrix(n, std::vector<structures::PathPriorityQueue>(n));
    locallyHistoricalPaths[0] = PathPriorityQueueMatrix(n, std::vector<structures::PathPriorityQueue>(n));
    levelTime[0] = 0;
}

void ThorupAlgorithm::initializeSingleVertexPaths(std::uint8_t level)
{
    // don't process inactive levels
    if (not graph.contains(level))
    {
        return;
    }

    for (common::Vertex v{0u}; v < graph[level].getN(); ++v)
    {
        auto path{std::make_shared<structures::Path>(v, v)};
        locallyHistoricalPaths[level][v][v].push(path);
        historicalPaths[level][v][v].push(path);
    }
}

void ThorupAlgorithm::initializeSingleEdgePath(std::uint8_t level, common::Vertex from, common::Vertex to)
{
    if (graph[level].hasEdge(from, to))
    {
        auto path{std::make_shared<structures::Path>(from, to)};
        path->weight = graph[level].getEdgeWeight(from, to);
        path->id = static_cast<std::uint64_t>(graph[level].getN())*to + from;
        path->leftSubpath = historicalPaths[level][from][from].top();
        path->rightSubpath = historicalPaths[level][to][to].top();

        locallyHistoricalPaths[level][from][to].push(path);
        historicalPaths[level][to][to].top()->locallyHistoricalPreExtensions.insert(path);
        historicalPaths[level][from][from].top()->locallyHistoricalPostExtensions.insert(path);

        std::cerr << std::format("initialized edge ({}, {}) with weight {}, id: {}\n", path->start, path->end, path->weight, path->id);
    }
}

void ThorupAlgorithm::activateLevel(std::uint8_t level)
{
    // construct structure for activated level as a copy of current structure (level 0)
    levelTime[level] = currentTime;
    graph[level] = graph[0];
    historicalPaths[level] = historicalPaths[0];
    locallyHistoricalPaths[level] = locallyHistoricalPaths[0];
}

void ThorupAlgorithm::deactivateLevel(std::uint8_t level)
{
    // don't process inactive levels
    if (not graph.contains(level))
    {
        return;
    }

    // cleanup all paths through deactivated level centers
    for (std::uint8_t i{level}; (currentTime-1)>>i; ++i)
    {
        if (((currentTime-1)>>i)&1)
        {
            for (common::Vertex u{0u}; u < graph[0].getN(); ++u)
            {
                if (levelTime[i+1] < insertionTime[u] and insertionTime[u] <= levelTime[level])
                {
                    cleanup(0, u);
                }
            }
            break;
        }
    }

    // destroy structure for deactivated level
    graph.erase(level);
    historicalPaths.erase(level);
    locallyHistoricalPaths.erase(level);
    levelTime.erase(level);
}

void ThorupAlgorithm::updateImpl(UpdateType type, common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out)
{
    const auto n{graph[0].getN()};
    if (v >= n)
    {
        return;
    }

    ++currentTime;
    if (type == INSERT)
    {
        insertionTime[v] = currentTime;
    }
    graph[0].updateVertex(v, in, out);

    cleanup(0, v);

    for (std::uint8_t i{0u}; currentTime>>i; ++i)
    {
        if ((currentTime>>i)&1)
        {
            activateLevel(i+1);
            break;
        }
        else
        {
            deactivateLevel(i+1);
        }
    }

    if (type == DELETE)
    {
        // If the update t deletes a vertex v, we also destroy all system paths containing v.
        // -> z wszystkich poziomów zawierających v (+ z base?)
        // TODO: czy sprawdzać (jak?) czy v \in graph[level]?
        for (const auto& [i, _] : graph)
        {
            cleanup(i, v);
            fixup(i, v);
        }
    }

    fixup(0, v);

    std::cerr << "ALGORITHM SHORTEST PATHS:\n";
    for (common::Vertex x{0u}; x < n; ++x)
    {
        std::cerr << x << ": ";
        for (common::Vertex y{0u}; y < n; ++y)
        {
            std::cerr << distance(x, y) << ' ';
        }
        std::cerr << '\n';
    }
    std::cerr << '\n';
}

void ThorupAlgorithm::cleanup(std::uint8_t level, common::Vertex v)
{
    // don't process inactive levels
    if (not graph.contains(level))
    {
        return;
    }

    std::queue<std::shared_ptr<structures::Path>> q{};
    q.push(historicalPaths[level][v][v].top());

    while (not q.empty())
    {
        const auto path{q.front()};
        q.pop();

        auto& preExtensions{path->locallyHistoricalPreExtensions};
        for (auto it{preExtensions.begin()}; it != preExtensions.end();)
        {
            const auto p{*it};
            q.push(p);

            std::cerr << std::format("cleanup of path ({}, {}) (pre-extension of ({}, {}))\n", p->start, p->end, path->start, path->end);

            // remove from all structures
            locallyHistoricalPaths[level][p->start][p->end].erase(p); // P_xy
            it = preExtensions.erase(it); // L(r(p))
            static_pointer_cast<structures::Path>(p->leftSubpath)->locallyHistoricalPostExtensions.erase(p); // R(l(p))

            if (historicalPaths[level][p->start][p->end].contains(p))
            {
                historicalPaths[level][p->start][p->end].erase(p); // P*_xy
                static_pointer_cast<structures::Path>(p->rightSubpath)->historicalPreExtensions.erase(p); // L*(r(p))
                static_pointer_cast<structures::Path>(p->leftSubpath)->historicalPostExtensions.erase(p); // R*(l(p))
            }
        }

        auto& postExtensions{path->locallyHistoricalPostExtensions};
        for (auto it{postExtensions.begin()}; it != postExtensions.end();)
        {
            const auto p{*it};
            q.push(p);

            std::cerr << std::format("cleanup of path ({}, {}) (post-extension of ({}, {}))\n", p->start, p->end, path->start, path->end);

            // remove from all structures
            locallyHistoricalPaths[level][p->start][p->end].erase(p); // P_xy
            static_pointer_cast<structures::Path>(p->rightSubpath)->locallyHistoricalPreExtensions.erase(p); // L(r(p))
            it = postExtensions.erase(it); // R(l(p))

            if (historicalPaths[level][p->start][p->end].contains(p))
            {
                historicalPaths[level][p->start][p->end].erase(p); // P*_xy
                static_pointer_cast<structures::Path>(p->rightSubpath)->historicalPreExtensions.erase(p); // L*(r(p))
                static_pointer_cast<structures::Path>(p->leftSubpath)->historicalPostExtensions.erase(p); // R*(l(p))
            }
        }
    }
}

void ThorupAlgorithm::fixup(std::uint8_t level, common::Vertex v)
{
    // don't process inactive levels
    if (not graph.contains(level))
    {
        return;
    }

    // phase 1 of fixup in paper
    for (common::Vertex u{0u}; u < graph[level].getN(); ++u)
    {
        initializeSingleEdgePath(level, u, v);
        initializeSingleEdgePath(level, v, u);
    }

    // phase 2 and 3 of fixup in paper
    calculateLocallyHistoricalPaths(level);
}

// phase 2 and 3 of fixup in paper
void ThorupAlgorithm::calculateLocallyHistoricalPaths(std::uint8_t level)
{
    // don't process inactive levels
    if (not graph.contains(level))
    {
        return;
    }

    const auto n{graph[level].getN()};

    // phase 2 of fixup in paper
    structures::PathPriorityQueue h{};
    for (common::Vertex x{0u}; x < n; ++x)
    {
        for (common::Vertex y{0u}; y < n; ++y)
        {
            if (x == y)
            {
                continue;
            }

            if (level == 0)
            {
                // for base update take paths from all active levels
                for (const auto& [_, paths] : locallyHistoricalPaths)
                {
                    if (not paths[x][y].empty())
                    {
                        std::cerr << std::format("adding path ({}, {}) with weight {} to H\n", x, y, paths[x][y].top()->weight);
                        h.push(paths[x][y].top());
                    }
                }
            }
            else if (not locallyHistoricalPaths[level][x][y].empty())
            {
                std::cerr << std::format("adding path ({}, {}) with weight {} to H\n", x, y, locallyHistoricalPaths[level][x][y].top()->weight);
                h.push(locallyHistoricalPaths[level][x][y].top());
            }
        }
    }

    // phase 3 of fixup in paper
    std::vector<std::vector<bool>> wasExtracted(n, std::vector<bool>(n, false));
    // std::vector<std::vector<bool>> wasExtracted(n, std::vector<bool>(n, false));
    while (not h.empty())
    {
        auto path{h.top()};
        h.pop();

        const auto x{path->start};
        const auto y{path->end};

        std::cerr << std::format("extracting path ({}, {}) with weight {} from H\n", x, y, path->weight);

        // If path for this pair of vertices was already extracted, compare extended weight.
        // The idea is to keep all paths with the same extended weight, as they might yield
        // different pre/post extensions. Keeping only one of them may cause some paths never be found.
        if (wasExtracted[x][y] and historicalPaths[level][x][y].top()->extendedWeight() < path->extendedWeight())
        {
            std::cerr << "a path was already extracted for this pair, skipping\n";
            continue;
        }

        wasExtracted[x][y] = true;

        if (historicalPaths[level][x][y].contains(path))
        {
            std::cerr << "this path is already present in P* for this pair, skipping\n";
            continue;
        }

        auto lPath{static_pointer_cast<structures::Path>(path->leftSubpath)};
        auto rPath{static_pointer_cast<structures::Path>(path->rightSubpath)};

        historicalPaths[level][x][y].push(path);
        rPath->historicalPreExtensions.insert(path);
        lPath->historicalPostExtensions.insert(path);

        for (const auto& preExtension : lPath->historicalPreExtensions)
        {
            const auto newX{preExtension->start};
            if (newX == y)
            {
                continue;
            }

            auto newPath{std::make_shared<structures::Path>(newX, y)};
            newPath->weight = graph[level].getEdgeWeight(newX, x) + path->weight;
            newPath->id = std::max(preExtension->id, path->id);
            newPath->leftSubpath = preExtension;
            newPath->rightSubpath = path;

            std::cerr << std::format("adding new path ({}, {}) with weight {} (pre-extension of ({}, {}))\n", newX, y, newPath->weight, path->start, path->end);

            locallyHistoricalPaths[level][newX][y].push(newPath);
            path->locallyHistoricalPreExtensions.insert(newPath);
            preExtension->locallyHistoricalPostExtensions.insert(newPath);
            h.push(newPath);
        }

        for (const auto& postExtension : rPath->historicalPostExtensions)
        {
            const auto newY{postExtension->end};
            if (x == newY)
            {
                continue;
            }

            auto newPath{std::make_shared<structures::Path>(x, newY)};
            newPath->weight = path->weight + graph[level].getEdgeWeight(y, newY);
            newPath->id = std::max(path->id, postExtension->id);
            newPath->leftSubpath = path;
            newPath->rightSubpath = postExtension;

            std::cerr << std::format("adding new path ({}, {}) with weight {} (post-extension of ({}, {}))\n", x, newY, newPath->weight, path->start, path->end);

            locallyHistoricalPaths[level][x][newY].push(newPath);
            postExtension->locallyHistoricalPreExtensions.insert(newPath);
            path->locallyHistoricalPostExtensions.insert(newPath);
            h.push(newPath);
        }
    }
}

} // namespace apsp::algorithms
