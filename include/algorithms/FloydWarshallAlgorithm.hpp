#pragma once

#include <algorithms/DynamicAPSPAlgorithm.hpp>
#include <common/Graph.hpp>

namespace apsp::algorithms
{

class FloydWarshallAlgorithm : public DynamicAPSPAlgorithm
{
public:
    FloydWarshallAlgorithm(std::uint32_t n);
    FloydWarshallAlgorithm(const common::Graph&);

    double distance(common::Vertex from, common::Vertex to) override;
    std::shared_ptr<common::Path> path(common::Vertex from, common::Vertex to) override;
    void update(common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out) override;

private:
    void initializeShortestPaths();

    common::Graph graph;
    std::vector<std::vector<std::shared_ptr<common::Path>>> shortestPaths;
};

} // namespace apsp::algorithms
