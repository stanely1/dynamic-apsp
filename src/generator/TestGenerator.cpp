#include <generator/TestGenerator.hpp>
#include <common/Constants.hpp>
#include <cassert>

namespace apsp::generator
{

TestGenerator::TestGenerator() : TestGenerator(std::random_device()())
{
}

TestGenerator::TestGenerator(RngSeed seed) : rng(seed)
{
}

TestGenerator::ResultType TestGenerator::getRandomTest(
    std::uint32_t n,
    std::uint32_t m,
    std::uint32_t numUpdates,
    std::uint32_t minEdgesPerUpdate,
    std::uint32_t maxEdgesPerUpdate,
    double minEdgeWeight,
    double maxEdgeWeight,
    double edgeDeleteProbability)
{
    assert(n > 0u);
    assert(minEdgeWeight >= 0.0);

    ResultType result{
        .graph{common::Graph(n)},
        .updates{std::vector<ResultType::Update>(numUpdates)}};

    std::uniform_int_distribution<common::Vertex> vertexDist(0u, n-1);
    std::uniform_int_distribution<std::uint32_t> edgesPerUpdateDist(minEdgesPerUpdate, maxEdgesPerUpdate);
    // std::uniform_real_distribution<double> weightDist(minEdgeWeight, maxEdgeWeight);
    std::uniform_int_distribution<std::uint32_t> weightDist(static_cast<std::uint32_t>(minEdgeWeight), static_cast<std::uint32_t>(maxEdgeWeight));

    // generate graph
    for (auto i{0u}; i < m; ++i)
    {
        const auto x{vertexDist(rng)};
        auto y{vertexDist(rng)};
        while (x == y)
        {
            y = vertexDist(rng);
        }
        const auto w{weightDist(rng)};
        result.graph.updateEdgeWeight(x, y, w);
    }

    // generate update sequence
    for (auto& update : result.updates)
    {
        update.vertex = vertexDist(rng);

        const auto inEdgesCount{edgesPerUpdateDist(rng)};
        for (auto i{0u}; i < inEdgesCount; ++i)
        {
            auto v{vertexDist(rng)};
            while (v == update.vertex)
            {
                v = vertexDist(rng);
            }
            const auto w{randomDouble01() < edgeDeleteProbability ? common::UNREACHABLE : weightDist(rng)};
            update.in[v] = w;
        }

        const auto outEdgesCount{edgesPerUpdateDist(rng)};
        for (auto i{0u}; i < outEdgesCount; ++i)
        {
            auto v{vertexDist(rng)};
            while (v == update.vertex)
            {
                v = vertexDist(rng);
            }
            const auto w{randomDouble01() < edgeDeleteProbability ? common::UNREACHABLE : weightDist(rng)};
            update.out[v] = w;
        }
    }

    return result;
}

TestGenerator::ResultType TestGenerator::getRandomUndirectedTree(
    std::uint32_t n,
    std::uint32_t numUpdates,
    std::uint32_t minEdgesPerUpdate,
    std::uint32_t maxEdgesPerUpdate,
    double minEdgeWeight,
    double maxEdgeWeight)
{
    assert(n > 0u);
    assert(minEdgeWeight >= 0.0);

    ResultType result{
        .graph{common::Graph(n)},
        .updates{std::vector<ResultType::Update>(numUpdates)}};

    std::uniform_int_distribution<common::Vertex> vertexDist(0u, n-1u);
    std::uniform_int_distribution<std::uint32_t> edgesPerUpdateDist(minEdgesPerUpdate, maxEdgesPerUpdate);
    // std::uniform_real_distribution<double> weightDist(minEdgeWeight, maxEdgeWeight);
    std::uniform_int_distribution<std::uint32_t> weightDist(static_cast<std::uint32_t>(minEdgeWeight), static_cast<std::uint32_t>(maxEdgeWeight));

    // generate graph
    for (common::Vertex y{1u}; y < n; ++y)
    {
        std::uniform_int_distribution<common::Vertex> sourceDist(0u, y-1u);
        const auto x{sourceDist(rng)};
        const auto w1{weightDist(rng)};
        const auto w2{weightDist(rng)};
        result.graph.updateEdgeWeight(x, y, w1);
        result.graph.updateEdgeWeight(y, x, w2);
    }

    // generate update sequence
    for (auto& update : result.updates)
    {
        update.vertex = vertexDist(rng);

        const auto& edges{result.graph.getOutEdges(update.vertex)};
        std::vector<common::Vertex> connectedVertices{};
        connectedVertices.reserve(edges.size());
        for (const auto [x, _] : edges)
        {
            connectedVertices.push_back(x);
        }
        std::uniform_int_distribution<std::size_t> idxDist(0, connectedVertices.size()-1);

        const auto inEdgesCount{edgesPerUpdateDist(rng)};
        for (auto i{0u}; i < inEdgesCount; ++i)
        {
            const auto v{connectedVertices[idxDist(rng)]};
            update.in[v] = weightDist(rng);
        }

        const auto outEdgesCount{edgesPerUpdateDist(rng)};
        for (auto i{0u}; i < outEdgesCount; ++i)
        {
            const auto v{connectedVertices[idxDist(rng)]};
            update.out[v] = weightDist(rng);
        }
    }

    return result;
}

TestGenerator::ResultType TestGenerator::getRandomClique(
        std::uint32_t n,
        std::uint32_t numUpdates,
        std::uint32_t minEdgesPerUpdate,
        std::uint32_t maxEdgesPerUpdate,
        double minEdgeWeight,
        double maxEdgeWeight)
{
    assert(n > 0u);
    assert(minEdgeWeight >= 0.0);

    ResultType result{
        .graph{common::Graph(n)},
        .updates{std::vector<ResultType::Update>(numUpdates)}};

    std::uniform_int_distribution<common::Vertex> vertexDist(0u, n-1);
    std::uniform_int_distribution<std::uint32_t> edgesPerUpdateDist(minEdgesPerUpdate, maxEdgesPerUpdate);
    // std::uniform_real_distribution<double> weightDist(minEdgeWeight, maxEdgeWeight);
    std::uniform_int_distribution<std::uint32_t> weightDist(static_cast<std::uint32_t>(minEdgeWeight), static_cast<std::uint32_t>(maxEdgeWeight));

    // generate graph
    for (common::Vertex x{0u}; x < n; ++x)
    {
        common::VertexToWeightMap out{};
        for (common::Vertex y{0u}; y < n; ++y)
        {
            if (x == y)
            {
                continue;
            }
            out[y] = weightDist(rng);
        }
        result.graph.updateVertex(x, {}, out);
    }

    // generate update sequence
    for (auto& update : result.updates)
    {
        update.vertex = vertexDist(rng);

        const auto inEdgesCount{edgesPerUpdateDist(rng)};
        for (auto i{0u}; i < inEdgesCount; ++i)
        {
            auto v{vertexDist(rng)};
            while (v == update.vertex)
            {
                v = vertexDist(rng);
            }
            update.in[v] = weightDist(rng);
        }

        const auto outEdgesCount{edgesPerUpdateDist(rng)};
        for (auto i{0u}; i < outEdgesCount; ++i)
        {
            auto v{vertexDist(rng)};
            while (v == update.vertex)
            {
                v = vertexDist(rng);
            }
            update.out[v] = weightDist(rng);
        }
    }

    return result;
}

TestGenerator::ResultType TestGenerator::getEqualWeightClique(
        std::uint32_t n,
        std::uint32_t numUpdates,
        std::uint32_t minEdgesPerUpdate,
        std::uint32_t maxEdgesPerUpdate,
        double edgeWeight)
{
    return getRandomClique(n, numUpdates, minEdgesPerUpdate, maxEdgesPerUpdate, edgeWeight, edgeWeight);
}

// Figure 3 in paper
TestGenerator::ResultType TestGenerator::getNonUniqueShortestPathsCounterexampleFromPaper()
{
    ResultType result{
        .graph{common::Graph(6u)},
        .updates{}};

    result.graph.updateVertex(0, {}, {{1, 1}, {2, 1}});
    result.graph.updateVertex(1, {}, {{3, 1}, {4, 1}});
    result.graph.updateVertex(2, {}, {{3, 1}, {4, 1}});
    result.graph.updateVertex(3, {}, {{5, 1}});
    result.graph.updateVertex(4, {}, {{5, 1}});

    return result;
}

/*
      0
     ^ \
    /   v
4->5     2->3
    \   ^
     v /
      1
*/
TestGenerator::ResultType TestGenerator::getEqualExtendedWeightCounterexample()
{
    ResultType result{
        .graph{common::Graph(6u)},
        .updates{}};

    result.graph.updateVertex(0, {}, {{2, 2}});
    result.graph.updateVertex(1, {}, {{2, 1}});
    result.graph.updateVertex(2, {}, {{3, 1}});
    result.graph.updateVertex(4, {}, {{5, 1}});
    result.graph.updateVertex(5, {}, {{0, 1}, {1, 2}});

    return result;
}

// Figure 4 in paper
TestGenerator::ResultType TestGenerator::getPathologicalTestForLocallyShortestPathsFromPaper(
    std::uint32_t layerSize,
    std::uint32_t numUpdates)
{
    const auto n{4u*layerSize + 2u};

    ResultType result{
        .graph{common::Graph(n)},
        .updates{std::vector<ResultType::Update>(numUpdates)}};

    // generate graph
    const common::Vertex u{0u};
    const common::Vertex v{n-1u};
    result.graph.updateEdgeWeight(u, v, 10);

    common::VertexToWeightMap outL1{};
    common::VertexToWeightMap outL3{};

    for (common::Vertex x{1u}; x <= layerSize; ++x)
    {
        outL1[layerSize + x] = 1;
        outL3[3u*layerSize + x] = 1;
    }

    for (common::Vertex x{1u}; x <= layerSize; ++x)
    {
        // connect layers L1->L2 and L3->L4
        result.graph.updateVertex(x, {}, outL1);
        result.graph.updateVertex(2u*layerSize + x, {}, outL3);

        // connect L2->L3
        if (x >= 2u)
        {
            result.graph.updateEdgeWeight(layerSize + x, 2u*layerSize + x, 10u*x);
        }

        // connect L2->u and v->L3
        result.graph.updateEdgeWeight(layerSize + x, u, 1);
        result.graph.updateEdgeWeight(v, 2u*layerSize + x, 1);
    }

    // generate update sequence
    for (std::uint32_t i{0u}; i < numUpdates; ++i)
    {
        // repeatedly insert and remove edge u->v
        result.updates[i].vertex = u;
        result.updates[i].out[v] = (i&1) ? 10 : common::UNREACHABLE;
    }

    return result;
}

// Figure 6 in paper
TestGenerator::ResultType TestGenerator::getWorstCaseTestForHistoricalPathsFromPaper(std::uint32_t layerSize)
{
    const auto n{6u*layerSize};
    const auto k{layerSize - 1u};

    ResultType result{
        .graph{common::Graph(n)},
        .updates{}};

    // generate graph
    common::VertexToWeightMap outL1{};
    common::VertexToWeightMap outL3{};
    common::VertexToWeightMap outL5{};

    for (common::Vertex x{0u}; x < layerSize; ++x)
    {
        outL1[layerSize + x] = 1;
        outL3[3u*layerSize + x] = 1;
        outL5[5u*layerSize + x] = 1;
    }

    for (common::Vertex x{0u}; x < layerSize; ++x)
    {
        // connect L1->L2, L3->L4, L5->L6
        result.graph.updateVertex(x, {}, outL1);
        result.graph.updateVertex(2u*layerSize + x, {}, outL3);
        result.graph.updateVertex(4u*layerSize + x, {}, outL5);

        // connect L2->L3
        result.graph.updateEdgeWeight(layerSize + x, 2u*layerSize + x, n + x);

        // connect L4->L5
        result.graph.updateEdgeWeight(3u*layerSize + x, 4u*layerSize + x, n + x - k);
    }

    // generate update sequence
    result.updates.reserve(2u*k);

    // Phase 1
    for (std::uint32_t i{1u}; i <= k; ++i)
    {
        const ResultType::Update update{
            .vertex = layerSize + i,
            .in = {},
            .out = {{2u*layerSize + 1, n - i}}};
        result.updates.push_back(update);
    }

    // Phase 2
    for (std::uint32_t i{0u}; i < k; ++i)
    {
        const ResultType::Update update{
            .vertex = 3u*layerSize + i,
            .in = {},
            .out = {{4u*layerSize + 1, n - i + k}}};
        result.updates.push_back(update);
    }

    return result;
}

double TestGenerator::randomDouble01()
{
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng);
}

} // apsp::generator
