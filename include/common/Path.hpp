#pragma once

#include <common/Types.hpp>
#include <memory>
#include <vector>

namespace apsp::common
{

struct Path
{
    Path(Vertex start, Vertex end);

    std::vector<Vertex> getVertices() const;

    Vertex start, end;
    double weight;
    std::shared_ptr<Path> leftSubpath, rightSubpath;
};

} // namespace apsp::common
