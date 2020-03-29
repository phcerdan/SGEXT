/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef THIN_FUNCTION_HPP
#define THIN_FUNCTION_HPP

#include <string>
#include "scripts_types.hpp"
#include "spatial_graph.hpp"

namespace SG {

/**
 * Enumeration of available Skeletonizations.
 * Usually we are interested in a skeleton, but conserving some points of interest.
 * even though the removal of those points wouldn't change the topology of the final object.
 */
enum class SkelType {
    /** Conserve the end points */
    end,
    /** Only conserve points that alters topology.
     * Any volume without holes will be reduced to a single centered point. */
    ultimate,
    /** Conserve points that are line (1d) isthmus. */
    isthmus1,
    /** Conserve points that are surface (2d) and line (1d) isthmus. */
    isthmus
};

inline std::string to_string(const SkelType & skel_en) {
    switch(skel_en)
    {
        case SkelType::end: return "end"; break;
        case SkelType::ultimate: return "ultimate"; break;
        case SkelType::isthmus1: return "isthmus1"; break;
        case SkelType::isthmus: return "isthmus"; break;
        default:
            throw std::runtime_error("skel_en is not valid");
    }
}
inline SkelType skel_string_to_enum(const std::string & skel_string) {
    if(skel_string == "end") return SkelType::end;
    if(skel_string == "ultimate") return SkelType::ultimate;
    if(skel_string == "isthmus1") return SkelType::isthmus1;
    if(skel_string == "isthmus") return SkelType::isthmus;
    // for compatibility with thin_script: 1isthmus
    if(skel_string == "1isthmus") return SkelType::isthmus1;
    throw std::runtime_error("skel_string is not valid: \"" + skel_string + "\"");
}

/**
 * Enumeration of available selections for the asymmetric processing of the skeletonization.
 * In case more than one option is available to keep in a skeleton in the asymmetric process, use this method.
 */
enum class SkelSelectType {
    /** Choose the first complex in the container.
     * No meaningful order in the container */
    first,
    /** Choose randomly between all the options */
    random,
    /** Use a distance map to choose the complex with greatest value of distance map. */
    dmax
};

inline std::string to_string(const SkelSelectType & skel_en) {
    switch(skel_en)
    {
        case SkelSelectType::first: return "first"; break;
        case SkelSelectType::random: return "random"; break;
        case SkelSelectType::dmax: return "dmax"; break;
        default:
            throw std::runtime_error("skel_en is not valid");
    }
}
inline SkelSelectType skel_select_string_to_enum(const std::string & select_string) {
    if(select_string == "first") return SkelSelectType::first;
    if(select_string == "random") return SkelSelectType::random;
    if(select_string == "dmax") return SkelSelectType::dmax;
    throw std::runtime_error("select_string is not valid: " + select_string);
}

/**
 *
 * @param filename
 * @param skel_string type of skeletonization.
 *     Valid options: end, ulti, 1isthmus, isthmus
 *     end: conserve end points
 *     ulti: ultimate skeleton, only conserve points which removal change topology.
 *     it generates a single point centered in any volume without holes.
 *     1isthmus: conserve linear (1D) isthmus
 *     isthmus: conserve surface (2D) isthmus and 1D isthmus.
 * @param select_string type for choosing a voxel (asymmetric thinning).
 *     Valid options: dmax, random, first
 *     dmax: use a distance map to select the Complex with the greates distance map value.
 *     The final skeleton will be a centered medial axis skeleton using this option
 *     random: select a random voxel in case an asymmetric choice is needed.
 *     first: choose the first voxel of the container (its not ordered in any meaningful way)
 * @param verbose extra info
 * @param visualize visualize the end result.
 *      Only if compile definitions are enabled.
 *
 * @return 
 */
BinaryImageType::Pointer thin_function(
        const std::string & filename,
        const std::string & skel_type_str,
        const std::string & skel_select_type_str,
        const std::string & output_foldername,
        const int & persistence = 0,
        const std::string & inputDistanceMapImageFilename = "",
        // itk_filters
        const std::string & foreground = "white", // or "black"
        // export files
        const std::string & out_sequence_discrete_points_foldername = "",
        const bool profile = false,
        const bool verbose = false,
        const bool visualize = false,
        const int thresholdMin = 0,
        const int thresholdMax = 255
        );

} // end ns
#endif
