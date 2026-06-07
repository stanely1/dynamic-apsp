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

void printInput(const TestInput& input)
{
    const auto n{input.graph.getN()};
    std::cout << "Graph:\n";
    for (Vertex x{0u}; x < n; ++x)
    {
        std::cout << x << ": ";
        for (Vertex y{0u}; y < n; ++y)
        {
            std::cout << input.graph.getEdgeWeight(x, y) << ' ';
        }
        std::cout << '\n';
    }

    std::cout << "\nUpdates:\n";
    int i{0};
    for (const auto& update : input.updates)
    {
        std::cout << ++i << ":\n"
                  << "v: " << update.vertex << '\n'
                  << "in: ";
        for (const auto [v, w] : update.in)
        {
            std::cout << "(" << v << ", " << w << ") ";
        }
        std::cout << "\nout: ";
        for (const auto [v, w] : update.out)
        {
            std::cout << "(" << v << ", " << w << ") ";
        }
        std::cout << "\n\n";
    }
}

bool verifyOutput(const int updateNum, const TestInput& input, const AlgorithmList& algorithms, DynamicAPSPAlgorithm* baseline)
{
    const auto n{input.graph.getN()};
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
                                    "Mismatch in output for ({}, {}) after update {}:\n{} (baseline): {}\n{}: {}\n",
                                    x, y, updateNum, BASELINE_NAME, baselineOutput, algorithm->name(), algorithmOutput);
                    printInput(input);
                    exit(EXIT_FAILURE); // TODO: remove
                    return false;
                }
            }
        }
    }
    return true;
}

// Correctnes test - check if all algorithms return the same output
bool runCorrectnessTest(const TestInput& input)
{
    auto algorithms{initializeAlgorithms(input.graph)};
    auto* baseline{std::ranges::find_if(algorithms, [](const auto& a){ return a->name() == BASELINE_NAME; } )->get()};

    int updateNum{0};
    if (not verifyOutput(updateNum, input, algorithms, baseline))
    {
        return false;
    }

    for (const auto& update : input.updates)
    {
        for (auto& algorithm : algorithms)
        {
            algorithm->update(update.vertex, update.in, update.out);
        }

        if (not verifyOutput(++updateNum, input, algorithms, baseline))
        {
            return false;
        }
    }

    std::cout << GREEN << "PASSED\n" << RESET;
    return true;
}

bool runCorrectnessTests(const std::optional<TestGenerator::RngSeed>& seed)
{
    std::cout << std::format("BASELINE: {}\n", BASELINE_NAME);

    auto generator{seed.has_value() ? TestGenerator(seed.value()) : TestGenerator{}};

    int ntestsTotal{0};
    int passedTotal{0};

    std::cout << "Random undirected trees (n=20, updates=50)\n";
    constexpr int ntestsTree{100};
    int passedTree{0};
    for (int i{1}; i <= ntestsTree; ++i)
    {
        std::cout << std::format("running test {}/{}...", i, ntestsTree);
        passedTree += runCorrectnessTest(generator.getRandomUndirectedTree(20, 50, 0, 20, 0, 100));
    }
    std::cout << std::format("{}/{} tests passed\n", passedTree, ntestsTree);
    ntestsTotal += ntestsTree;
    passedTotal += passedTree;

    std::cout << "Small random graphs (n=10, m=25, updates=20)\n";
    constexpr int ntestsSmall{10000};
    int passedSmall{0};
    for (int i{1}; i <= ntestsSmall; ++i)
    {
        std::cout << std::format("running test {}/{}...", i, ntestsSmall);
        passedSmall += runCorrectnessTest(generator.getRandomTest(10, 25, 20, 0, 10, 0, 100));
    }
    std::cout << std::format("{}/{} tests passed\n", passedSmall, ntestsSmall);
    ntestsTotal += ntestsSmall;
    passedTotal += passedSmall;

    std::cout << "Big random graphs (n=50, m=500, updates=200)\n";
    constexpr int ntestsBig{100};
    int passedBig{0};
    for (int i{1}; i <= ntestsBig; ++i)
    {
        std::cout << std::format("running test {}/{}...", i, ntestsBig);
        passedBig += runCorrectnessTest(generator.getRandomTest(50, 500, 200, 0, 50));
    }
    std::cout << std::format("{}/{} tests passed\n", passedBig, ntestsBig);
    ntestsTotal += ntestsBig;
    passedTotal += passedBig;

    std::cout << "Clique with all equal weights (n=20, weight=1)\n";
    ntestsTotal += 1;
    passedTotal += runCorrectnessTest(generator.getEqualWeightClique(20, 2, 0, 10, 1));

    std::cout << "Random clique with all almost equal weights (n=20, weights in {1, 2})\n";
    ntestsTotal += 1;
    passedTotal += runCorrectnessTest(generator.getRandomClique(20, 50, 0, 10, 1, 2));

    std::cout << "Counterexample for non-unique shortest paths from paper (figure 3)\n";
    ntestsTotal += 1;
    passedTotal += runCorrectnessTest(generator.getNonUniqueShortestPathsCounterexampleFromPaper());

    std::cout << "Counterexample for non-unique extended weight\n";
    ntestsTotal += 1;
    passedTotal += runCorrectnessTest(generator.getEqualExtendedWeightCounterexample());

    std::cout << "Pathological case triggering n^3 changes in locally shortest paths from paper (figure 4) (layer size=20, updates=20)\n";
    ntestsTotal += 1;
    passedTotal += runCorrectnessTest(generator.getPathologicalTestForLocallyShortestPathsFromPaper(20, 20));

    std::cout << "Worst-case instance with n^3 historical paths from paper (figure 6) (layer size=20)\n";
    ntestsTotal += 1;
    passedTotal += runCorrectnessTest(generator.getWorstCaseTestForHistoricalPathsFromPaper(20));

    std::cout << std::format("{}/{} tests passed in total\n", passedTotal, ntestsTotal);

    return passedTotal == ntestsTotal;
}

bool runPerformanceBenchmarks()
{
    // TODO:
    // - random tests with different graph density
    // - specific graph types (trees/clique/...)
    // - ...?

    // TODO: non-random tests (e.g. bad examples from paper)
    // - te co mają być wolne
    // - takie gdzie będzie dużo ścieżek tej samej wagi (np klika z wszystkimi wagami równymi)
    // - te wszystkie co są już w correctness (z wyjątkiem tych malych)

    return true;
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
