#include "simulated_annealing_generator_config_tree.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SG {
namespace pt = boost::property_tree;

void simulated_annealing_generator_config_tree::load(
        const std::string &filename) {
    pt::ptree tree;
    // Parse the json into the property tree.
    pt::read_json(filename, tree);
    this->load_cosine(tree);
    this->load_degree(tree);
    this->load_domain(tree);
    this->load_ete_distance(tree);
    this->load_physical_scaling(tree);
    this->load_transition(tree);
}

void simulated_annealing_generator_config_tree::save(
        const std::string &filename) {
    pt::ptree tree;
    this->save_cosine(tree);
    this->save_degree(tree);
    this->save_domain(tree);
    this->save_ete_distance(tree);
    this->save_physical_scaling(tree);
    this->save_transition(tree);
    // Write property to json file
    pt::write_json(filename, tree);
}

void simulated_annealing_generator_config_tree::load_domain(pt::ptree &tree) {
    std::string boundary_condition_str =
            tree.get<std::string>("domain.boundary_condition");
    domain_params.boundary_condition =
            ArrayUtilities::string_to_boundary_condition(
                    boundary_condition_str);

    domain_params.domain[0] = tree.get<double>("domain.x");
    domain_params.domain[1] = tree.get<double>("domain.y");
    domain_params.domain[2] = tree.get<double>("domain.z");
}

void simulated_annealing_generator_config_tree::save_domain(pt::ptree &tree) {
    const auto boundary_condition_str =
            ArrayUtilities::boundary_condition_to_string(
                    domain_params.boundary_condition);
    tree.put("domain.boundary_condition", boundary_condition_str);
    tree.put("domain.x", domain_params.domain[0]);
    tree.put("domain.y", domain_params.domain[1]);
    tree.put("domain.z", domain_params.domain[2]);
}

void simulated_annealing_generator_config_tree::load_transition(
        pt::ptree &tree) {
    transition_params.temp_cooling_rate =
            tree.get<double>("transition.temp_cooling_rate");
    transition_params.MAX_CONSECUTIVE_FAILURES =
            tree.get<size_t>("transition.MAX_CONSECUTIVE_FAILURES");
    transition_params.MAX_ENGINE_ITERATIONS =
            tree.get<size_t>("transition.MAX_ENGINE_ITERATIONS");
    transition_params.ENERGY_CONVERGENCE =
            tree.get<double>("transition.ENERGY_CONVERGENCE");
    transition_params.UPDATE_STEP_MOVE_NODE_PROBABILITY =
            tree.get<double>("transition.UPDATE_STEP_MOVE_NODE_PROBABILITY");
    transition_params.update_step_move_node_max_step_distance =
            tree.get<double>(
                    "transition.update_step_move_node_max_step_distance");
}

void simulated_annealing_generator_config_tree::save_transition(
        pt::ptree &tree) {
    tree.put("transition.energy", transition_params.energy);
    tree.put("transition.energy_initial", transition_params.energy_initial);
    tree.put("transition.accepted_transitions",
             transition_params.accepted_transitions);
    tree.put("transition.rejected_transitions",
             transition_params.rejected_transitions);
    tree.put("transition.high_temp_transitions",
             transition_params.high_temp_transitions);
    tree.put("transition.consecutive_failures",
             transition_params.consecutive_failures);
    tree.put("transition.total_failures", transition_params.total_failures);
    tree.put("transition.time_elapsed", transition_params.time_elapsed);
    tree.put("transition.temp_initial", transition_params.temp_initial);
    tree.put("transition.temp_current", transition_params.temp_current);
    tree.put("transition.temp_cooling_rate",
             transition_params.temp_cooling_rate);
    tree.put("transition.MAX_CONSECUTIVE_FAILURES",
             transition_params.MAX_CONSECUTIVE_FAILURES);
    tree.put("transition.MAX_ENGINE_ITERATIONS",
             transition_params.MAX_ENGINE_ITERATIONS);
    tree.put("transition.ENERGY_CONVERGENCE",
             transition_params.ENERGY_CONVERGENCE);
    tree.put("transition.UPDATE_STEP_MOVE_NODE_PROBABILITY",
             transition_params.UPDATE_STEP_MOVE_NODE_PROBABILITY);
    tree.put("transition.update_step_move_node_max_step_distance",
             transition_params.update_step_move_node_max_step_distance);
}
void simulated_annealing_generator_config_tree::load_degree(pt::ptree &tree) {
    degree_params.mean = tree.get<double>("degree.mean");
    degree_params.min_degree = tree.get<size_t>("degree.min_degree");
    degree_params.max_degree = tree.get<size_t>("degree.max_degree");
}
void simulated_annealing_generator_config_tree::save_degree(pt::ptree &tree) {
    tree.put("degree.mean", degree_params.mean);
    tree.put("degree.min_degree", degree_params.min_degree);
    tree.put("degree.max_degree", degree_params.max_degree);
}

