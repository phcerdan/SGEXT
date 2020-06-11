#include "simulated_annealing_generator.hpp"
#include "cramer_von_mises_test.hpp"
#include "cumulative_distribution_functions.hpp"
#include "degree_sequences.hpp"
#include "degree_viger_generator.hpp"
#include "generate_common.hpp"
#include "rng.hpp"
#include <boost/graph/graphviz.hpp> // for print_graph
#include <chrono>

namespace SG {
simulated_annealing_generator::simulated_annealing_generator()
            : step_move_node_(graph_, histo_ete_distances_, histo_cosines_),
              step_swap_edges_(graph_, histo_ete_distances_, histo_cosines_) {}

simulated_annealing_generator::simulated_annealing_generator(
        const size_t &num_vertices)
        : simulated_annealing_generator() {
    this->init_graph_degree(num_vertices);
    this->init_graph_vertex_positions();
    this->init_parameters();
    this->init_histograms(this->ete_distance_params.num_bins,
                          this->cosine_params.num_bins);
}
simulated_annealing_generator::simulated_annealing_generator(
        const GraphType &input_graph)
        : graph_(input_graph),
          step_move_node_(graph_, histo_ete_distances_, histo_cosines_),
          step_swap_edges_(graph_, histo_ete_distances_, histo_cosines_) {
    this->init_parameters();
    this->init_histograms(this->ete_distance_params.num_bins,
                          this->cosine_params.num_bins);
}

simulated_annealing_generator::simulated_annealing_generator(
        const simulated_annealing_generator_config_tree &tree)
        : simulated_annealing_generator() {
    this->set_parameters_from_configuration_tree(tree);
    this->init_graph_degree(this->physical_scaling_params.num_vertices);
    this->init_graph_vertex_positions();
    this->init_parameters();
    this->init_histograms(this->ete_distance_params.num_bins,
                          this->cosine_params.num_bins);
}

simulated_annealing_generator::simulated_annealing_generator(
        const std::string &input_parameters_file)
        : simulated_annealing_generator() {
    this->set_parameters_from_file(input_parameters_file);
    this->init_graph_degree(this->physical_scaling_params.num_vertices);
    this->init_graph_vertex_positions();
    this->init_parameters();
    this->init_histograms(this->ete_distance_params.num_bins,
                          this->cosine_params.num_bins);
}

void simulated_annealing_generator::set_boundary_condition(
        const ArrayUtilities::boundary_condition &bc) {
    step_move_node_.boundary_condition = bc;
    step_swap_edges_.boundary_condition = bc;
    domain_params.boundary_condition = bc;
}

void simulated_annealing_generator::set_parameters_from_file(
        const std::string &input_file) {
    simulated_annealing_generator_config_tree tree;
    tree.load(input_file);
    this->set_parameters_from_configuration_tree(tree);
}
void simulated_annealing_generator::save_parameters_to_file(
        const std::string &output_file) {
    auto tree = save_parameters_to_configuration_tree();
    tree.save(output_file);
}

simulated_annealing_generator_config_tree
simulated_annealing_generator::save_parameters_to_configuration_tree() {
    simulated_annealing_generator_config_tree tree;
    tree.cosine_params = cosine_params;
    tree.degree_params = degree_params;
    tree.domain_params = domain_params;
    tree.ete_distance_params = ete_distance_params;
    tree.physical_scaling_params = physical_scaling_params;
    tree.transition_params = transition_params;
    return tree;
}

void simulated_annealing_generator::set_parameters_from_configuration_tree(
        const simulated_annealing_generator_config_tree &tree) {
    cosine_params = tree.cosine_params;
    degree_params = tree.degree_params;
    domain_params = tree.domain_params;
    ete_distance_params = tree.ete_distance_params;
    physical_scaling_params = tree.physical_scaling_params;
    transition_params = tree.transition_params;
}

void simulated_annealing_generator::init_parameters() {
    const auto num_vertices = boost::num_vertices(graph_);
    auto &psp = physical_scaling_params;
    auto &etep = ete_distance_params;
    psp.length_scaling_factor =
            std::pow(num_vertices / psp.node_density, 1 / 3.0);
    etep.normalized_normal_mean =
            etep.physical_normal_mean / psp.length_scaling_factor;
    etep.set_normalized_log_std_deviation(etep.normalized_normal_mean,
                                          etep.normalized_normal_std_deviation);
    etep.set_normalized_log_mean(etep.normalized_normal_mean,
                                 etep.normalized_log_std_deviation);
}
void simulated_annealing_generator::init_graph_degree(
        const size_t &num_vertices) {
    const double q = 1.0 / (degree_params.mean - 2.0);
    const size_t min_degree_allowed = degree_params.min_degree;
    const size_t max_degree_allowed = degree_params.max_degree;
    const double percentage_of_one_degree_nodes =
        degree_params.percentage_of_one_degree_nodes;

    // Generate a degree_sequence
    std::vector<int> degree_sequence;
    size_t sum = 0;
    const size_t max_iterations = 20;
    size_t iteration = 0;
    do {
        degree_sequence =
                generate_degree_sequence_geometric_distribution_bounded(
                        num_vertices, q, min_degree_allowed,
                        max_degree_allowed, percentage_of_one_degree_nodes);
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
        graph_[*vi].pos = RNG::random_pos(domain_params.domain);
    }
}

void simulated_annealing_generator::init_histogram_ete_distances(
        const size_t &num_bins) {
    auto &parameters = ete_distance_params;
    parameters.num_bins = num_bins;
    std::vector<double> dummy_data;
    const double min_distance = 0.0;
    // Assuming {0, 0, 0} is a corner of the simulation box:
    double max_distance = ArrayUtilities::norm(domain_params.domain);
    if (domain_params.boundary_condition ==
        ArrayUtilities::boundary_condition::PERIODIC) {
        max_distance /= 2.0;
    }
    std::vector<double> breaks = histo::GenerateBreaksFromRangeAndBins(
            min_distance, max_distance, num_bins);
    histo_ete_distances_ = Histogram(dummy_data, breaks);
}

void simulated_annealing_generator::
        populate_target_cumulative_distro_histo_ete_distances(
                const std::vector<double> &histo_centers,
                const std::function<double(double)> &cumulative_func) {
    auto &F = target_cumulative_distro_histo_ete_distances_;
    const auto bins = std::size(histo_centers);
    F.resize(bins);
    apply_distro(histo_centers, F, cumulative_func);
}
void simulated_annealing_generator::
        populate_target_cumulative_distro_histo_cosines(
                const std::vector<double> &histo_centers,
                const std::function<double(double)> &cumulative_func) {
    auto &F = target_cumulative_distro_histo_cosines_;
    const auto bins = std::size(histo_centers);
    F.resize(bins);
    apply_distro(histo_centers, F, cumulative_func);
}

void simulated_annealing_generator::init_histogram_cosines(
        const size_t &num_bins) {
    auto &parameters = cosine_params;
    parameters.num_bins = num_bins;
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
            graph_, domain_params.boundary_condition));
    // Populate LUT for optimization
    auto &LUT = LUT_cumulative_histo_ete_distances_;
    LUT.resize(histo_ete_distances_.bins);
    const auto total_counts =
            std::accumulate(std::begin(histo_ete_distances_.counts),
                            std::end(histo_ete_distances_.counts), 0);
    std::transform(std::begin(target_cumulative_distro_histo_ete_distances_),
                   std::end(target_cumulative_distro_histo_ete_distances_),
                   std::begin(LUT),
                   [&total_counts = total_counts](const double &x) -> double {
                       return total_counts * x + 0.5;
                   });
    total_counts_ete_distances_ = total_counts;
}

