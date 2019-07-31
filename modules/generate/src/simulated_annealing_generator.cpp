#include "simulated_annealing_generator.hpp"
#include "cramer_von_mises_test.hpp"
#include "cumulative_distribution_functions.hpp"
#include "degree_sequences.hpp"
#include "degree_viger_generator.hpp"
#include "generate_common.hpp"
#include "rng.hpp"
#include <chrono>

namespace SG {
void simulated_annealing_generator::set_default_parameters() {
    // TODO change default number of bins
    const auto num_vertices = boost::num_vertices(graph_);
    const size_t num_bins_ete_distances = 100;
    const size_t num_bins_cosines = 100;
    this->init_histograms(num_bins_ete_distances, num_bins_cosines);
    transition_parameters.energy_initial = this->compute_energy();
    transition_parameters.energy = transition_parameters.energy_initial;
    // TODO node_density and all other parameters has to be defined/chosen by
    // the user
    auto &psp = physical_scaling_parameters;
    auto &etep = end_to_end_distances_distribution_parameters;
    psp.length_scaling_factor =
            std::pow(num_vertices / psp.node_density, 1 / 3.0);
    etep.normalized_normal_mean =
            etep.physical_normal_mean / psp.length_scaling_factor;
    etep.set_normalized_log_std_deviation(etep.normalized_normal_mean,
                                          etep.normalized_normal_std_deviation);
    etep.set_normalized_log_mean(etep.normalized_normal_mean,
                                 etep.normalized_log_std_deviation);
    print(std::cout);
}
simulated_annealing_generator::simulated_annealing_generator(
        const size_t &num_vertices)
        : simulated_annealing_generator() {
    this->init_graph_degree(num_vertices);
    this->init_graph_vertex_positions();
    this->set_default_parameters();
}
simulated_annealing_generator::simulated_annealing_generator(
        const GraphType &input_graph)
        : graph_(input_graph),
          step_move_node_(graph_, histo_ete_distances_, histo_cosines_),
          step_swap_edges_(graph_, histo_ete_distances_, histo_cosines_) {
    this->set_default_parameters();
}
void simulated_annealing_generator::init_graph_degree(
        const size_t &num_vertices) {
    const double q = 1.0 / (degree_distribution_parameters.mean - 2.0);
    const size_t min_degree_allowed = degree_distribution_parameters.min_degree;
    const size_t max_degree_allowed = degree_distribution_parameters.max_degree;

    // Generate a degree_sequence
    std::vector<int> degree_sequence;
    size_t sum = 0;
    const size_t max_iterations = 20;
    size_t iteration = 0;
    do {
        degree_sequence =
                generate_degree_sequence_geometric_distribution_bounded(
                        num_vertices, q, min_degree_allowed,
                        max_degree_allowed);
        sum = std::accumulate(std::begin(degree_sequence),
                              std::end(degree_sequence), 0);
        iteration++;
    } while (sum % 2 != 0 && iteration < max_iterations);
    if (iteration >= max_iterations) {
        throw std::runtime_error("init_graph_degree error. Not able to "
                                 "generate a degree sequence where "
                                 "the sum of degrees is an even number.");
    }
    // Using the valid degree_sequence, create the graph.
    graph_ = SG::create_graph_from_degree_sequence(degree_sequence);
    assert(boost::num_vertices(graph_) == std::size(degree_sequence));
    assert(boost::num_edges(graph_) > 0);
    assert(boost::num_edges(graph_) <= sum);
}
void simulated_annealing_generator::init_graph_vertex_positions() {

    auto [vi, vi_end] = boost::vertices(graph_);
    for (; vi != vi_end; ++vi) {
        graph_[*vi].pos = RNG::random_pos(domain_parameters.domain);
    }
}

void simulated_annealing_generator::init_histogram_ete_distances(
        const size_t &num_bins) {
    std::vector<double> dummy_data;
    const double min_distance = 0.0;
    // Assuming {0, 0, 0} is a corner of the simulation box:
    const double max_distance = ArrayUtilities::norm(domain_parameters.domain);
    std::vector<double> breaks = histo::GenerateBreaksFromRangeAndBins(
            min_distance, max_distance, num_bins);
    histo_ete_distances_ = Histogram(dummy_data, breaks);
}

void simulated_annealing_generator::
        populate_target_cumulative_distro_histo_ete_distances(
                const std::vector<double> &histo_centers) {
    auto &F = target_cumulative_distro_histo_ete_distances_;
    const auto bins = std::size(histo_centers);
    F.resize(bins);
    apply_distro(
            histo_centers, F,
            [&distro_parameters =
                     this->end_to_end_distances_distribution_parameters,
             &bins = bins](const double &x) -> double {
                return (bins * cumulative_distribution_lognormal(
                                       x, distro_parameters.normalized_log_mean,
                                       distro_parameters
                                               .normalized_log_std_deviation) +
                        0.5);
            });
}
void simulated_annealing_generator::
        populate_target_cumulative_distro_histo_cosines(
                const std::vector<double> &histo_centers) {
    auto &F = target_cumulative_distro_histo_cosines_;
    const auto bins = std::size(histo_centers);
    F.resize(bins);
    apply_distro(
            histo_centers, F,
            [&distro_parameters =
                     this->cosine_directors_distribution_parameters,
             &bins = bins](const double &x) -> double {
                return (bins * cumulative_distribution_truncated_power_series_3(
                                       x, distro_parameters.b1,
                                       distro_parameters.b2,
                                       distro_parameters.b3) +
                        0.5);
            });
}

void simulated_annealing_generator::init_histogram_cosines(
        const size_t &num_bins) {
    std::vector<double> dummy_data;
    const double min_cosine = -1.0;
    const double max_cosine = 1.0;
    std::vector<double> breaks = histo::GenerateBreaksFromRangeAndBins(
            min_cosine, max_cosine, num_bins);
    histo_cosines_ = Histogram(dummy_data, breaks);
}

void simulated_annealing_generator::populate_histogram_ete_distances() {
    histo_ete_distances_.ResetCounts();
    histo_ete_distances_.FillCounts(get_all_end_to_end_distances_of_edges(
            graph_, domain_parameters.boundary_condition));
}

void simulated_annealing_generator::populate_histogram_cosines() {
    histo_cosines_.ResetCounts();
    histo_cosines_.FillCounts(get_all_cosine_directors_between_connected_edges(
            graph_, domain_parameters.boundary_condition));
}
void simulated_annealing_generator::init_histograms(
        const size_t &num_bins_ete_distances, const size_t &num_bins_cosines) {
    this->init_histogram_ete_distances(num_bins_ete_distances);
    // if another distribution is required, change the vector
    // target_cumulative_distro_xxx
    this->populate_target_cumulative_distro_histo_ete_distances(
            histo_ete_distances_.ComputeBinCenters());
    this->populate_histogram_ete_distances();
    this->init_histogram_cosines(num_bins_cosines);
    this->populate_target_cumulative_distro_histo_cosines(
            histo_cosines_.ComputeBinCenters());
    this->populate_histogram_cosines();
}
void simulated_annealing_generator::engine() {

    const auto t_start = std::chrono::high_resolution_clock::now();
    size_t steps = 0;

    while (transition_parameters.consecutive_failures !=
                   transition_parameters.MAX_CONSECUTIVE_FAILURES &&
           steps != transition_parameters.MAX_ENGINE_ITERATIONS &&
           transition_parameters.energy >=
                   transition_parameters.ENERGY_CONVERGENCE) {
        if (verbose) {
            std::cout << "Step #: " << steps << std::endl;
        }
        simulated_annealing_generator::transition transition;

        if (RNG::rand01() <
            transition_parameters.UPDATE_STEP_MOVE_NODE_PROBABILITY) {
            if (verbose) {
                std::cout << "Step type: move_node" << std::endl;
            }
            step_move_node_.randomize();
            step_move_node_.perform();
            /*Because a edge could have leng zero after a movement (2 nodes same
             *pos). A std::logic_error could be thrown from
             *selectEdges(SpatialNode::distance) or from computeMovement
             *(distance(array<double,3>) */
            // PHC 2017, remove this try when remove exception.
            // Change exception for std::optional (c++17)
            // or boost::optional from version >= 1.63
            transition = check_transition();
            // TODO REMOVE
            if (verbose) {
                std::cout << "von-mises_distances= " << energy_ete_distances()
                          << " ; ";
                std::cout << "von-mises_cosines= " << energy_cosines()
                          << std::endl;
                std::cout << "******HISTOGRAMS AT STEP " << steps
                          << " ***********" << std::endl;
                std::cout << "******ETE_DISTANCES***********" << std::endl;
                histo_ete_distances_.PrintBreaksAndCounts(std::cout);
                std::cout << "******COSINES***********" << std::endl;
                histo_cosines_.PrintBreaksAndCounts(std::cout);
            }
            if (transition == transition::REJECTED) {
                step_move_node_.undo();
            } else if (transition == transition::ACCEPTED ||
                       transition == transition::ACCEPTED_HIGH_TEMP) {
                step_move_node_.update_graph();
            }

        } else {
            if (verbose) {
                std::cout << "Step type: swap_edges" << std::endl;
            }
            step_swap_edges_.randomize();
            step_swap_edges_.perform();
            transition = check_transition();
            // TODO REMOVE
            if (verbose) {
                std::cout << "von-mises_distances= " << energy_ete_distances()
                          << " ; ";
                std::cout << "von-mises_cosines= " << energy_cosines()
                          << std::endl;
                std::cout << "******HISTOGRAMS AT STEP " << steps
                          << " ***********" << std::endl;
                std::cout << "******ETE_DISTANCES***********" << std::endl;
                histo_ete_distances_.PrintBreaksAndCounts(std::cout);
                std::cout << "******COSINES***********" << std::endl;
                histo_cosines_.PrintBreaksAndCounts(std::cout);
                step_swap_edges_.print(std::cout);
            }
            if (transition == transition::REJECTED) {
                // TODO remove
                if (verbose) {
                    std::cout
                            << "von-mises_distances= " << energy_ete_distances()
                            << " ; ";
                    std::cout << "von-mises_cosines= " << energy_cosines()
                              << std::endl;
                }
                step_swap_edges_.undo();
            } else if (transition == transition::ACCEPTED ||
                       transition == transition::ACCEPTED_HIGH_TEMP) {
                step_swap_edges_.update_graph();
            }
        }
        steps++;
        if (verbose) {
            std::string transition_str;
            if (transition == transition::REJECTED) {
                transition_str = "REJECTED";
            } else if (transition == transition::ACCEPTED) {
                transition_str = "ACCEPTED";
            } else if (transition == transition::ACCEPTED_HIGH_TEMP) {
                transition_str = "ACCEPTED_HIGH_TEMP";
            }
            std::cout << "Transition: " + transition_str << std::endl;
            std::cout << "*******************end**************" << std::endl;
            std::cout << std::endl;
        }
    }

    const auto t_final = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = t_final - t_start;
    transition_parameters.time_elapsed = elapsed.count();
} // namespace SG

double simulated_annealing_generator::energy_ete_distances() const {
    return cramer_von_mises_test(histo_ete_distances_.counts,
                                 target_cumulative_distro_histo_ete_distances_);
}
double simulated_annealing_generator::energy_cosines() const {
    return cramer_von_mises_test(histo_cosines_.counts,
                                 target_cumulative_distro_histo_cosines_);
}
double simulated_annealing_generator::compute_energy() const {

    const double test_ete_distances = energy_ete_distances();
    const double test_cosines = energy_cosines();
    return test_ete_distances + test_cosines;
}

simulated_annealing_generator::transition
simulated_annealing_generator::check_transition() {

    const double energy_new = compute_energy();
    const double energy_diff = energy_new - transition_parameters.energy;
    if (verbose) {
        std::cout << "energy_new : " << energy_new << std::endl;
        std::cout << "energy_diff: " << energy_diff << std::endl;
    }
    if (transition_parameters.accepted_transitions == 0 &&
        transition_parameters.consecutive_failures == 0) {
        // transition_parameters.temp_initial = std::abs(energy_diff /
        // log(0.5));
        transition_parameters.temp_initial =
                transition_parameters.energy_initial /
                boost::num_vertices(graph_);
        transition_parameters.temp_current = transition_parameters.temp_initial;
        //      //alternative provisional definition; Dependance on num of nodes
        //      of the graph. if(T_actual_==0) T_actual_=energy_new/
        //      num_vertices(network_.g);
    }
    // Keep always a probability of jump-transition. Why?
    // if(T_initial_ !=0 && T_actual_<T_minimum_) T_actual_=T_minimum_;

    if (energy_diff <= 0.0) {
        // Transition Accepted
        transition_parameters.energy = energy_new;
        transition_parameters.accepted_transitions++;
        transition_parameters.temp_current *=
                transition_parameters.temp_cooling_rate;
        transition_parameters.consecutive_failures = 0;
        return transition::ACCEPTED;
    } else {
        // energy_new-Eold_ is negative in this case.
        // T is a decreasing function of accepted_transitions. T_parameter_
        // should be tweaked to enhance convergence. if the expression is lesser
        // than a random number[0,1), is accepted.

        // double energy_dif = Eold_ - energy_new; //ALWAYS NEGATIVE IN THIS
        // CASE. double prob_transition =exp(energy_dif/T_actual_) ; double
        // total_test=exp( (Eold_ -
        // energy_new)*exp(T_decay_rate_*accepted_transitions_) );
        //      std::cout<<energy_dif<<" "<<prob_transition<<std::endl;

        // energy_new>energy_
        if (RNG::random_bool(
                    exp(-energy_diff / transition_parameters.temp_current))) {
            transition_parameters.energy = energy_new;
            transition_parameters.accepted_transitions++;
            transition_parameters.high_temp_transitions++;
            transition_parameters.temp_current *=
                    transition_parameters.temp_cooling_rate;
            transition_parameters.consecutive_failures = 0;
            return transition::ACCEPTED_HIGH_TEMP;
        } else {
            // Call undo? or call undo if compare()==0 in other function?
            transition_parameters.consecutive_failures++;
            transition_parameters.total_failures++;
            return transition::REJECTED;
        }
    }
}

void simulated_annealing_generator::print(std::ostream &os, int spaces) {
    os << "%/************NETWORK PARAMETERS*****************/" << '\n'
       << '\n'
       << std::left << std::setw(spaces)
       << "Nodes_N= " << boost::num_vertices(graph_) << '\n'
       << std::left << std::setw(spaces)
       << "Edges_E= " << boost::num_edges(graph_) << '\n'
       << std::left << std::setw(spaces)
       << "SpatialDomain= " << domain_parameters.domain[0] << " , "
       << domain_parameters.domain[1] << " , " << domain_parameters.domain[2]
       << '\n'
       << "%/************PHYSICAL SCALING PARAMETERS*****************/" << '\n'
       << '\n'
       << std::left << std::setw(spaces)
       << "PhysicalNodeDensity= " << physical_scaling_parameters.node_density
       << '\n'
       << std::left << std::setw(spaces)
       << "ScaleFactor_S= " << physical_scaling_parameters.length_scaling_factor
       << '\n'
       << "%/************END_TO_END DISTANCES DITRIBUTION "
          "PARAMETERS*****************/"
       << '\n'
       << std::left << std::setw(spaces) << "physical_normal_mean= "
       << end_to_end_distances_distribution_parameters.physical_normal_mean
       << '\n'
       << std::left << std::setw(spaces) << "physical_normal_std_deviation= "
       << end_to_end_distances_distribution_parameters
                    .physical_normal_std_deviation
       << '\n'
       << std::left << std::setw(spaces) << "normalized_mean= "
       << end_to_end_distances_distribution_parameters.normalized_normal_mean
       << '\n'
       << std::left << std::setw(spaces) << "normalized_std_deviation= "
       << end_to_end_distances_distribution_parameters
                    .normalized_normal_std_deviation
       << '\n'
       << std::left << std::setw(spaces) << "normalized_log_mean= "
       << end_to_end_distances_distribution_parameters.normalized_log_mean
       << '\n'
       << std::left << std::setw(spaces) << "normalized_log_std_deviation= "
       << end_to_end_distances_distribution_parameters
                    .normalized_log_std_deviation
       << '\n'
       << "%/************DEGREE DISTRIBUTION PARAMETERS*****************/"
       << '\n'
       << '\n'
       << std::left << std::setw(spaces)
       << "MeanDegree_Z= " << degree_distribution_parameters.mean << '\n'
       << std::left << std::setw(spaces)
       << "min_degree= " << degree_distribution_parameters.min_degree << '\n'
       << std::left << std::setw(spaces)
       << "max_degree= " << degree_distribution_parameters.max_degree << '\n'
       << '\n'
       << "%/************DIRECTOR COSINES DISTRIBUTION "
          "PARAMETERS*****************/"
       << '\n'
       << std::left << std::setw(spaces)
       << "b1= " << cosine_directors_distribution_parameters.b1 << '\n'
       << std::left << std::setw(spaces)
       << "b2= " << cosine_directors_distribution_parameters.b2 << '\n'
       << std::left << std::setw(spaces)
       << "b3= " << cosine_directors_distribution_parameters.b3 << '\n'
       << '\n'
       << "%/************HISTOGRAM BINS RELATED*****************/" << '\n'
       << '\n'
       << std::left << std::setw(spaces) << "DistancesNumberElements= "
       << std::accumulate(std::begin(histo_ete_distances_.counts),
                          std::end(histo_ete_distances_.counts), 0.0)
       << " %  (Edges)" << '\n'
       << std::left << std::setw(spaces) << "DistancesBinSize= "
       << histo_ete_distances_.breaks[1] - histo_ete_distances_.breaks[0]
       << '\n'
       << std::left << std::setw(spaces)
       << "DistancesNumberOfBins= " << histo_ete_distances_.bins << '\n'
       << std::left << std::setw(spaces) << "CosinesNumberElements= "
       << std::accumulate(std::begin(histo_cosines_.counts),
                          std::end(histo_cosines_.counts), 0.0)
       << "  % SUM_i_nodes{ Zi*(Zi-1)/2 }" << '\n'
       << std::left << std::setw(spaces) << "CosinesBinSize= "
       << histo_cosines_.breaks[1] - histo_cosines_.breaks[0] << '\n'
       << std::left << std::setw(spaces)
       << "CosinesNumberOfBins= " << histo_cosines_.bins << '\n'
       << '\n';
    transition_parameters.print(os, spaces);
    os << std::left << std::setw(spaces)
       << "von_mises_test-distances= " << energy_ete_distances() << '\n'
       << std::left << std::setw(spaces)
       << "von_mises_test-cosines= " << energy_cosines() << '\n';
}

} // namespace SG
