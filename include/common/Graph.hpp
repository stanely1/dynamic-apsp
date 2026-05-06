#pragma once

#include <common/Types.hpp>
#include <cstdint>
#include <vector>

namespace apsp::common
{

class Graph
{
public:
    Graph() = default;
    Graph(std::uint32_t n);

    std::uint32_t getN() const;
    std::uint32_t getM() const;

    double getEdgeWeight(Vertex from, Vertex to) const;
    void updateEdgeWeight(Vertex from, Vertex to, double w);
    void updateVertex(Vertex v, const WeightUpdateMap& in, const WeightUpdateMap& out);

private:
    std::uint32_t n, m;
    std::vector<std::vector<double>> edgeWeight;
};

} // namespace apsp::common