void simulated_annealing_generator::populate_histogram_cosines() {
    histo_cosines_.ResetCounts();
    histo_cosines_.FillCounts(get_all_cosine_directors_between_connected_edges(
            graph_, domain_params.boundary_condition));
    // Populate LUT for optimization
    auto &LUT = LUT_cumulative_histo_cosines_;
    LUT.resize(histo_cosines_.bins);
    const auto total_counts =
            std::accumulate(std::begin(histo_cosines_.counts),
                            std::end(histo_cosines_.counts), 0);
    std::transform(std::begin(target_cumulative_distro_histo_cosines_),
                   std::end(target_cumulative_distro_histo_cosines_),
                   std::begin(LUT),
                   [&total_counts = total_counts](const double &x) -> double {
                       return total_counts * x + 0.5;
                   });
    total_counts_cosines_ = total_counts;
}
void simulated_annealing_generator::init_histograms(
        const size_t &num_bins_ete_distances, const size_t &num_bins_cosines) {
    this->init_histogram_ete_distances(num_bins_ete_distances);
    // if another distribution is required, call
    // populate_target_cumulative_distro_xxx with another function
    auto ete_distances_cumulative_func =
            [&distro_parameters =
                     this->ete_distance_params](const double &x) -> double {
        return (cumulative_distribution_lognormal(
                x, distro_parameters.normalized_log_mean,
                distro_parameters.normalized_log_std_deviation));
    };
    this->populate_target_cumulative_distro_histo_ete_distances(
            histo_ete_distances_.ComputeBinCenters(),
            ete_distances_cumulative_func);
    this->populate_histogram_ete_distances();

    this->init_histogram_cosines(num_bins_cosines);
    auto cosines_cumulative_func = [&distro_parameters = this->cosine_params](
                                           const double &x) -> double {
        return (cumulative_distribution_truncated_power_series_3(
                x, distro_parameters.b1, distro_parameters.b2,
                distro_parameters.b3));
    };
    this->populate_target_cumulative_distro_histo_cosines(
            histo_cosines_.ComputeBinCenters(), cosines_cumulative_func);
    this->populate_histogram_cosines();
}
void simulated_annealing_generator::engine(const bool &reset_steps) {
    const auto t_start = std::chrono::high_resolution_clock::now();
    auto & steps = transition_params.steps_performed;
    if(reset_steps) steps = 0;
    /****** For reporting progress **********/
    const double log_size =
            std::log10(transition_params.MAX_ENGINE_ITERATIONS) - 2;
    const size_t report_every = log_size > 0 ? std::pow(10, log_size) : 1;
    size_t progress_count = 0;
    /****/
    const double energy_initial = compute_energy();
    transition_params.energy_initial = energy_initial;
    transition_params.energy = energy_initial;
    transition_params.temp_initial =
            transition_params.energy_initial / boost::num_vertices(graph_);
    transition_params.temp_current = transition_params.temp_initial;
    // const double energy_diff = energy_new - transition_params.energy;
    // transition_params.temp_initial = std::abs(energy_diff /
    // log(0.5));
    while (transition_params.consecutive_failures !=
                   transition_params.MAX_CONSECUTIVE_FAILURES &&
           steps != transition_params.MAX_ENGINE_ITERATIONS &&
           transition_params.energy >= transition_params.ENERGY_CONVERGENCE) {
        if (verbose) {
            std::cout << "Step #: " << steps << std::endl;
        }
        const bool show_progress = true;
        if (show_progress) {
            if (progress_count == report_every) {
                progress_count = 0;
                std::cout << "Step #" << steps << std::endl;
                std::cout << "von-mises_distances= " << energy_ete_distances()
                          << std::endl;
                std::cout << "von-mises_cosines= " << energy_cosines()
                          << std::endl;
            }
            progress_count++;
        }
        simulated_annealing_generator::transition transition;

        if (RNG::rand01() <
            transition_params.UPDATE_STEP_MOVE_NODE_PROBABILITY) {
            if (verbose) {
                std::cout << "Step type: move_node" << std::endl;
            }
            step_move_node_.randomize();
            step_move_node_.perform();
            transition = check_transition();
            if (transition == transition::REJECTED) {
                step_move_node_.undo();
            } else if (transition == transition::ACCEPTED ||
                       transition == transition::ACCEPTED_HIGH_TEMP) {
                step_move_node_.update_graph();
            }

        } else {
            step_swap_edges_.randomize();
            step_swap_edges_.perform();
            transition = check_transition();
            if (transition == transition::REJECTED) {
                step_swap_edges_.undo();
            } else if (transition == transition::ACCEPTED ||
                       transition == transition::ACCEPTED_HIGH_TEMP) {
                step_swap_edges_.update_graph();
            }
        }
        steps++;
    }

    const auto t_final = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = t_final - t_start;
    transition_params.time_elapsed = elapsed.count();
} // namespace SG

