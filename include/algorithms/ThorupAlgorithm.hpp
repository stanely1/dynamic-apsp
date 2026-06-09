#pragma once

#include <algorithms/DynamicAPSPAlgorithm.hpp>
#include <algorithms/DemetrescuItalianoAlgorithm.hpp>
#include <common/Graph.hpp>

namespace apsp::algorithms
{

class ThorupAlgorithm : public DynamicAPSPAlgorithm
{
public:
    ThorupAlgorithm() = default;
    ThorupAlgorithm(std::uint32_t n);
    ThorupAlgorithm(const common::Graph&);

    std::string name() override;

    double distance(common::Vertex from, common::Vertex to) override;
    std::shared_ptr<common::Path> path(common::Vertex from, common::Vertex to) override;
    void update(common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out) override;

private:
    using PathPriorityQueueMatrix = std::vector<std::vector<structures::PathPriorityQueue>>;

    enum UpdateType
    {
        INSERT,
        DELETE
    };

    void initializeDeleteVertexEdgeWeights();
    void initializeBaseLevel(const common::Graph& graph);
    void initializeSingleVertexPaths(std::uint8_t level);
    void initializeSingleEdgePath(std::uint8_t level, common::Vertex from, common::Vertex to);
    void activateLevel(std::uint8_t level);
    void deactivateLevel(std::uint8_t level);
    void updateImpl(UpdateType type, common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out);
    void cleanup(std::uint8_t level, common::Vertex v);
    void fixup(std::uint8_t level, common::Vertex v);
    void calculateLocallyHistoricalPaths(std::uint8_t level);

    common::VertexToWeightMap deleteVertexEdgeWeights;

    // level 0 = base (current)
    std::unordered_map<std::uint8_t, common::Graph> graph;
    std::unordered_map<std::uint8_t, PathPriorityQueueMatrix> historicalPaths;
    std::unordered_map<std::uint8_t, PathPriorityQueueMatrix> locallyHistoricalPaths;

    std::uint32_t currentTime;
    std::vector<std::uint32_t> insertionTime;
    std::unordered_map<std::uint8_t, std::uint32_t> levelTime;
};

} // namespace apsp::algorithms
