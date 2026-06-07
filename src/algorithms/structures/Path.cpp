#include <algorithms/structures/Path.hpp>

namespace apsp::algorithms::structures
{

Path::ExtendedWeight Path::extendedWeight() const
{
    return {weight, id};
}

} // namespace apsp::algorithms::structures
