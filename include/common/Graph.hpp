#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace apsp::common
{

class Graph
{
public:
    using Vertex = std::uint32_t;
    using WeightUpdateMap = std::unordered_map<Vertex, double>;

    Graph() = default;
    Graph(std::uint32_t n);

    std::uint32_t getN();
    std::uint32_t getM();

    double getEdgeWeight(Vertex from, Vertex to);
    void updateEdgeWeight(Vertex from, Vertex to, double w);
    void updateVertex(Vertex v, const WeightUpdateMap& in, const WeightUpdateMap& out);
private:
    std::uint32_t n, m;
    std::vector<std::vector<double>> edgeWeight;
};

} // namespace apsp::common
