/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

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
 * Thin input image using DGtal library, with the asymmetric thining algorithm of
 * Bertrand and Couprie using Voxel Complex.
 * See: https://dgtal.org/doc/1.0/moduleVoxelComplex.html
 *
 * @param input_image binary image to thin/skeletonize
 *
 * @param skel_type_str type of skeletonization.
 *     Valid options: end, ulti, 1isthmus, isthmus
 *     end: conserve end points
 *     ulti: ultimate skeleton, only conserve points which removal change topology.
 *     it generates a single point centered in any volume without holes.
 *     1isthmus: conserve linear (1D) isthmus
 *     isthmus: conserve surface (2D) isthmus and 1D isthmus.
 *
 * @param skel_select_type_str type for choosing a voxel (asymmetric thinning).
 *     Valid options: dmax, random, first
 *     dmax: use a distance map to select the Complex with the greates distance map value.
 *     The final skeleton will be a centered medial axis skeleton using this option
 *     random: select a random voxel in case an asymmetric choice is needed.
 *     first: choose the first voxel of the container (its not ordered in any meaningful way)
 *
 * @param tables_folder Path where to find look up tables from DGtal.
 *     Simplicity and Isthmusicity tables.
 *
 * @param persistence Integer to locally trim non-important branches.
 * If 0 no persistence algorith.
 *
 * @param distance_map_image distance map image used when @ref skel_select_type_str
 * is dmax
 *
 * @param profile timing the algorithm
 *
 * @param verbose extra info
 *
 * @param visualize visualize the end result.
 *      Only if compile definitions are enabled.
 *
 * @return thin image
 */

BinaryImageType::Pointer thin_function(
    const BinaryImageType::Pointer & input_image,
    const std::string & skel_type_str,
    const std::string & skel_select_type_str,
    const std::string & tables_folder,
    const int & persistence = 0,
    const FloatImageType::Pointer & distance_map_image = nullptr,
    const bool profile = false,
    const bool verbose = false,
    const bool visualize = false
    );

/**
 * Thin input image using DGtal library, with the asymmetric thining algorithm of
 * Bertrand and Couprie using Voxel Complex.
 * Input is a filename, and it writes the result to a file. It calls @ref thin
 * function that does not perform io operations.
 * See: https://dgtal.org/doc/1.0/moduleVoxelComplex.html
 *
 * @param filename input filename holding a binary image that will be thinned
 *
 * @param skel_type_str type of skeletonization.
 *     Valid options: end, ulti, 1isthmus, isthmus
 *     end: conserve end points
 *     ulti: ultimate skeleton, only conserve points which removal change topology.
 *     it generates a single point centered in any volume without holes.
 *     1isthmus: conserve linear (1D) isthmus
 *     isthmus: conserve surface (2D) isthmus and 1D isthmus.
 *
 * @param skel_select_type_str type for choosing a voxel (asymmetric thinning).
 *     Valid options: dmax, random, first
 *     dmax: use a distance map to select the Complex with the greates distance map value.
 *     The final skeleton will be a centered medial axis skeleton using this option
 *     random: select a random voxel in case an asymmetric choice is needed.
 *     first: choose the first voxel of the container (its not ordered in any meaningful way)
 *
 * @param output_foldername folder where output image will be written
 *
 * @param tables_folder Path where to find look up tables from DGtal.
 *     Simplicity and Isthmusicity tables.
 *
 * @param inputDistanceMapImageFilename filename holding a distance map image
 * used when @ref skel_select_type_str is dmax
 *
 * @param out_sequence_discrete_points_foldername foldername to save a sequence
 * of discrete points of the thin results. If string is empty, won't be computed.
 *
 * @param profile timing the algorithm
 *
 * @param verbose extra info
 *
 * @param visualize visualize the end result.
 *      Only if compile definitions are enabled.
 *
 * @return thin image
 */

BinaryImageType::Pointer thin_function_io(
        const std::string & filename,
        const std::string & skel_type_str,
        const std::string & skel_select_type_str,
        const std::string & output_foldername,
        const std::string & tables_folder,
        const int & persistence = 0,
        const std::string & inputDistanceMapImageFilename = "",
        // itk_filters
        const std::string & foreground = "white", // or "black"
        // export files
        const std::string & out_sequence_discrete_points_foldername = "",
        const bool profile = false,
        const bool verbose = false,
        const bool visualize = false
        );

} // end ns
#endif
