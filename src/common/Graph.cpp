#include <common/Graph.hpp>
#include <common/Constants.hpp>
#include <cmath>

namespace apsp::common
{

Graph::Graph(std::uint32_t n) : n{n}, m{0u}, edgeWeight(n, std::vector<double>(n, UNREACHABLE))
{
}

std::uint32_t Graph::getN() const
{
    return n;
}

std::uint32_t Graph::getM() const
{
    return m;
}

double Graph::getEdgeWeight(Vertex from, Vertex to) const
{
    if (from >= n or to >= n)
    {
        return UNREACHABLE;
    }
    return edgeWeight[from][to];
}

void Graph::updateEdgeWeight(Vertex from, Vertex to, double w)
{
    // assumption: only non-negative weights are supported
    if (from >= n or to >= n or w < 0)
    {
        return;
    }

    auto& weight{edgeWeight[from][to]};
    if (std::isinf(weight))
    {
        ++m;
    }
    if (std::isinf(w))
    {
        --m;
    }
    weight = w;
}

void Graph::updateVertex(Vertex v, const WeightUpdateMap& in, const WeightUpdateMap& out)
{
    for (const auto& [u, w] : in)
    {
        updateEdgeWeight(u, v, w);
    }
    for (const auto& [u, w] : out)
    {
        updateEdgeWeight(v, u, w);
    }
}

} // namespace apsp::common
