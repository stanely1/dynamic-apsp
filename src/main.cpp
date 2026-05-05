#include <common/Graph.hpp>
#include <format>
#include <iostream>

int main()
{
    auto g = apsp::common::Graph(10);
    g.updateEdgeWeight(1, 4, 32);
    g.updateEdgeWeight(6, 7, 6.7);

    std::cout << std::format("n={}, m={}\n", g.getN(), g.getM());

    auto w1 = g.getEdgeWeight(1, 4);
    auto w2 = g.getEdgeWeight(6, 7);
    auto w3 = g.getEdgeWeight(7, 6);

    std::cout << std::format("w(1,4)={}, w(6,7)={}, w(7,6)={}\n", w1, w2, w3);
}
