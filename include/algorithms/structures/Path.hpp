#pragma once

#include <common/Path.hpp>
#include <cstdint>
#include <memory>
#include <unordered_set>

namespace apsp::algorithms::structures
{

struct Path : public common::Path
{
    using common::Path::Path;

    std::uint64_t id{0u};

    using PathSet = std::unordered_set<std::shared_ptr<Path>>;
    PathSet historicalPreExtensions{}; // L* in paper
    PathSet locallyHistoricalPreExtensions{}; // L in paper
    PathSet historicalPostExtensions{}; // R* in paper
    PathSet locallyHistoricalPostExtensions{}; // R in paper
};

} // namespace apsp::algorithms::structures
