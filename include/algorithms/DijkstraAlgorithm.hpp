#pragma once

#include <algorithms/DynamicAPSPAlgorithm.hpp>
#include <common/Graph.hpp>

namespace apsp::algorithms
{

class DijkstraAlgorithm : public DynamicAPSPAlgorithm
{
public:
    DijkstraAlgorithm(std::uint32_t n);

    double distance(common::Vertex from, common::Vertex to) override;
    std::shared_ptr<common::Path> path(common::Vertex from, common::Vertex to) override;
    void update(common::Vertex v, const common::WeightUpdateMap& in, const common::WeightUpdateMap& out) override;

private:
    void calculateSingleSourceShortestPaths(common::Vertex source);

    common::Graph graph;
    std::vector<std::vector<std::shared_ptr<common::Path>>> shortestPaths;
};

} // namespace apsp::algorithms
