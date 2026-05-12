#pragma once

#include <cstdint>
#include <unordered_map>

namespace apsp::common
{

using Vertex = std::uint32_t;
using VertexToWeightMap = std::unordered_map<Vertex, double>;

} // namespace apsp::common
