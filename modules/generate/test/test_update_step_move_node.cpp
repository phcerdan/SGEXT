#include "common_types.hpp"
#include "generate_common.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "update_step_move_node.hpp"
#include "gmock/gmock.h"

struct UpdateStepMoveNodeFixture : public ::testing::Test {
    void SetUp() override {
        this->CreateGraph();
        this->CreateHistograms();
    }
    void CreateGraph() {
        g = SG::GraphType(3);
        p0 = SG::PointType{{0.0, 0.0, 0.0}};
        p1 = SG::PointType{{0.5, 0.0, 0.0}};
        p2 = SG::PointType{{0.5, 0.5, 0.0}};
        g[0].pos = p0;
        g[1].pos = p1;
        g[2].pos = p2;
        boost::add_edge(0, 1, SG::SpatialEdge(), g);
        boost::add_edge(1, 2, SG::SpatialEdge(), g);
    }
    void CreateHistograms() {
        // Create histograms involved
        auto all_distances = SG::get_all_end_to_end_distances_of_edges(g, bc);
        histo_distances = SG::Histogram(
                all_distances,
                histo::GenerateBreaksFromRangeAndBins(
                        0.0, sqrt(2.0) + std::numeric_limits<double>::epsilon(),
                        histo_bins_distances));
        std::vector<double> dummy_histo_data;
        histo_cosines = SG::Histogram(dummy_histo_data,
                                      histo::GenerateBreaksFromRangeAndBins(
                                              -pi, pi, histo_bins_cosines));
    }
    const double pi = atan(1.0) * 4.0;
    SG::GraphType g;
    SG::PointType p0, p1, p2;
    size_t histo_bins_distances = 10;
    size_t histo_bins_cosines = 10;
    SG::Histogram histo_distances;
    SG::Histogram histo_cosines;
    ArrayUtilities::boundary_condition bc =
            ArrayUtilities::boundary_condition::PERIODIC;
};

TEST_F(UpdateStepMoveNodeFixture, constructor_works) {
    auto step = SG::update_step_move_node(g, histo_distances, histo_cosines);
    histo_distances.PrintBreaksAndCounts(std::cout);
}

TEST_F(UpdateStepMoveNodeFixture, perform_works) {
    auto step = SG::update_step_move_node(g, histo_distances, histo_cosines);
    step.perform();
    histo_distances.PrintBreaksAndCounts(std::cout);
}
// TEST_F(UpdateStepMoveNodeFixture, undo_works) {
//     auto step = SG::update_step_move_node(g, histo_distances, histo_cosines);
//     step.perform();
//     step.undo();
// }