double simulated_annealing_generator::energy_ete_distances() const {
    // return cramer_von_mises_test(histo_ete_distances_.counts,
    //                              target_cumulative_distro_histo_ete_distances_);
    return energy_ete_distances_extra_penalty() +
           cramer_von_mises_test_optimized(histo_ete_distances_.counts,
                                           LUT_cumulative_histo_ete_distances_,
                                           total_counts_ete_distances_);
}

double
simulated_annealing_generator::energy_ete_distances_extra_penalty() const {
    // TODO: Implement average ete_distances (cheap).
    // Hint, use the histograms and the distances stores in update_steps
    // Or accumulate in the histograms
    // Or, instead of average, any test of the tail of the distro.
    return 0.0;
}

double simulated_annealing_generator::energy_cosines() const {
    // return cramer_von_mises_test(histo_cosines_.counts,
    //                              target_cumulative_distro_histo_cosines_);
    // Correcting term to avoid accumulation in the last bin. The value of
    // target_distribution_cosines_ in the last bin is zero or close by.
    return energy_cosines_extra_penalty() +
           cramer_von_mises_test_optimized(histo_cosines_.counts,
                                           LUT_cumulative_histo_cosines_,
                                           total_counts_cosines_);
}
double simulated_annealing_generator::energy_cosines_extra_penalty() const {
    return histo_cosines_.counts.back() /
           static_cast<double>(histo_cosines_.bins);
}
double simulated_annealing_generator::compute_energy() const {
    const double test_ete_distances = energy_ete_distances();
    const double test_cosines = energy_cosines();
    return test_ete_distances + test_cosines;
}

