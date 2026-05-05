#pragma once

#include <cstdint>
#include <vector>

namespace apsp::common
{

class Graph
{
public:
    using Vertex = std::uint32_t;

    Graph() = default;
    Graph(std::uint32_t n);

    std::uint32_t getN();
    std::uint32_t getM();

    double getEdgeWeight(Vertex from, Vertex to);
    void updateEdgeWeight(Vertex from, Vertex to, double w);
private:
    std::uint32_t n, m;
    std::vector<std::vector<double>> edgeWeight;
};

} // namespace apsp::common
