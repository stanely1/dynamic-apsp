#include <common/Graph.hpp>
#include <format>
#include <iostream>

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

    apsp::common::Graph::WeightUpdateMap in{{6, 10}, {8, 55.55}};
    apsp::common::Graph::WeightUpdateMap out{{6, 11.1}, {8, 67}};
    g.updateVertex(7, in, out);

    auto w67{g.getEdgeWeight(6,7)};
    auto w76{g.getEdgeWeight(7,6)};
    auto w78{g.getEdgeWeight(7,8)};
    auto w87{g.getEdgeWeight(8,7)};
    auto w68{g.getEdgeWeight(6,8)};

    std::cout << std::format("w(6,7)={}, w(7,6)={}, w(7,8)={}, w(8,7)={}, w(6,8)={}\n", w67, w76, w78, w87, w68);
}
