/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "write_vtu_file.hpp"

#include <vtkDoubleArray.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>

namespace SG {
void write_vtu_file(const System &sys, const std::string &file_name) {
    auto ugrid = vtkUnstructuredGrid::New();
    // Set geometry with position of points
    auto vtk_points = vtkSmartPointer<vtkPoints>::New();
    using particle_id_t = size_t;
    std::unordered_map<particle_id_t, vtkIdType> particle_id_to_vtk_id_map;
    for (const auto &particle : sys.all.particles) {
        const auto &pos = particle.pos;
        particle_id_to_vtk_id_map[particle.id] =
                vtk_points->InsertNextPoint(pos[0], pos[1], pos[2]);
    }
    ugrid->SetPoints(vtk_points);
    // Set topology using bonds
    auto unique_bonds = SG::unique_bonds(sys);
    // Allocate Number of cells, i.e number of bonds in the system;
    ugrid->Allocate(unique_bonds.size());
    for (const auto &bond : unique_bonds) {
        auto vtk_id_list = vtkIdList::New();
        vtk_id_list->InsertNextId(particle_id_to_vtk_id_map[bond.id_a]);
        vtk_id_list->InsertNextId(particle_id_to_vtk_id_map[bond.id_b]);
        auto line = vtkLine::New();
        ugrid->InsertNextCell(line->GetCellType(), vtk_id_list);
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

    size_t counter = 0;
    for (const auto &particle : sys.all.particles) {
        acc->SetTuple3(counter, particle.dynamics.acc[0],
                       particle.dynamics.acc[1], particle.dynamics.acc[2]);
        vel->SetTuple3(counter, particle.dynamics.vel[0],
                       particle.dynamics.vel[1], particle.dynamics.vel[2]);
        mass->SetTuple1(counter, particle.material.mass);
        radius->SetTuple1(counter, particle.material.radius);
        volume->SetTuple1(counter, particle.material.volume);
        counter++;
    }
    point_data->AddArray(acc);
    point_data->AddArray(vel);
    point_data->AddArray(mass);
    point_data->AddArray(radius);
    point_data->AddArray(volume);

    point_data->Update();

    auto ugrid_writer = vtkXMLUnstructuredGridWriter::New();
    ugrid_writer->SetFileName(file_name.c_str());
    ugrid_writer->SetInputData(ugrid);
    ugrid_writer->Update();
}
} // namespace SG
