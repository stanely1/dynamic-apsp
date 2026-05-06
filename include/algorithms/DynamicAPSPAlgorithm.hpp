#pragma once

#include <common/Path.hpp>
#include <memory>

namespace apsp::algorithms
{

class DynamicAPSPAlgorithm
{
public:
    virtual ~DynamicAPSPAlgorithm() = default;

    virtual double distance(common::Vertex from, common::Vertex to) = 0;
    virtual std::shared_ptr<common::Path> path(common::Vertex from, common::Vertex to) = 0;
    virtual void update(common::Vertex v, const common::WeightUpdateMap& in, const common::WeightUpdateMap& out) = 0;
};

} // namespace apsp::algorithms
