#include <common/Path.hpp>
#include <common/Constants.hpp>

namespace apsp::common
{

Path::Path(Vertex start, Vertex end)
: start{start},
  end{end},
  weight{UNREACHABLE},
  leftSubpath{nullptr},
  rightSubpath{nullptr}
{
}

std::vector<Vertex> Path::getVertices() const
{
    // right subpath should be empty if and only if path contains 1 vertex (start == end)
    if (rightSubpath == nullptr and start != end)
    {
        return {};
    }

    std::vector<Vertex> result{};
    for (auto* path{this}; path->rightSubpath != nullptr; path = path->rightSubpath.get())
    {
        result.push_back(path->start);
    }
    result.push_back(end);

    return result;
}

} // namespace apsp::common
