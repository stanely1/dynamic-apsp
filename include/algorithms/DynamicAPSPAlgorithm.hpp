#pragma once

#include <common/Path.hpp>
#include <memory>
#include <string>

namespace apsp::algorithms
{

class DynamicAPSPAlgorithm
{
public:
    virtual ~DynamicAPSPAlgorithm() = default;

    virtual std::string name() = 0;

    virtual double distance(common::Vertex from, common::Vertex to) = 0;
    virtual std::shared_ptr<common::Path> path(common::Vertex from, common::Vertex to) = 0;
    virtual void update(common::Vertex v, const common::VertexToWeightMap& in, const common::VertexToWeightMap& out) = 0;
};

} // namespace apsp::algorithms