simulated_annealing_generator::transition
simulated_annealing_generator::check_transition() {
    const double energy_new = compute_energy();
    const double energy_diff = energy_new - transition_params.energy;

    if (energy_diff <= 0.0) {
        // Transition Accepted
        transition_params.energy = energy_new;
        transition_params.accepted_transitions++;
        transition_params.temp_current *= transition_params.temp_cooling_rate;
        transition_params.consecutive_failures = 0;
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
                    exp(-energy_diff / transition_params.temp_current))) {
            transition_params.energy = energy_new;
            transition_params.accepted_transitions++;
            transition_params.high_temp_transitions++;
            transition_params.temp_current *=
                    transition_params.temp_cooling_rate;
            transition_params.consecutive_failures = 0;
            return transition::ACCEPTED_HIGH_TEMP;
        } else {
            // Call undo? or call undo if compare()==0 in other function?
            transition_params.consecutive_failures++;
            transition_params.rejected_transitions++;
            return transition::REJECTED;
        }
    }
}

void simulated_annealing_generator::print(std::ostream &os, int spaces) const {
    os << "%/************NETWORK PARAMETERS*****************/" << '\n'
       << '\n'
       << std::left << std::setw(spaces)
       << "Nodes_N= " << boost::num_vertices(graph_) << '\n'
       << std::left << std::setw(spaces)
       << "Edges_E= " << boost::num_edges(graph_) << '\n';
    domain_params.print(os, spaces);
    physical_scaling_params.print(os, spaces);
    ete_distance_params.print(os, spaces);
    degree_params.print(os, spaces);
    cosine_params.print(os, spaces);
    transition_params.print(os, spaces);
    os << "%/************HISTOGRAM BINS RELATED*****************/" << '\n'
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
    os << "%/************CURRENT ENERGY INFO ****************/" << '\n';
    const auto en_ete = energy_ete_distances();
    const auto en_ete_extra_penalty = energy_ete_distances_extra_penalty();
    const auto en_ete_von_mises = en_ete - en_ete_extra_penalty;
    const auto en_cosines = energy_cosines();
    const auto en_cosines_extra_penalty = energy_cosines_extra_penalty();
    const auto en_cosines_von_mises = en_cosines - en_cosines_extra_penalty;
    os << std::left << std::setw(spaces)
       << "energy_ete_distances= " << en_ete << '\n'
       << std::left << std::setw(spaces)
       << "energy_ete_distances von_mises part" << en_ete_von_mises << '\n'
       << std::left << std::setw(spaces)
       << "energy_ete_distances extra_penaly= " << en_ete_extra_penalty << '\n';
    os << std::left << std::setw(spaces)
       << "energy_cosines= " << en_cosines << '\n'
       << std::left << std::setw(spaces)
       << "energy_cosines von_mises part" << en_cosines_von_mises << '\n'
       << std::left << std::setw(spaces)
       << "energy_cosines extra_penaly= " << en_cosines_extra_penalty << '\n';
}

