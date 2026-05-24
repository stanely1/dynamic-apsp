#pragma once

#include <algorithms/DynamicAPSPAlgorithm.hpp>
#include <algorithms/structures/PathPriorityQueue.hpp>
#include <common/Graph.hpp>

namespace apsp::algorithms
{

class DemetrescuItalianoAlgorithm : public DynamicAPSPAlgorithm
{
public:
    DemetrescuItalianoAlgorithm(std::uint32_t n);

    double distance(common::Vertex from, common::Vertex to) override;
    std::shared_ptr<common::Path> path(common::Vertex from, common::Vertex to) override;
    void update(common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out) override;

private:
    void updateImpl(common::Vertex v);
    void cleanup(common::Vertex v);
    void fixup(common::Vertex v);

    common::Graph graph;
    std::vector<std::vector<structures::PathPriorityQueue>> historicalPaths; // P* in paper
    std::vector<std::vector<structures::PathPriorityQueue>> locallyHistoricalPaths; // P in paper

    std::uint32_t currentTime;
    std::vector<std::uint32_t> lastUpdateTime;
};

} // namespace apsp::algorithms
