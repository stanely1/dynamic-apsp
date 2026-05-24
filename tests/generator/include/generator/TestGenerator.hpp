#pragma once

#include <common/Graph.hpp>
#include <random>
#include <vector>

namespace apsp::tests::generator
{

class TestGenerator
{
public:
    using RngType = std::mt19937;

    TestGenerator();
    TestGenerator(RngType::result_type seed);

    struct ResultType
    {
        struct Update
        {
            common::Vertex vertex;
            common::VertexToWeightMap in;
            common::VertexToWeightMap out;
        };

        common::Graph graph;
        std::vector<Update> updates;
    };

    ResultType getRandomTest(
        std::uint32_t n = 100u,
        std::uint32_t m = 1000u,
        std::uint32_t numUpdates = 1000u,
        std::uint32_t minEdgesPerUpdate = 0u,
        std::uint32_t maxEdgesPerUpdate = 100u,
        double maxEdgeWeight = 1e6,
        double edgeDeleteProbability = 0.2);

private:
    double randomDouble01();

    RngType rng;
};

} // apsp::tests::generator