void simulated_annealing_generator::print_histo_and_target_distribution(
        std::ostream &os,
        const Histogram &histo,
        const std::vector<double> &distro) const {
    assert(std::size(histo.counts) == std::size(distro));
    std::ios::fmtflags os_flags(os.flags());
    os.setf(std::ios_base::fixed, std::ios_base::floatfield);
    os.precision(9);
    for (unsigned long long i = 0; i < histo.counts.size(); i++) {
        os << "[";
        os << std::setw(18) << histo.breaks[i] << "," << std::setw(18)
           << histo.breaks[i + 1];
        if (i == histo.counts.size() - 1)
            os << "]";
        else
            os << ")";
        os << " " << std::setw(18) << histo.counts[i] << "  " << distro[i]
           << std::endl;
    }
    os.flags(os_flags);
}

void simulated_annealing_generator::
        print_histo_and_target_distribution_ete_distances(
                std::ostream &os) const {
    const auto &histo = histo_ete_distances_;
    const auto &distro = target_cumulative_distro_histo_ete_distances_;
    auto target_bins = distro;
    const size_t N = std::accumulate(std::begin(histo.counts),
                                     std::end(histo.counts), 0.0);
    for (size_t i = 0; i < std::size(distro); i++) {
        const auto t = distro[i];
        const double diff = (i == 0) ? t : t - distro[i - 1];
        target_bins[i] = diff * N;
    }
    this->print_histo_and_target_distribution(os, histo, target_bins);
}

void simulated_annealing_generator::print_histo_and_target_distribution_cosines(
        std::ostream &os) const {
    const auto &histo = histo_cosines_;
    const auto &distro = target_cumulative_distro_histo_cosines_;
    auto target_bins = distro;
    const size_t N = std::accumulate(std::begin(histo.counts),
                                     std::end(histo.counts), 0.0);
    for (size_t i = 0; i < std::size(distro); i++) {
        const auto t = distro[i];
        const double diff = (i == 0) ? t : t - distro[i - 1];
        target_bins[i] = diff * N;
    }
    this->print_histo_and_target_distribution(os, histo, target_bins);
}

} // namespace SG
