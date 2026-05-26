#include <algorithms/DynamicAPSPAlgorithm.hpp>
#include <algorithms/DemetrescuItalianoAlgorithm.hpp>
#include <algorithms/DijkstraAlgorithm.hpp>
#include <algorithms/FloydWarshallAlgorithm.hpp>
#include <generator/TestGenerator.hpp>
#include <algorithm>
#include <cassert>
#include <format>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"

#define TEST_MODE  "test"
#define BENCH_MODE "bench"

using namespace apsp::algorithms;
using namespace apsp::common;
using namespace apsp::generator;

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
bool runCorrectnessTest(const TestInput& input)
{
    auto algorithms{initializeAlgorithms(input.graph)};
    auto* baseline{std::ranges::find_if(algorithms, [](const auto& a){ return a->name() == BASELINE_NAME; } )->get()};

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
                        std::cout << RED << "FAILED\n" << RESET
                                  << std::format(
                                        "Mismatch in output for ({}, {}):\n{} (baseline): {}\n{}: {}\n",
                                        x, y, BASELINE_NAME, baselineOutput, algorithm->name(), algorithmOutput);
                        return false;
                    }
                }
            }
        }
    }

    std::cout << GREEN << "PASSED\n" << RESET;
    return true;
}

bool runCorrectnessTests(const std::optional<TestGenerator::RngSeed>& seed)
{
    std::cout << std::format("BASELINE: {}\n", BASELINE_NAME);

    auto generator{seed.has_value() ? TestGenerator(seed.value()) : TestGenerator{}};

    // TODO:
    // - random tests with different graph density
    // - specific graph types (trees/clique/...)
    // - ...?
    constexpr int ntests{100};
    int passed{0};
    for (int i{1}; i <= ntests; ++i)
    {
        std::cout << std::format("running test {}/{}...", i, ntests);
        // TODO: save failed test for debugging
        passed += runCorrectnessTest(generator.getRandomTest(50, 250, 50, 0, 50, 100));
    }

    // TODO: non-random tests (e.g. bad examples from paper)

    std::cout << std::format("{}/{} tests passed\n", passed, ntests);
    return passed == ntests;
}

void printUsage(const std::string& arg)
{
    constexpr int padding{10};
    std::cerr << "Usage: " << arg << " <mode> [seed]\n"
              << "\nModes:\n"
              << "  "
              << std::left << std::setw(padding) << TEST_MODE
              << "Run correctness tests\n"
              << "  "
              << std::left << std::setw(padding) << BENCH_MODE
              << "Run performance benchmarks\n"
              << "\nseed: integer\n";
}

std::optional<TestGenerator::RngSeed> parseSeed(const std::string& arg)
{
    try
    {
        size_t pos{0u};
        const auto seed{std::stoi(arg, &pos)};
        if (pos != arg.size() or seed < 0)
        {
            return std::nullopt;
        }
        return seed;
    }
    catch (const std::exception&)
    {
        return std::nullopt;
    }
}

} // namespace

int main(int argc, char *argv[])
{
    if (argc < 2 or argc > 3)
    {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const std::string mode{argv[1]};
    std::optional<TestGenerator::RngSeed> seed{};
    if (argc == 3)
    {
        seed = parseSeed(argv[2]);
        if (not seed.has_value())
        {
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (mode == TEST_MODE)
    {
        return runCorrectnessTests(seed) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if (mode == BENCH_MODE)
    {
        // TODO
        // run performance benchmarks
    }
    else
    {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

}
