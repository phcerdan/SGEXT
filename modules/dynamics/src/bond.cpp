/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bond.hpp"
#include <memory>

#ifdef SG_USING_VTK
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include <vtkLine.h>
#endif

namespace SG {

bool operator==(const Bond &lhs, const Bond &rhs) {
    return (lhs.id_a == rhs.id_a) && (lhs.id_b == rhs.id_b);
}
bool operator!=(const Bond &lhs, const Bond &rhs) { return !(lhs == rhs); }
bool operator<(const Bond &lhs, const Bond &rhs) {
    if (lhs.id_a < rhs.id_a) {
        return true;
    }

    if (lhs.id_a == rhs.id_a) {
        if (lhs.id_b < rhs.id_b) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

std::string tag_bond_int_to_string(const int &tag_bond_int)
{
    switch(tag_bond_int) {
        case tag_bond_chain:
            return tag_bond_chain_char;
            break;
        case tag_bond_contour_length_chain:
            return tag_bond_contour_length_chain_char;
            break;
        case tag_bond_free_chain:
            return tag_bond_free_chain_char;
            break;
        default:
            return std::to_string(tag_bond_int);
    }
}
int tag_bond_string_to_int(const std::string &tag_bond_string)
{
    if(tag_bond_string == tag_bond_chain_char)
        return tag_bond_chain;
    else if(tag_bond_string == tag_bond_contour_length_chain_char)
        return tag_bond_contour_length_chain;
    else if(tag_bond_string == tag_bond_free_chain_char)
        return tag_bond_free_chain;
    else
        return std::stoi(tag_bond_string);
}

void print(const BondProperties &properties,
           std::ostream &os,
           bool add_end_of_line) {
    os << "tags: [ ";
    for (const auto & tag : properties.tags) {
        os << tag_bond_int_to_string(tag) << ", ";
    }
    os << " ]";

    if (add_end_of_line)
        os << std::endl;
}

void print(const Bond &bonded_pair, std::ostream &os, bool add_end_of_line) {
    os << "[" << bonded_pair.id_a;
    os << ", ";
    os << bonded_pair.id_b;
    os << "] ";
    print(*bonded_pair.properties, os, false);
    if (add_end_of_line)
        os << std::endl;
};

void sort(Bond &bonded_pair) {
    if (bonded_pair.id_a > bonded_pair.id_b) {
        std::swap(bonded_pair.id_a, bonded_pair.id_b);
    }
};

void print(const BondChain &bonded_pair,
           std::ostream &os,
           bool add_end_of_line) {
    print(static_cast<const Bond &>(bonded_pair), os, false);
    os << " Lc: " << bonded_pair.length_contour;
    if (add_end_of_line)
        os << std::endl;
};

#ifdef SG_USING_VTK
vtkIdType
Bond::add_to_vtu(vtkUnstructuredGrid *ugrid,
                 const particle_id_to_vtk_id_map_t &particle_id_to_vtk_id_map) {
    // auto cell_data = ugrid->GetCellData();
    auto vtk_id_list = vtkIdList::New();
    vtk_id_list->InsertNextId(particle_id_to_vtk_id_map.at(this->id_a));
    vtk_id_list->InsertNextId(particle_id_to_vtk_id_map.at(this->id_b));
    auto line = vtkLine::New();
    return ugrid->InsertNextCell(line->GetCellType(), vtk_id_list);
}

vtkIdType Bond::append_to_vtu(vtkUnstructuredGrid *ugrid,
                              const vtkIdType &cell_id) {
    auto cell_data = ugrid->GetCellData();
    // Number Of Cells is set in the first bonds sweep from add_to_vtu.
    const auto ncells = ugrid->GetNumberOfCells();
    const std::string array_name_ids = "bond_ids";
    auto ids_array = cell_data->GetArray(array_name_ids.c_str());
    if (!ids_array) { // create array if doesn't exists
        auto vtk_array = vtkIdTypeArray::New();
        vtk_array->SetName(array_name_ids.c_str());
        vtk_array->SetNumberOfComponents(2);
        vtk_array->SetNumberOfTuples(ncells);
        cell_data->AddArray(vtk_array);
        ids_array = vtk_array;
    }
    // populate the array tuple:
    ids_array->SetTuple2(cell_id, this->id_a, this->id_b);
    cell_data->Update();
    return cell_id;
}
vtkIdType BondChain::append_to_vtu(vtkUnstructuredGrid *ugrid,
                                   const vtkIdType &cell_id) {
    // call base class first to populate ugrid with its data
    Bond::append_to_vtu(ugrid, cell_id);
    auto cell_data = ugrid->GetCellData();
    // Number Of Cells is set in the first bonds sweep from add_to_vtu.
    const auto ncells = ugrid->GetNumberOfCells();
    // Add array (of size number of cells) to hold contour length, only if
    // doesn't exist already
    const std::string array_name = "contour_length";
    auto contour_array = cell_data->GetArray(array_name.c_str());
    if (!contour_array) { // create array if doesn't exists
        auto vtk_array = vtkDoubleArray::New();
        vtk_array->SetName(array_name.c_str());
        vtk_array->SetNumberOfComponents(1);
        vtk_array->SetNumberOfTuples(ncells);
        cell_data->AddArray(vtk_array);
        contour_array = vtk_array;
    }
    // populate the array tuple:
    contour_array->SetTuple1(cell_id, this->length_contour);
    cell_data->Update();
    return cell_id;
}
#endif // SG_USING_VTK

void print(const BondPropertiesPhysical &properties,
           std::ostream &os,
           bool add_end_of_line) {
    print(static_cast<BondProperties>(properties), os, true);
    os << "persistence_length: " << properties.persistence_length << std::endl;
    os << "kT: " << properties.kT;
    if (add_end_of_line)
        os << std::endl;
}
} // namespace SG
