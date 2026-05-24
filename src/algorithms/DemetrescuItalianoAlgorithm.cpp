#include <algorithms/DemetrescuItalianoAlgorithm.hpp>
#include <common/Constants.hpp>
#include <algorithm>
#include <bit>
#include <queue>

namespace apsp::algorithms
{

DemetrescuItalianoAlgorithm::DemetrescuItalianoAlgorithm(std::uint32_t n)
: graph(n),
  historicalPaths(n, std::vector<structures::PathPriorityQueue>(n)),
  locallyHistoricalPaths(n, std::vector<structures::PathPriorityQueue>(n)),
  currentTime{0u},
  lastUpdateTime(n, 0u)
{
    initializeSingleVertexPaths();
}

DemetrescuItalianoAlgorithm::DemetrescuItalianoAlgorithm(const common::Graph& graph)
: graph{graph},
  historicalPaths(graph.getN(), std::vector<structures::PathPriorityQueue>(graph.getN())),
  locallyHistoricalPaths(graph.getN(), std::vector<structures::PathPriorityQueue>(graph.getN())),
  currentTime{0u},
  lastUpdateTime(graph.getN(), 0u)
{
    initializeSingleVertexPaths();

    const auto n{graph.getN()};
    for (common::Vertex u{0u}; u < n; ++u)
    {
        for (common::Vertex v{0u}; v < n; ++v)
        {
            initializeSingleEdgePath(u, v);
        }
    }
    calculateLocallyHistoricalPaths();
}

std::string DemetrescuItalianoAlgorithm::name()
{
    return "Demetrescu-Italiano";
}

double DemetrescuItalianoAlgorithm::distance(common::Vertex from, common::Vertex to)
{
    const auto n{graph.getN()};
    if (from >= n or to >= n)
    {
        return common::UNREACHABLE;
    }
    return historicalPaths[from][to].empty() ? common::UNREACHABLE : historicalPaths[from][to].top()->weight;
}

std::shared_ptr<common::Path> DemetrescuItalianoAlgorithm::path(common::Vertex from, common::Vertex to)
{
    const auto n{graph.getN()};
    if (from >= n or to >= n)
    {
        return nullptr;
    }
    return historicalPaths[from][to].empty() ? nullptr : historicalPaths[from][to].top();
}

// fully-update in paper
void DemetrescuItalianoAlgorithm::update(common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out)
{
    const auto n{graph.getN()};
    if (v >= n)
    {
        return;
    }

    lastUpdateTime[v] = ++currentTime;
    graph.updateVertex(v, in, out);
    updateImpl(v);

    // smoothing
    for (common::Vertex u{0u}; u < n; ++u)
    {
        const auto tu{lastUpdateTime[u]};
        if (tu > 0u and std::has_single_bit(currentTime - tu))
        {
            updateImpl(u);
        }
    }
}

void DemetrescuItalianoAlgorithm::initializeSingleVertexPaths()
{
    for (common::Vertex v{0u}; v < graph.getN(); ++v)
    {
        auto path{std::make_shared<structures::Path>(v, v)};
        locallyHistoricalPaths[v][v].push(path);
        historicalPaths[v][v].push(path);
    }
}

void DemetrescuItalianoAlgorithm::initializeSingleEdgePath(common::Vertex from, common::Vertex to)
{
    if (graph.hasEdge(from, to))
    {
        auto path{std::make_shared<structures::Path>(from, to)};
        path->weight = graph.getEdgeWeight(from, to);
        path->id = static_cast<std::uint64_t>(graph.getN())*to + from;
        path->leftSubpath = historicalPaths[from][from].top();
        path->rightSubpath = historicalPaths[to][to].top();

        locallyHistoricalPaths[from][to].push(path);
        historicalPaths[to][to].top()->locallyHistoricalPreExtensions.insert(path);
        historicalPaths[from][from].top()->locallyHistoricalPostExtensions.insert(path);
    }
}

// update in paper
void DemetrescuItalianoAlgorithm::updateImpl(common::Vertex v)
{
    cleanup(v);
    fixup(v);
}

void DemetrescuItalianoAlgorithm::cleanup(common::Vertex v)
{
    std::queue<std::shared_ptr<structures::Path>> q{};
    q.push(historicalPaths[v][v].top());

    while (not q.empty())
    {
        const auto path{q.front()};
        q.pop();

        auto& preExtensions{path->locallyHistoricalPreExtensions};
        for (auto it{preExtensions.begin()}; it != preExtensions.end();)
        {
            const auto p{*it};
            q.push(p);

            // remove from all structures
            locallyHistoricalPaths[p->start][p->end].erase(p); // P_xy
            it = preExtensions.erase(it); // L(r(p))
            static_pointer_cast<structures::Path>(p->leftSubpath)->locallyHistoricalPostExtensions.erase(p); // R(l(p))

            if (historicalPaths[p->start][p->end].contains(p))
            {
                historicalPaths[p->start][p->end].erase(p); // P*_xy
                static_pointer_cast<structures::Path>(p->rightSubpath)->historicalPreExtensions.erase(p); // L*(r(p))
                static_pointer_cast<structures::Path>(p->leftSubpath)->historicalPostExtensions.erase(p); // R*(l(p))
            }
        }

        auto& postExtensions{path->locallyHistoricalPostExtensions};
        for (auto it{postExtensions.begin()}; it != postExtensions.end();)
        {
            const auto p{*it};
            q.push(p);

            // remove from all structures
            locallyHistoricalPaths[p->start][p->end].erase(p); // P_xy
            static_pointer_cast<structures::Path>(p->rightSubpath)->locallyHistoricalPreExtensions.erase(p); // L(r(p))
            it = postExtensions.erase(it); // R(l(p))

            if (historicalPaths[p->start][p->end].contains(p))
            {
                historicalPaths[p->start][p->end].erase(p); // P*_xy
                static_pointer_cast<structures::Path>(p->rightSubpath)->historicalPreExtensions.erase(p); // L*(r(p))
                static_pointer_cast<structures::Path>(p->leftSubpath)->historicalPostExtensions.erase(p); // R*(l(p))
            }
        }
    }
}

void DemetrescuItalianoAlgorithm::fixup(common::Vertex v)
{
    // phase 1 of fixup in paper
    for (common::Vertex u{0u}; u < graph.getN(); ++u)
    {
        initializeSingleEdgePath(u, v);
        initializeSingleEdgePath(v, u);
    }

    // phase 2 and 3 of fixup in paper
    calculateLocallyHistoricalPaths();
}

// phase 2 and 3 of fixup in paper
void DemetrescuItalianoAlgorithm::calculateLocallyHistoricalPaths()
{
    const auto n{graph.getN()};

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

            if (not locallyHistoricalPaths[x][y].empty())
            {
                h.push(locallyHistoricalPaths[x][y].top());
            }
        }
    }

    // phase 3 of fixup in paper
    std::vector<std::vector<bool>> wasExtracted(n, std::vector<bool>(n, false));
    while (not h.empty())
    {
        auto path{h.top()};
        h.pop();

        const auto x{path->start};
        const auto y{path->end};

        if (wasExtracted[x][y])
        {
            continue;
        }

        wasExtracted[x][y] = true;

        if (historicalPaths[x][y].contains(path))
        {
            continue;
        }

        auto lPath{static_pointer_cast<structures::Path>(path->leftSubpath)};
        auto rPath{static_pointer_cast<structures::Path>(path->rightSubpath)};

        historicalPaths[x][y].push(path);
        rPath->historicalPreExtensions.insert(path);
        lPath->historicalPostExtensions.insert(path);

        for (const auto& preExtension : lPath->historicalPreExtensions)
        {
            auto newX{preExtension->start};
            auto newPath{std::make_shared<structures::Path>(newX, y)};
            newPath->weight = graph.getEdgeWeight(newX, x) + path->weight;
            newPath->id = std::max(preExtension->id, path->id);
            newPath->leftSubpath = preExtension;
            newPath->rightSubpath = path;

            locallyHistoricalPaths[newX][y].push(newPath);
            path->locallyHistoricalPreExtensions.insert(newPath);
            preExtension->locallyHistoricalPostExtensions.insert(newPath);
            h.push(newPath);
        }

        for (const auto& postExtension : rPath->historicalPostExtensions)
        {
            auto newY{postExtension->end};
            auto newPath{std::make_shared<structures::Path>(x, newY)};
            newPath->weight = path->weight + graph.getEdgeWeight(y, newY);
            newPath->id = std::max(path->id, postExtension->id);
            newPath->leftSubpath = path;
            newPath->rightSubpath = postExtension;

            locallyHistoricalPaths[x][newY].push(newPath);
            postExtension->locallyHistoricalPreExtensions.insert(newPath);
            path->locallyHistoricalPostExtensions.insert(newPath);
            h.push(newPath);
        }
    }
}

} // namespace apsp::algorithms
