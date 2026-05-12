#include <common/Graph.hpp>
#include <common/Constants.hpp>
#include <cmath>

namespace apsp::common
{

Graph::Graph(std::uint32_t n) : n{n}, m{0u}, edgeWeight(n)
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

bool Graph::hasEdge(Vertex from, Vertex to) const
{
    return from < n and edgeWeight[from].contains(to);
}

double Graph::getEdgeWeight(Vertex from, Vertex to) const
{
    if (from >= n)
    {
        return UNREACHABLE;
    }
    if (from == to)
    {
        return 0.0;
    }
    return edgeWeight[from].contains(to) ? edgeWeight[from].at(to) : UNREACHABLE;
}

const VertexToWeightMap& Graph::getOutEdges(Vertex from) const
{
    static const VertexToWeightMap empty{};
    return from < n ? edgeWeight[from] : empty;
}

void Graph::updateEdgeWeight(Vertex from, Vertex to, double w)
{
    // assumption: only non-negative weights are supported
    // self-loops not allowed
    if (from == to or from >= n or to >= n or w < 0.0)
    {
        return;
    }

    if (std::isinf(w)) // delete edge
    {
        if (edgeWeight[from].contains(to))
        {
            edgeWeight[from].erase(to);
            --m;
        }
        return;
    }

    if (not edgeWeight[from].contains(to)) // insert new edge
    {
        ++m;
    }

    edgeWeight[from][to] = w;
}

void Graph::updateVertex(Vertex v, const VertexToWeightMap& in, const VertexToWeightMap& out)
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
