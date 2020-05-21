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

void print(const Bond &bonded_pair, std::ostream &os, bool add_end_of_line) {
    os << "[" << bonded_pair.id_a;
    os << ", ";
    os << bonded_pair.id_b;
    os << "]";
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
} // namespace SG
