/* ********************************************************************
 * Copyright (C) 2021 Pablo Hernandez-Cerdan.
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

#include "visualize_with_label_image.hpp"

#include <vtkColorSeries.h>
#include <vtkImageMapToColors.h>
#include <vtkNamedColors.h>
#include <vtkPlane.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTexture.h>
#include <vtkWindowLevelLookupTable.h>

namespace SG {

vtkSmartPointer<vtkImageMapToColors> vtkImage_to_rgba_filter(vtkImageData* input_image)
{
    double range_first[2];
    input_image->GetScalarRange(range_first);
    const double &min_intensity = range_first[0];
    const double &max_intensity = range_first[1];
    const double window = max_intensity - min_intensity;
    const double level = min_intensity + window / 2;

    auto window_level_lookup_input =
            vtkSmartPointer<vtkWindowLevelLookupTable>::New();
    window_level_lookup_input->SetWindow(window);
    window_level_lookup_input->SetLevel(level);
    window_level_lookup_input->Build();
    // If the min of the image is equal to min of the type, make it transparent
    // (background).
    if (min_intensity == input_image->GetScalarTypeMin()) {
        window_level_lookup_input->SetTableValue(min_intensity, 0, 0, 0, 0);
    }

    // Output directly a RGBA.
    auto image_map_to_colors_input =
            vtkSmartPointer<vtkImageMapToColors>::New();
    image_map_to_colors_input->SetInputData(input_image);
    image_map_to_colors_input->SetLookupTable(window_level_lookup_input);
    image_map_to_colors_input->SetOutputFormatToRGBA();
    image_map_to_colors_input->Update();
    return image_map_to_colors_input;
}

vtkSmartPointer<vtkImageData> vtkImage_to_rgba(vtkImageData* input_image) {
    auto filter = vtkImage_to_rgba_filter(input_image);
    return filter->GetOutput();
}

vtkSmartPointer<vtkImageBlend> blend_images_for_view(
        const std::vector<vtkSmartPointer<vtkImageData>> inputImages,
        const double label_opacity) {

    const size_t num_images = inputImages.size();
    if (num_images < 2) {
        throw std::runtime_error(
                "Need at least two images for blend_images_for_view");
    }
    auto blender = vtkSmartPointer<vtkImageBlend>::New();

    auto &first_image = inputImages[0];
    auto image_map_to_colors_input = vtkImage_to_rgba_filter(first_image);

    // std::cout << "input MapToColor #components " <<
    // image_map_to_colors_input->GetOutput()->GetNumberOfScalarComponents() <<
    // std::endl; unsigned char *pix_input =
    //   (unsigned char
    //   *)image_map_to_colors_input->GetOutput()->GetScalarPointer(24, 24, 4);
    // printf("r,g,b : %d, %d, %d, %d \n", pix_input[0], pix_input[1],
    // pix_input[2], pix_input[3]);

    // Add first image to blender
    blender->AddInputConnection(image_map_to_colors_input->GetOutputPort());

    const std::string color_scheme = "Brewer Qualitative Set1";
    auto colorSeries = vtkSmartPointer<vtkColorSeries>::New();

    for (size_t img_index = 1; img_index < num_images; ++img_index) {
        auto &image_label = inputImages[img_index];
        double range_label[2];
        image_label->GetScalarRange(range_label);
        auto max_label_value = range_label[1];

        colorSeries->SetColorSchemeByName(color_scheme.c_str());

        vtkSmartPointer<vtkLookupTable> lookup_table_label =
                vtkSmartPointer<vtkLookupTable>::New();
        lookup_table_label->IndexedLookupOn(); // categorical
        const int number_of_values_label = 2;  // (only one + background)
        lookup_table_label->SetNumberOfTableValues(number_of_values_label);
        // Nan color by default is red
        // lookup_table_label->SetNanColor(1.0, 1.0, 1.0, 0.3); // Nan to white
        // (with transparency)
        {
            const double annotated_value = 0;
            const int index = 0;
            lookup_table_label->SetAnnotation(annotated_value,
                                              std::to_string(annotated_value));
            lookup_table_label->SetTableValue(index, 0, 0, 0, 0.0);
        }
        {
            const double annotated_value = max_label_value;
            const int index = 1;
            auto color = colorSeries->GetColorRepeating(index);
            lookup_table_label->SetAnnotation(annotated_value,
                                              std::to_string(annotated_value));
            lookup_table_label->SetTableValue(index, color.GetRed() / 255.,
                                              color.GetGreen() / 255.,
                                              color.GetBlue() / 255., 1.0);
        }

        auto image_map_to_colors_label =
                vtkSmartPointer<vtkImageMapToColors>::New();
        image_map_to_colors_label->SetInputData(image_label);
        image_map_to_colors_label->SetLookupTable(lookup_table_label);
        image_map_to_colors_label->SetOutputFormatToRGBA();
        image_map_to_colors_label->Update();

        // std::cout << "label MapToColor #components " <<
        // image_map_to_colors_label->GetOutput()->GetNumberOfScalarComponents()
        // << std::endl; unsigned char *pix_label =
        //   (unsigned char
        //   *)image_map_to_colors_label->GetOutput()->GetScalarPointer(24, 24,
        //   4);
        // printf("r,g,b : %d, %d, %d, %d \n", pix_label[0], pix_label[1],
        // pix_label[2], pix_label[3]);

        blender->AddInputConnection(image_map_to_colors_label->GetOutputPort());
        blender->SetOpacity(img_index, label_opacity);
    }

    blender->Update();

    // std::cout << "blender_output  #components: " <<
    // blender->GetOutput()->GetNumberOfScalarComponents() << std::endl;
    // unsigned char *pix_blender = (unsigned char
    // *)blender->GetOutput()->GetScalarPointer(24, 24, 4); printf("r,g,b : %d,
    // %d, %d, %d \n", pix_blender[0], pix_blender[1], pix_blender[2],
    // pix_blender[3]);

    return blender;
}

std::array<vtkSmartPointer<sgextImagePlaneWidget>, 3>
create_slice_planes(vtkImageData *blendImage, // Assume RGBA already
                    vtkRenderWindowInteractor *renderWindowInteractor,
                    const std::vector<vtkSmartPointer<vtkImageData>>
                            &imagesForCursorDisplay,
                    const std::vector<std::string> &namesForCursorDisplay,
                    vtkSmartPointer<vtkCellPicker> picker_shared) {

    if (imagesForCursorDisplay.size() != namesForCursorDisplay.size()) {
        const std::string message_error =
                "Provide the same number of elements for "
                "imagesForCursorDisplay and namesForCursorDisplay";
        throw std::runtime_error(message_error);
    }
    // Create shared picker for three planes:
    if (picker_shared == nullptr) {
        picker_shared = vtkSmartPointer<vtkCellPicker>::New();
        picker_shared->SetTolerance(0.005); // need some fluff
    }
    /** SLICES */
    std::array<vtkSmartPointer<sgextImagePlaneWidget>, 3> slice_planes;
    for (unsigned int i = 0; i < 3; ++i) {
        slice_planes[i] = vtkSmartPointer<sgextImagePlaneWidget>::New();
    }

    // Color the edges (slicer like)
    auto namedColors = vtkSmartPointer<vtkNamedColors>::New();
    double edge_color_x[3];
    double edge_color_y[3];
    double edge_color_z[3];
    namedColors->GetColor("Yellow", edge_color_x);
    namedColors->GetColor("DarkRed", edge_color_y);
    namedColors->GetColor("DarkGreen", edge_color_z);
    slice_planes[0]->GetPlaneProperty()->SetColor(edge_color_x);
    slice_planes[1]->GetPlaneProperty()->SetColor(edge_color_y);
    slice_planes[2]->GetPlaneProperty()->SetColor(edge_color_z);

    // All selected planes to white
    slice_planes[0]->GetSelectedPlaneProperty()->SetColor(1., 1., 1.);
    slice_planes[1]->GetSelectedPlaneProperty()->SetColor(1., 1., 1.);
    slice_planes[2]->GetSelectedPlaneProperty()->SetColor(1., 1., 1.);

    // Get center of the input image to place the planes
    double input_center[3];
    blendImage->GetCenter(input_center);

    for (unsigned int i = 0; i < 3; ++i) {
        // Share picker
        slice_planes[i]->SetPicker(picker_shared);
        // sgextImagePlaneWidget only
        for (size_t img_cursor_index = 0;
             img_cursor_index < imagesForCursorDisplay.size();
             ++img_cursor_index) {
            slice_planes[i]->AddImageForCursorDisplay(
                    imagesForCursorDisplay[img_cursor_index],
                    namesForCursorDisplay[img_cursor_index]);
        }

        // Increase width of plane outline
        slice_planes[i]->GetPlaneProperty()->SetLineWidth(3.0);
        slice_planes[i]->GetPlaneProperty()->RenderLinesAsTubesOn();
        slice_planes[i]->GetSelectedPlaneProperty()->SetLineWidth(4.0);
        slice_planes[i]->GetSelectedPlaneProperty()->RenderLinesAsTubesOn();
        // All selected planes have white outline
        slice_planes[i]->GetSelectedPlaneProperty()->SetColor(1., 1., 1.);

        vtkCoordinate *coord =
                slice_planes[i]->TextActor->GetPositionCoordinate();
        coord->SetCoordinateSystemToNormalizedViewport();
        coord->SetValue(.01, .99);
        slice_planes[i]
                ->TextActor->GetTextProperty()
                ->SetVerticalJustificationToTop();

        // Passthrough (input is already RGBA)
        slice_planes[i]->GetColorMap()->SetLookupTable(nullptr);
        slice_planes[i]->GetColorMap()->PassAlphaToOutputOn();
        slice_planes[i]->DisplayTextOn();
        slice_planes[i]->TextureInterpolateOff();
        slice_planes[i]->GetTexture()->InterpolateOff();
        slice_planes[i]->SetResliceInterpolateToNearestNeighbour();
        slice_planes[i]->SetInteractor(renderWindowInteractor);
        slice_planes[i]->SetInputData(blendImage);
        slice_planes[i]->GetMarginProperty()->SetOpacity(0);
        slice_planes[i]->SetMarginSizeX(0);
        slice_planes[i]->SetMarginSizeY(0);
        slice_planes[i]->SetRightButtonAction(
                sgextImagePlaneWidget::VTK_SLICE_MOTION_ACTION);
        slice_planes[i]->SetMiddleButtonAction(
                sgextImagePlaneWidget::VTK_WINDOW_LEVEL_ACTION);
        slice_planes[i]->SetPlaneOrientation(i);
        slice_planes[i]->UpdatePlacement();

        // Not needed, autocomputed from blendImage
        // slice_planes[i]->SetWindowLevel(window, level);

        // if PlaceWidget here (after SetPlaneOrientation), planes are centerd
        // if not called, planes are at origin
        slice_planes[i]->PlaceWidget();
        slice_planes[i]->On();
    }
    return slice_planes;
}

} // end namespace SG
