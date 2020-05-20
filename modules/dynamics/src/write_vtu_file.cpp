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

#include "write_vtu_file.hpp"
#include "bond.hpp"

#include <vtkDoubleArray.h>
#include <vtkIdTypeArray.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <vtkCellData.h>
#include <vtkXMLUnstructuredGridReader.h>

namespace SG {
void write_vtu_file(const System *sys, const std::string &file_name) {
    auto ugrid = vtkUnstructuredGrid::New();
    // Set geometry with position of points
    auto vtk_points = vtkSmartPointer<vtkPoints>::New();
    using particle_id_t = size_t;
    std::unordered_map<particle_id_t, vtkIdType> particle_id_to_vtk_id_map;
    for (const auto &particle : sys->all.particles) {
        const auto &pos = particle.pos;
        particle_id_to_vtk_id_map[particle.id] =
                vtk_points->InsertNextPoint(pos[0], pos[1], pos[2]);
    }
    ugrid->SetPoints(vtk_points);

    // Set topology using bonds
    const auto &unique_bonds = sys->bonds.bonds;
    // auto unique_bonds = SG::unique_bonds(sys);
    if (unique_bonds.empty()) {
        std::cerr << "WARNING: write_vtu_file, the system has no bonds."
                  << std::endl;
    }
    // Allocate Number of cells, i.e number of bonds in the system;
    ugrid->Allocate(unique_bonds.size());
    std::vector<vtkIdType> cell_ids;
    for (const auto &bond : unique_bonds) {
        // This adds cells (Bond adds vtkLines, but it can be overriden by
        // derived classes)
        cell_ids.emplace_back(
                bond->add_to_vtu(ugrid, particle_id_to_vtk_id_map));
    }
    // second sweep to append data dynamically depending on the type of Bond,
    // (because vtk arrays needs to know the total number of cells beforehand)
    {
        size_t cell_id_index = 0;
        for (const auto &bond : unique_bonds) {
            bond->append_to_vtu(ugrid, cell_ids[cell_id_index]);
            cell_id_index++;
        }
    }

    auto point_data = ugrid->GetPointData();
    const auto number_of_points = vtk_points->GetNumberOfPoints();

    auto acc = vtkDoubleArray::New();
    acc->SetName("acceleration");
    acc->SetNumberOfComponents(3);
    acc->SetNumberOfTuples(number_of_points);

    auto vel = vtkDoubleArray::New();
    vel->SetName("velocity");
    vel->SetNumberOfComponents(3);
    vel->SetNumberOfTuples(number_of_points);

    auto mass = vtkDoubleArray::New();
    mass->SetName("mass");
    mass->SetNumberOfComponents(1);
    mass->SetNumberOfTuples(number_of_points);

    auto radius = vtkDoubleArray::New();
    radius->SetName("radius");
    radius->SetNumberOfComponents(1);
    radius->SetNumberOfTuples(number_of_points);

    auto volume = vtkDoubleArray::New();
    volume->SetName("volume");
    volume->SetNumberOfComponents(1);
    volume->SetNumberOfTuples(number_of_points);

    auto particle_id = vtkIdTypeArray::New();
    particle_id->SetName("particle_id");
    particle_id->SetNumberOfComponents(1);
    particle_id->SetNumberOfTuples(number_of_points);

    size_t counter = 0;
    for (const auto &particle : sys->all.particles) {
        acc->SetTuple3(counter, particle.dynamics.acc[0],
                       particle.dynamics.acc[1], particle.dynamics.acc[2]);
        vel->SetTuple3(counter, particle.dynamics.vel[0],
                       particle.dynamics.vel[1], particle.dynamics.vel[2]);
        mass->SetTuple1(counter, particle.material.mass);
        radius->SetTuple1(counter, particle.material.radius);
        volume->SetTuple1(counter, particle.material.volume);
        particle_id->SetTuple1(counter, particle.id);
        counter++;
    }
    point_data->AddArray(acc);
    point_data->AddArray(vel);
    point_data->AddArray(mass);
    point_data->AddArray(radius);
    point_data->AddArray(volume);
    point_data->AddArray(particle_id);

    point_data->Update();

    auto ugrid_writer = vtkXMLUnstructuredGridWriter::New();
    ugrid_writer->SetFileName(file_name.c_str());
    ugrid_writer->SetInputData(ugrid);
    ugrid_writer->Update();
}

std::shared_ptr<System> read_vtu_file(const std::string &file_name) {
    auto sys = std::make_shared<System>();

    auto reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
    reader->SetFileName(file_name.c_str());
    reader->Update();
    const auto ugrid = reader->GetOutput();
    const auto npoints = ugrid->GetNumberOfPoints(); // particles
    const auto ncells = ugrid->GetNumberOfCells();   // bonds

    auto &particles = sys->all.particles;
    auto &bonds = sys->bonds.bonds;
    auto &conexions = sys->conexions;
    particles.resize(npoints);
    bonds.resize(ncells);
    conexions.resize(npoints);

    read_vtu_point_data(ugrid, sys.get());
    read_vtu_bond_ids(ugrid, sys.get());
    read_vtu_bond_contour_length(ugrid, sys.get());

    return sys;
}
void read_vtu_point_data(vtkUnstructuredGrid *ugrid, System *sys) {
    const auto npoints = ugrid->GetNumberOfPoints(); // particles
    auto &particles = sys->all.particles;
    if (particles.size() != npoints) {
        particles.resize(npoints);
    }
    // Get arrays of points
    const auto point_data = ugrid->GetPointData();

    const auto particle_id_array = point_data->GetArray("particle_id");
    const auto acc_array = point_data->GetArray("acceleration");
    const auto vel_array = point_data->GetArray("velocity");
    const auto mass_array = point_data->GetArray("mass");
    const auto volume_array = point_data->GetArray("volume");
    const auto radius_array = point_data->GetArray("radius");

    for (size_t i = 0; i < npoints; i++) {
        // pos (always there)
        memcpy(particles[i].pos.data(), ugrid->GetPoint(i), sizeof(double) * 3);

        // particle_id
        if (particle_id_array) {
            particles[i].id = particle_id_array->GetTuple1(i);
        } else {
            particles[i].id = i;
        }
        if (acc_array) {
            memcpy(particles[i].dynamics.acc.data(), acc_array->GetTuple3(i),
                   sizeof(double) * 3);
        }
        if (vel_array) {
            memcpy(particles[i].dynamics.vel.data(), vel_array->GetTuple3(i),
                   sizeof(double) * 3);
        }
        if (mass_array) {
            particles[i].material.mass = mass_array->GetTuple1(i);
        }
        if (volume_array) {
            particles[i].material.volume = volume_array->GetTuple1(i);
        }
        if (radius_array) {
            particles[i].material.radius = radius_array->GetTuple1(i);
        }
    }
}

void read_vtu_bond_ids(vtkUnstructuredGrid *ugrid, System *sys) {
    const auto ncells = ugrid->GetNumberOfCells(); // bonds
    auto &particles = sys->all.particles;
    auto &bonds = sys->bonds.bonds;
    // Populate bonds ids from Cell points
    auto line_cell = vtkSmartPointer<vtkLine>::New();
    for (size_t i = 0; i < ncells; ++i) {
        // Get the cell (a vtkLine)
        auto cell = ugrid->GetCell(i);
        if (cell->GetCellType() != line_cell->GetCellType()) {
            throw std::runtime_error(
                    "The only allowed cells to read are vtkLine, but found a "
                    "different one. Use another reader for this cell type and "
                    "data.");
        }
        auto cell_point_ids = cell->GetPointIds();
        auto line_id_a = cell_point_ids->GetId(0);
        auto line_id_b = cell_point_ids->GetId(1);
        // Create bond (Base class) with this info
        bonds[i] = std::make_shared<Bond>(particles[line_id_a].id,
                                          particles[line_id_b].id);
    }
}

void read_vtu_bond_contour_length(vtkUnstructuredGrid *ugrid, System *sys) {
    const auto ncells = ugrid->GetNumberOfCells(); // bonds
    auto &bonds = sys->bonds.bonds;
    // Get cell data
    auto cell_data = ugrid->GetCellData();
    const std::string array_name = "contour_length";
    auto contour_length_array = cell_data->GetArray(array_name.c_str());
    if (contour_length_array) {
        for (size_t i = 0; i < ncells; ++i) {
            bonds[i] = std::make_shared<BondChain>(
                    bonds[i]->id_a, bonds[i]->id_b,
                    contour_length_array->GetTuple1(i));
        }
    }
}

} // namespace SG
