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

#include "render_binary_volume.hpp"
#include <itkImageToVTKImageFilter.h>
#include <vtkColorTransferFunction.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkContourValues.h>
#include <vtkMatrix4x4.h>


namespace SG {
vtkSmartPointer<vtkSmartVolumeMapper> volume_mapper_for_binary_image(BinaryImageType * binary_image)
{
    // Create the volume mapper (there are multiple options: GPU, CPU)
    auto volume_mapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();

    // Connect mapper to vtkImageData:
    //   First: convert itk image to vtkImageData
    using ImageToVTKImageFilter = itk::ImageToVTKImageFilter<BinaryImageType>;
    ImageToVTKImageFilter::Pointer image_to_vtk_filter = ImageToVTKImageFilter::New();
    image_to_vtk_filter->SetInput(binary_image);
    image_to_vtk_filter->Update();

    vtkSmartPointer<vtkImageData> vtk_image_data = image_to_vtk_filter->GetOutput();
    volume_mapper->SetInputData(vtk_image_data);
    return volume_mapper;
}

vtkSmartPointer<vtkVolumeProperty> volume_property_for_binary_image(
        const unsigned char label, const double opacity)
{
    auto volume_property = vtkSmartPointer<vtkVolumeProperty>::New();
    // Set volume properties: Opacity, Color, Shades, effects, etc.
    // Opacity: Only show input label
    vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
    // opacityTransferFunction->AddPoint(x, opacity, midpoint, sharpness);
    opacityTransferFunction->AddPoint(0,     0.0, 0.0, 1.0);
    opacityTransferFunction->AddPoint(255,   0.0, 0.0, 1.0);
    opacityTransferFunction->AddPoint(label, opacity, 0.0, 1.0);

    // Color
    vtkNew<vtkColorTransferFunction> colorTransferFunction;
    // colorTransferFunction->AddRGBPoint(x,    r,   g,   b, midpoint, sharpness);
    colorTransferFunction->AddRGBPoint(0,     0.0, 0.0, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(255,   0.0, 0.0, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(label, 1.0, 1.0, 1.0, 0.0, 1.0);


    volume_property->SetColor(colorTransferFunction);
    volume_property->SetScalarOpacity(opacityTransferFunction);
    volume_property->GetIsoSurfaceValues()->SetNumberOfContours(1);
    volume_property->GetIsoSurfaceValues()->SetValue(0, label);
    // volume_property->ShadeOn();
    // volume_property->SetInterpolationTypeToNearest(); // Default
    return volume_property;
}

vtkSmartPointer<vtkVolume> volume_actor_for_binary_image(
        BinaryImageType * binary_image,
        const unsigned char label,
        const double opacity) {
    auto volume_actor = vtkSmartPointer<vtkVolume>::New();
    auto volume_mapper = volume_mapper_for_binary_image(binary_image);
    auto volume_property = volume_property_for_binary_image(label, opacity);
    volume_actor->SetMapper(volume_mapper);
    volume_actor->SetProperty(volume_property);

    // Here we take care of position and orientation
    // so that volume is in DICOM patient physical space
    auto direction = binary_image->GetDirection();
    auto mat = vtkSmartPointer<vtkMatrix4x4>::New(); // start with identity
                                                     // matrix
    for (int i = 0; i < 3; i++)
        for (int k = 0; k < 3; k++)
            mat->SetElement(i, k, direction(i, k));

    // Counteract the built-in translation by origin
    auto origin = binary_image->GetOrigin();
    volume_actor->SetPosition(-origin[0], -origin[1], -origin[2]);

    // Add translation to the user matrix
    for (int i = 0; i < 3; i++) {
        mat->SetElement(i, 3, origin[i]);
    }
    volume_actor->SetUserMatrix(mat);

    return volume_actor;
}
} // end namespace SG
