#include <algorithms/DynamicAPSPAlgorithm.hpp>
#include <algorithms/DemetrescuItalianoAlgorithm.hpp>
#include <algorithms/DijkstraAlgorithm.hpp>
#include <algorithms/FloydWarshallAlgorithm.hpp>
#include <generator/TestGenerator.hpp>
#include <algorithm>
#include <cassert>
#include <format>
#include <iostream>
#include <memory>
#include <vector>

using namespace apsp::algorithms;
using namespace apsp::common;
using namespace apsp::tests::generator;

namespace
{

using TestInput = TestGenerator::ResultType;
using AlgorithmList = std::vector<std::unique_ptr<DynamicAPSPAlgorithm>>;

const std::string BASELINE_NAME{FloydWarshallAlgorithm().name()};

AlgorithmList initializeAlgorithms(const apsp::common::Graph& graph)
{
    AlgorithmList result{};
    result.push_back(std::make_unique<FloydWarshallAlgorithm>(graph));
    result.push_back(std::make_unique<DijkstraAlgorithm>(graph));
    result.push_back(std::make_unique<DemetrescuItalianoAlgorithm>(graph));

    std::ranges::shuffle(result, std::mt19937(std::random_device()()));

    return result;
}

// Correctnes test - check if all algorithms return the same output
void runCorrectnessTest(const TestInput& input)
{
    auto algorithms{initializeAlgorithms(input.graph)};
    auto* baseline{std::ranges::find_if(algorithms, [](const auto& a){ return a->name() == BASELINE_NAME; } )->get()};

    std::cout << std::format("BASELINE: {}\n", baseline->name());

    const auto n{input.graph.getN()};

    for (const auto& update : input.updates)
    {
        // perform update
        for (auto& algorithm : algorithms)
        {
            algorithm->update(update.vertex, update.in, update.out);
        }

        // verify output
        for (Vertex x{0u}; x < n; ++x)
        {
            for (Vertex y{0u}; y < n; ++y)
            {
                const auto baselineOutput{baseline->distance(x, y)};
                for (auto& algorithm : algorithms)
                {
                    const auto algorithmOutput{algorithm->distance(x, y)};
                    if (algorithmOutput != baselineOutput)
                    {
                        std::cerr << std::format(
                            "Mismatch in output for ({}, {}):\n{} (baseline): {}\n{}: {}\n",
                            x, y, BASELINE_NAME, baselineOutput, algorithm->name(), algorithmOutput);
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }
}

} // namespace

int main(int argc, char *argv[])
{
    TestGenerator generator{};

    runCorrectnessTest(generator.getRandomTest(10, 20, 20, 0, 20, 100));
}
