#include <algorithms/DemetrescuItalianoAlgorithm.hpp>
#include <algorithms/DijkstraAlgorithm.hpp>
#include <algorithms/FloydWarshallAlgorithm.hpp>
#include <common/Constants.hpp>
#include <common/Graph.hpp>
#include <common/Path.hpp>
#include <format>
#include <iostream>

void printPath(const apsp::common::Path& path)
{
    for (auto x : path.getVertices())
    {
        std::cout << x << ' ';
    }
    std::cout << '\n';
}

void printPath(const std::shared_ptr<apsp::common::Path>& path)
{
    if (path)
    {
        printPath(*path);
    }
    else
    {
        std::cout << '\n';
    }
}

int main()
{
    auto g{apsp::common::Graph(10)};
    g.updateEdgeWeight(1, 4, 32);
    g.updateEdgeWeight(6, 7, 6.7);

    std::cout << std::format("n={}, m={}\n", g.getN(), g.getM());

    auto w1{g.getEdgeWeight(1, 4)};
    auto w2{g.getEdgeWeight(6, 7)};
    auto w3{g.getEdgeWeight(7, 6)};

    std::cout << std::format("w(1,4)={}, w(6,7)={}, w(7,6)={}\n", w1, w2, w3);

    apsp::common::VertexToWeightMap in{{6, 10}, {8, 55.55}};
    apsp::common::VertexToWeightMap out{{6, 11.1}, {8, 67}};
    g.updateVertex(7, in, out);

    auto w67{g.getEdgeWeight(6,7)};
    auto w76{g.getEdgeWeight(7,6)};
    auto w78{g.getEdgeWeight(7,8)};
    auto w87{g.getEdgeWeight(8,7)};
    auto w68{g.getEdgeWeight(6,8)};

    std::cout << std::format("w(6,7)={}, w(7,6)={}, w(7,8)={}, w(8,7)={}, w(6,8)={}\n", w67, w76, w78, w87, w68);

    apsp::common::Path p22(2,2);
    apsp::common::Path p12(1,2);

    std::cout << (p22.getVertices() == std::vector<apsp::common::Vertex>{2}) << '\n';   // 1
    std::cout << (p12.getVertices() == std::vector<apsp::common::Vertex>{}) << '\n';    // 1
    std::cout << (p12.getVertices() == std::vector<apsp::common::Vertex>{1,2}) << '\n'; // 0

    p12.rightSubpath = std::make_shared<apsp::common::Path>(p22);

    std::cout << (p12.getVertices() == std::vector<apsp::common::Vertex>{}) << '\n';    // 0
    std::cout << (p12.getVertices() == std::vector<apsp::common::Vertex>{1,2}) << '\n'; // 1

    std::cout << std::format("|p22|={}, |p12|={}\n", p22.weight, p12.weight);
    printPath(p12);
    printPath(p22);

    std::cout << "FLOYD-WARSHALL\n\n";

    auto fwAlg{apsp::algorithms::FloydWarshallAlgorithm(10)};
    fwAlg.update(7, in, out);

    for (apsp::common::Vertex u{6}; u <= 9; ++u)
    {
        for (apsp::common::Vertex v{6}; v <= 9; ++v)
        {
            auto d{fwAlg.distance(u, v)};
            auto p{fwAlg.path(u, v)};

            std::cout << std::format("d({},{})={}\n", u, v, d);
            std::cout << std::format("w({},{})={}\n", u, v, p ? p->weight : apsp::common::UNREACHABLE);
            std::cout << std::format("p({},{}): ", u, v);
            printPath(p);
            std::cout << '\n';
        }
    }

    std::cout << "DIJKSTRA\n\n";

    auto dijkstraAlg{apsp::algorithms::DijkstraAlgorithm(10)};
    dijkstraAlg.update(7, in, out);

    for (apsp::common::Vertex u{6}; u <= 9; ++u)
    {
        for (apsp::common::Vertex v{6}; v <= 9; ++v)
        {
            auto d{dijkstraAlg.distance(u, v)};
            auto p{dijkstraAlg.path(u, v)};

            std::cout << std::format("d({},{})={}\n", u, v, d);
            std::cout << std::format("w({},{})={}\n", u, v, p ? p->weight : apsp::common::UNREACHABLE);
            std::cout << std::format("p({},{}): ", u, v);
            printPath(p);
            std::cout << '\n';
        }
    }

    std::cout << "DEMETRESCU-ITALIANO\n\n";

    auto diAlg{apsp::algorithms::DemetrescuItalianoAlgorithm(10)};
    diAlg.update(7, in, out);

    for (apsp::common::Vertex u{6}; u <= 9; ++u)
    {
        for (apsp::common::Vertex v{6}; v <= 9; ++v)
        {
            auto d{diAlg.distance(u, v)};
            auto p{diAlg.path(u, v)};

            std::cout << std::format("d({},{})={}\n", u, v, d);
            std::cout << std::format("w({},{})={}\n", u, v, p ? p->weight : apsp::common::UNREACHABLE);
            std::cout << std::format("p({},{}): ", u, v);
            printPath(p);
            std::cout << '\n';
        }
    }
}
