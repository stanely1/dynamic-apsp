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
    double maxEdgeWeight,
    double edgeDeleteProbability)
{
    assert(n > 0u);

    ResultType result{
        .graph{common::Graph(n)},
        .updates{std::vector<ResultType::Update>(numUpdates)}};

    std::uniform_int_distribution<common::Vertex> vertexDist(0u, n-1);
    std::uniform_int_distribution<std::uint32_t> edgesPerUpdateDist(minEdgesPerUpdate, maxEdgesPerUpdate);
    // std::uniform_real_distribution<double> weightDist(0.0, maxEdgeWeight);
    std::uniform_int_distribution<std::uint32_t> weightDist(0u, static_cast<std::uint32_t>(maxEdgeWeight));

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

double TestGenerator::randomDouble01()
{
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng);
}

} // apsp::generator
