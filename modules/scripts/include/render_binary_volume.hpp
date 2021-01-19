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

#ifndef RENDER_BINARY_VOLUME_HPP
#define RENDER_BINARY_VOLUME_HPP

#include "image_types.hpp" // for BinaryImageType
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolume.h> // actor
#include <vtkVolumeProperty.h>

namespace SG {
vtkSmartPointer<vtkSmartVolumeMapper> volume_mapper_for_binary_image(
        BinaryImageType * binary_image);

vtkSmartPointer<vtkVolumeProperty> volume_property_for_binary_image(
        const unsigned char label = 255,
        const double opacity = 0.8);

vtkSmartPointer<vtkVolume> volume_actor_for_binary_image(
        BinaryImageType * binary_image,
        const unsigned char label = 255,
        const double opacity = 0.8);
} // end namespace SG
#endif