void simulated_annealing_generator_config_tree::load_ete_distance(
        pt::ptree &tree) {
    ete_distance_params.physical_normal_mean =
            tree.get<double>("ete_distance.physical_normal_mean");
    ete_distance_params.physical_normal_std_deviation =
            tree.get<double>("ete_distance.physical_normal_std_deviation");
    ete_distance_params.num_bins = tree.get<size_t>("ete_distance.num_bins");
}

void simulated_annealing_generator_config_tree::save_ete_distance(
        pt::ptree &tree) {
    tree.put("ete_distance.physical_normal_mean",
             ete_distance_params.physical_normal_mean);
    tree.put("ete_distance.physical_normal_std_deviation",
             ete_distance_params.physical_normal_std_deviation);
    tree.put("ete_distance.normalized_normal_mean",
             ete_distance_params.normalized_normal_mean);
    tree.put("ete_distance.normalized_normal_std_deviation",
             ete_distance_params.normalized_normal_std_deviation);
    tree.put("ete_distance.normalized_log_mean",
             ete_distance_params.normalized_log_mean);
    tree.put("ete_distance.normalized_log_std_deviation",
             ete_distance_params.normalized_log_std_deviation);
    tree.put("ete_distance.num_bins", ete_distance_params.num_bins);
}
void simulated_annealing_generator_config_tree::load_physical_scaling(
        pt::ptree &tree) {
    physical_scaling_params.num_vertices =
            tree.get<size_t>("physical_scaling.num_vertices");
    physical_scaling_params.node_density =
            tree.get<double>("physical_scaling.node_density");
}

void simulated_annealing_generator_config_tree::save_physical_scaling(
        pt::ptree &tree) {
    tree.put("physical_scaling.num_vertices",
             physical_scaling_params.num_vertices);
    tree.put("physical_scaling.node_density",
             physical_scaling_params.node_density);
    tree.put("physical_scaling.length_scaling_factor",
             physical_scaling_params.length_scaling_factor);
}

void simulated_annealing_generator_config_tree::load_cosine(pt::ptree &tree) {
    cosine_params.b1 = tree.get<double>("cosine.b1");
    cosine_params.num_bins = tree.get<size_t>("cosine.num_bins");

    boost::optional<double> opt_b2 = tree.get_optional<double>("cosine.b2");
    boost::optional<double> opt_b3 = tree.get_optional<double>("cosine.b3");
    boost::optional<double> opt_bmax = tree.get_optional<double>("cosine.bmax");

    if (opt_bmax) {
        auto bmax = opt_bmax.get();
        cosine_params.b2 = 0.25 * (3. - 4. * cosine_params.b1 - bmax);
        cosine_params.b3 = -(3. / 32.) * (-1. + 2. * cosine_params.b1 +
                                          4. * cosine_params.b2);
        return;
    }

    if (opt_b2 && opt_b3) {
        cosine_params.b2 = opt_b2.get();
        cosine_params.b3 = opt_b3.get();
        return;
    }
    if (!opt_bmax && (!(opt_b2 && opt_b3))) {
        throw boost::property_tree::ptree_error(
                "Error reading cosine_params.\n"
                "Either b1, b2 and b3 are set OR"
                "b1 and bmax.");
    }
}

void simulated_annealing_generator_config_tree::save_cosine(pt::ptree &tree) {
    tree.put("cosine.b1", cosine_params.b1);
    tree.put("cosine.b2", cosine_params.b2);
    tree.put("cosine.b3", cosine_params.b3);
    tree.put("cosine.num_bins", cosine_params.num_bins);
}

} // end namespace SG
