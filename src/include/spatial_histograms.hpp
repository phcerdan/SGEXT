#ifndef SPATIAL_HISTOGRAMS_HPP
#define SPATIAL_HISTOGRAMS_HPP

#include "spatial_graph.hpp"
#include "histo.hpp"
#include <algorithm>
#include <cmath>

namespace SG {

/*
 * Compute degree of all nodes
 *
 * @param sg input spatial graph
 *
 * @return vector with degrees
 */
std::vector<unsigned int> compute_degrees(const SG::GraphAL & sg)
{
    std::vector<unsigned int> degrees;
    const auto verts = boost::vertices(sg);
    for (auto vi = verts.first; vi != verts.second; ++vi)
    {
        degrees.push_back(boost::degree(*vi, sg));
    }
    return degrees;
}

/**
 * Create histogram of degrees.
 * @sa compute_distances
 *
 * @param degrees input vector of degrees
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 * bins should be greater than max_degrees + 1
 *
 * @return  histogram of degrees
 */
histo::Histo<double> histogram_degrees(
        const std::vector<unsigned int> & degrees,
        size_t bins = 0)
{
	auto max_degree = *std::max_element(std::begin(degrees), std::end(degrees) );
    // Generate breaks to get middle of the bins to be the integer
    // value of the degree
    histo::Histo<double> hist_degrees;
    if(bins == 0)
        hist_degrees = histo::Histo<double>(degrees,
                histo::GenerateBreaksFromRangeAndBins(
                    -0.5, max_degree + 0.5, max_degree + 1)
                );
    else
        hist_degrees = histo::Histo<double>(degrees,
                histo::GenerateBreaksFromRangeAndBins(
                    -0.5, max_degree + 0.5, bins)
                );
    hist_degrees.name = "degrees";
    return hist_degrees;
}
/**
 * Compute end to end distances of nodes
 * It iterates over all edges and compute distance(target,source)
 *
 * @param sg input spatial graph
 *
 * @return vector with distances.
 */
std::vector<double> compute_distances(const SG::GraphAL & sg)
{
    std::vector<double> ete_distances;
    const auto edges = boost::edges(sg);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        auto source = boost::source(*ei, sg);
        auto target = boost::target(*ei, sg);
        const auto & source_pos = sg[source].pos;
        const auto & target_pos = sg[target].pos;
        ete_distances.emplace_back(
                ArrayUtilities::distance(target_pos, source_pos)
                );
    }
    return ete_distances;
}

/**
 * Create histogram of End to End distances between nodes.
 * @sa compute_distances
 *
 * @param distances input vector of distances
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 *
 * @return histogram
 */
histo::Histo<double> histogram_distances(const std::vector<double> & distances, size_t bins = 0)
{
	auto max_distance = *std::max_element(std::begin(distances), std::end(distances) );
    histo::Histo<double> hist_distances;
    if(bins == 0)
        hist_distances = histo::Histo<double>(distances);
    else
        hist_distances = histo::Histo<double>(distances,
                histo::GenerateBreaksFromRangeAndBins(
                    0.0, max_distance, bins)
                );
    hist_distances.name = "distances";
    return hist_distances;
}

/**
 * Compute angles between adjacent edges in sg
 *
 * @param sg input spatial graph
 * @param ignore_parallel_edges flag to don't compute angles for parallel edges.
 * if false, parallel edges will have an angle of 0.0.
 *
 * @return  vector with angles.
 */
std::vector<double> compute_angles(const SG::GraphAL & sg,
        bool ignore_parallel_edges = true)
{
    std::vector<double> ete_angles;
    const auto verts = boost::vertices(sg);
    // From http://www.boost.org/doc/libs/1_66_0/libs/graph/doc/IncidenceGraph.html
    // It is guaranteed that given: e=out_edge(v); then source(e) == v.
    for (auto vi = verts.first; vi != verts.second; ++vi)
    {
        // Don't analyze degree 2 nodes (they are only left to mark self-loops.
        // Degree 0 and 1 won't be computed even without this guard.
        // auto degree =  boost::out_degree(*vi,sg);
        // if (degree < 3)
        //     continue;
        const auto out_edges = boost::out_edges(*vi, sg);
        for (auto ei1 = out_edges.first; ei1 != out_edges.second; ++ei1) {
            auto source = boost::source(*ei1, sg); // = *vi
            auto target1 = boost::target(*ei1, sg);
            // Copy edge iterator and plus one (to avoid compare the edge with itself)
            auto ei2 = ei1;
            ei2++;
            for (; ei2 != out_edges.second; ++ei2)
            {
                auto target2 = boost::target(*ei2, sg);
                // Don't compute angle on parallel edges
                // WARNING: do not check target2 == source
                // source(ei2) is guaranteed (by out_edges) to be equal to source(ei1)
                if(ignore_parallel_edges && target2 == target1){
                    std::cout << "parallel" << std::endl;
                    continue;
                }

                ete_angles.emplace_back(
                        ArrayUtilities::angle(
                            ArrayUtilities::minus(sg[target1].pos, sg[source].pos),
                            ArrayUtilities::minus(sg[target2].pos, sg[source].pos)
                            )
                        );
            }
        }
    }
    return ete_angles;
}

/**
 * Histogram between -pi, pi
 * @sa compute_angles
 *
 * @param angles input data
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 *
 * @return histo
 */
histo::Histo<double> histogram_angles(const std::vector<double> & angles, size_t bins = 0)
{
    constexpr auto pi = 3.14159265358979323846;
    histo::Histo<double> hist_angles;
    if(bins == 0)
        hist_angles = histo::Histo<double>(angles, std::make_pair(-pi, pi));
    else
        hist_angles = histo::Histo<double>(angles,
                histo::GenerateBreaksFromRangeAndBins(
                    -pi, pi, bins)
                );
    hist_angles.name = "angles";
    return hist_angles;
}

std::vector<double> compute_cosines(const std::vector<double> & angles)
{
    std::vector<double> cosines(angles.size());
    // cosines.resize(angles.size())
    std::transform(angles.begin(), angles.end(), cosines.begin(),[](const double & a){return std::cos(a);});
    return cosines;
}

/**
 * Histogram between -1.0, 1.0
 * @sa compute_cosines
 *
 * @param cosines input data
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 * @return histo
 */
histo::Histo<double> histogram_cosines(const std::vector<double> & cosines, size_t bins = 0)
{
    histo::Histo<double> hist_cosines;
    if(bins == 0)
        hist_cosines = histo::Histo<double>(cosines, std::make_pair(-1.0, 1.0));
    else
        hist_cosines = histo::Histo<double>(cosines,
                histo::GenerateBreaksFromRangeAndBins(
                    -1.0, 1.0, bins)
                );
    hist_cosines.name = "cosines";
    return hist_cosines;
}
} // end namespace
#endif
