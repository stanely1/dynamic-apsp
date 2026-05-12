#pragma once

#include <common/Types.hpp>
#include <cstdint>
#include <unordered_map>
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

    bool hasEdge(Vertex from, Vertex to) const;
    double getEdgeWeight(Vertex from, Vertex to) const;
    const VertexToWeightMap& getOutEdges(Vertex from) const;
    void updateEdgeWeight(Vertex from, Vertex to, double w);
    void updateVertex(Vertex v, const VertexToWeightMap& in, const VertexToWeightMap& out);

private:
    std::uint32_t n, m;
    std::vector<VertexToWeightMap> edgeWeight;
};

} // namespace apsp::common
