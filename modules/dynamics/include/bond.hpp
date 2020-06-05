/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_BOND_HPP
#define SG_BOND_HPP
#include <cstddef>
#include <ostream>
#include <memory>
#include <set>
#ifdef SG_USING_VTK
#include <unordered_map>
#include <vtkUnstructuredGrid.h>
#endif // SG_USING_VTK

namespace SG {
/// Chain without any explicit property.
static constexpr int tag_bond_chain = 1000;
static constexpr auto tag_bond_chain_char = "chain";
/// Any node with degree == 1
static constexpr int tag_bond_free_chain = 1001;
static constexpr auto tag_bond_free_chain_char = "free chain";
/// Contains contour length
static constexpr int tag_bond_contour_length_chain = 1002;
static constexpr auto tag_bond_contour_length_chain_char = "contour length chain";

std::string tag_bond_int_to_string(const int &tag_bond_int);
int tag_bond_string_to_int(const std::string &tag_bond_string);

struct BondProperties
{
    using tag_t = int;
    using tags_t = std::set<tag_t>;
    tags_t tags;

    BondProperties() = default;
    explicit BondProperties(const int &tag) : tags({tag}){};
    explicit BondProperties(const tags_t &tags) : tags(tags){};
    BondProperties(const BondProperties &) = default;
    BondProperties(BondProperties &&) = default;
    BondProperties &operator=(const BondProperties &) = default;
    BondProperties &operator=(BondProperties &&) = default;
    virtual ~BondProperties() = default;
};

void print(const BondProperties &properties,
           std::ostream &os,
           bool add_end_of_line = true);

struct Bond {
    // id of bonded particles
    size_t id_a;
    size_t id_b;
    std::shared_ptr<BondProperties> properties;
    Bond(){
        properties = std::make_shared<BondProperties>();
    };
    Bond(const size_t &a, const size_t &b) : id_a(a), id_b(b){
        properties = std::make_shared<BondProperties>();
    };
    Bond(const Bond &) = default;
    Bond(Bond &&) = default;
    Bond &operator=(const Bond &) = default;
    Bond &operator=(Bond &&) = default;
    virtual ~Bond() = default;

#ifdef SG_USING_VTK
    using particle_id_t = size_t;
    using particle_id_to_vtk_id_map_t =
            std::unordered_map<particle_id_t, vtkIdType>;

    /**
     * Add a cell to ugrid (vtkLine) and returns the cell id.
     * Derived can re-use the cell id to append more data to the cell.
     *
     * @param ugrid
     * @param particle_id_to_vtk_id_map
     *
     * @return id of the cell
     */
    virtual vtkIdType
    add_to_vtu(vtkUnstructuredGrid *ugrid,
               const particle_id_to_vtk_id_map_t &particle_id_to_vtk_id_map);
    virtual vtkIdType append_to_vtu(vtkUnstructuredGrid *ugrid,
                                    const vtkIdType &cell_id);
#endif // SG_USING_VTK
};

bool operator==(const Bond &lhs, const Bond &rhs);
bool operator!=(const Bond &lhs, const Bond &rhs);
bool operator<(const Bond &lhs, const Bond &rhs);

void print(const Bond &bonded_pair,
           std::ostream &os,
           bool add_end_of_line = true);
// *
// * Ensures id_a to be smaller than id_b in a bonded_pair.
// *
// * @param bonded_pair
//
void sort(Bond &bonded_pair);

struct BondChain : public Bond {
    double length_contour = 1.0;
    BondChain(const size_t &a,
              const size_t &b,
              const double &input_contour_length)
            : Bond(a, b), length_contour(input_contour_length){};
    using Bond::Bond;
#ifdef SG_USING_VTK
    vtkIdType append_to_vtu(vtkUnstructuredGrid *ugrid,
                            const vtkIdType &cell_id) override;
#endif // SG_USING_VTK
};
void print(const BondChain &bonded_pair,
           std::ostream &os,
           bool add_end_of_line);

struct BondPropertiesPhysical : public BondProperties
{
    using BondProperties::BondProperties;

    BondPropertiesPhysical(const double & persistence_length, const double & kT):
        BondProperties(), persistence_length(persistence_length), kT(kT){};

    BondPropertiesPhysical(const tags_t & tags,
            const double & persistence_length,
            const double & kT):
        BondProperties(tags), persistence_length(persistence_length), kT(kT){};

    BondPropertiesPhysical(const int & tag,
            const double & persistence_length,
            const double & kT):
        BondProperties(tag), persistence_length(persistence_length), kT(kT){};

    double persistence_length;
    double kT;
};

void print(const BondPropertiesPhysical &properties,
           std::ostream &os,
           bool add_end_of_line = true);

} // namespace SG
#endif
