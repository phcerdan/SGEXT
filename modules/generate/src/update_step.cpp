#include "update_step.hpp"
// #include "array_utilities.hpp"
// #include "boundary_conditions.hpp"
// #include "generate_common.hpp"
// #include "spatial_graph.hpp"
// #include "spatial_graph_utilities.hpp"
// #include "spatial_node.hpp"
// #include <iostream>
//
namespace SG {

void update_step_with_distance_and_cosine_histograms::clear_stored_parameters(
        std::vector<double> &old_distances,
        std::vector<double> &old_cosines,
        std::vector<double> &new_distances,
        std::vector<double> &new_cosines) const {
    old_distances.clear();
    old_cosines.clear();
    new_distances.clear();
    new_cosines.clear();
}

void update_step_with_distance_and_cosine_histograms::undo(
        // in/out parameters
        Histogram &histo_distances,
        Histogram &histo_cosines,
        // out parameters
        std::vector<double> &old_distances,
        std::vector<double> &old_cosines,
        std::vector<double> &new_distances,
        std::vector<double> &new_cosines) const {
    this->update_distances_histogram(histo_distances, new_distances,
                                     old_distances);
    this->update_cosines_histogram(histo_cosines, new_cosines, old_cosines);
    this->clear_stored_parameters(old_distances, old_cosines, new_distances,
                                  new_cosines);
}

void update_step_with_distance_and_cosine_histograms::
        update_distances_histogram(
                Histogram &histo_distances,
                const std::vector<double> &old_distances,
                const std::vector<double> &new_distances) const {
    // std::cout << "OLD_DISTANCES_UPDATING_HISTOGRAM: " << old_distances.size()
    //           << std::endl;
    // std::cout << "NEW_DISTANCES_UPDATING_HISTOGRAM: " << new_distances.size()
    //           << std::endl;
    for (const auto &dist : new_distances) {
        const auto bin = histo_distances.IndexFromValue(dist);
        // std::cout << "bin: " << bin << "; new_distance: " << dist <<
        // std::endl;
        histo_distances.counts[bin]++;
    }
    for (const auto &dist : old_distances) {
        const auto bin = histo_distances.IndexFromValue(dist);
        // std::cout << "bin: " << bin << "; old_distance: " << dist <<
        // std::endl;
        histo_distances.counts[bin]--;
    }
}
void update_step_with_distance_and_cosine_histograms::update_cosines_histogram(
        Histogram &histo_cosines,
        const std::vector<double> &old_cosines,
        const std::vector<double> &new_cosines) const {

    for (const auto &cosine : new_cosines) {
        histo_cosines.counts[histo_cosines.IndexFromValue(cosine)]++;
    }
    for (const auto &cosine : old_cosines) {
        histo_cosines.counts[histo_cosines.IndexFromValue(cosine)]--;
    }
}
void update_step_with_distance_and_cosine_histograms::print(
        std::ostream &os) const {
    os << "old_distances: " << std::endl;
    for (const auto &v : old_distances_) {
        os << v << ", ";
    }
    os << std::endl;
    os << "new_distances: " << std::endl;
    for (const auto &v : new_distances_) {
        os << v << ", ";
    }
    os << std::endl;
    os << "old_cosines: " << std::endl;
    for (const auto &v : old_cosines_) {
        os << v << ", ";
    }
    os << std::endl;
    os << "new_cosines: " << std::endl;
    for (const auto &v : new_cosines_) {
        os << v << ", ";
    }
    os << std::endl;
}

} // namespace SG

