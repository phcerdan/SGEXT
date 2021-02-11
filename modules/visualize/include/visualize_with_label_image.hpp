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

#ifndef VISUALIZE_WITH_LABEL_IMAGE_HPP
#define VISUALIZE_WITH_LABEL_IMAGE_HPP

#include "sgextImagePlaneWidget.h"
#include "visualize_common.hpp"

#include <vtkButtonWidget.h>
#include <vtkCamera.h>
#include <vtkCallbackCommand.h>
#include <vtkCaptionActor2D.h>
#include <vtkCellPicker.h>
#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNamedColors.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTexturedButtonRepresentation2D.h>

#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>

namespace SG {

/**
 * Blend images, used in @see view_image_with_label
 *
 * @param inputImages first image could be any single component type,
 * the rest of images are label image types.
 * @param label_opacity label images are blend with the input (first)
 * using this opacity.
 *
 * @return blend filter, GetOutput will be a RGBA image.
 */
vtkSmartPointer<vtkImageBlend> blend_images_for_view(
        const std::vector<vtkSmartPointer<vtkImageData>> inputImages,
        const double label_opacity = 0.8);

/**
 * Create the three planes of the 3D blendImage, used in @see
 * view_image_with_label.
 *
 * @param blendImage output vtkImageData of @see blend_images_for_view
 * It should be a RGBA image.
 * @param renderWindowInteractor input interactor
 * @param imagesForCursorDisplay used to display the original value of the
 * images, should be the same that the parameter inputImages from
 * @blend_images_for_view
 * @param namesForCursorDisplay names to display the values,
 *  should be the same size than imagesForCursorDisplay,
 *  for example: {{"input"}, {"label"}}.
 * @param picker_shared picker shared between the three planes.
 * If nullptr, one will be created.
 *
 * @return three planes
 */
std::array<vtkSmartPointer<sgextImagePlaneWidget>, 3>
create_slice_planes(vtkImageData *blendImage, // Assume RGBA already
                    vtkRenderWindowInteractor *renderWindowInteractor,
                    const std::vector<vtkSmartPointer<vtkImageData>>
                            &imagesForCursorDisplay,
                    const std::vector<std::string> &namesForCursorDisplay,
                    vtkSmartPointer<vtkCellPicker> picker_shared = nullptr);

/**
 * button_callback for view_image_with_label.
 * Updates the blender opacity when button is pressed, and change the text.
 * Because the blender and the planes are not connected via pipelines, we
 * need to update the planes manually.
 */
struct DisableBlendButtonCallbackCommand : public vtkCommand {
    static DisableBlendButtonCallbackCommand *New() {
        return new DisableBlendButtonCallbackCommand;
    }

    DisableBlendButtonCallbackCommand() = default;

    void SetCaptionActor(vtkCaptionActor2D *input_caption_actor) {
        caption_actor = input_caption_actor;
    }
    void SetBlender(vtkImageBlend *input_blender) { blender = input_blender; }
    void SetBlenderLabelOpacity(const double *input_blender_label_opacity) {
        blender_label_opacity = input_blender_label_opacity;
    }
    void SetPlanes(const std::array<vtkSmartPointer<sgextImagePlaneWidget>, 3>
                           &input_planes) {
        planes = input_planes;
    }

    void Execute(vtkObject *caller,
                 unsigned long /*eventId*/,
                 void * /*callData*/) override {
        if (!blender)
            throw std::runtime_error("Missing blender");
        if (!blender_label_opacity)
            throw std::runtime_error("Missing blender_label_opacity");
        if (!caption_actor)
            throw std::runtime_error("Missing caption_actor");
        for (auto &plane : planes) {
            if (!plane)
                throw std::runtime_error("Missing planes");
        }

        auto *buttonWidget = reinterpret_cast<vtkButtonWidget *>(caller);
        auto *rep = reinterpret_cast<vtkTexturedButtonRepresentation2D *>(
                buttonWidget->GetRepresentation());
        auto state = rep->GetState();
        if (state != 0) {
            caption_actor->SetCaption("Blender: ON");
            blender->SetOpacity(1, *blender_label_opacity);

        } else {
            caption_actor->SetCaption("Blender: OFF");
            blender->SetOpacity(1, 0.);
        }
        blender->Update();
        for (auto &plane : planes) {
            auto slice_position = plane->GetSlicePosition();
            plane->SetInputData(blender->GetOutput());
            plane->SetSlicePosition(slice_position);
        }
    }
    vtkCaptionActor2D *caption_actor{nullptr};
    vtkImageBlend *blender{nullptr};
    const double *blender_label_opacity{nullptr};
    std::array<vtkSmartPointer<sgextImagePlaneWidget>, 3> planes;
};

/**
 * view_image_with_label is used to visualize any image (float) and
 * a matching label image (binarized image)
 *
 * input and label images should have the same region.

 * @tparam TInputImage Any single component itk image.
 * @tparam TLabelImage A binarized image (ideally with a single label/value).
 *.
 * @param input_image any itk image.
 * @param label_image any itk binary image (with a single label).
 * @param label_opacity used in the blender to superpose the label image
 * on top of the input image.
 * @param win_title name of the vtk window
 * @param win_x width of the vtk window
 * @param win_y height of the vtk window.
 */
template <typename TInputImage, typename TLabelImage>
void view_image_with_label(
        const TInputImage *input_image,
        const TLabelImage *label_image,
        const double &label_opacity = 0.8,
        const std::string &win_title = "SGEXT view with label",
        size_t win_x = 600,
        size_t win_y = 600) {
    // Get vtkImageData from input image(s)
    using ConnectorInputImage = itk::ImageToVTKImageFilter<TInputImage>;
    typename ConnectorInputImage::Pointer connectorInput =
            ConnectorInputImage::New();
    connectorInput->SetInput(input_image);
    connectorInput->Update();
    connectorInput->UpdateLargestPossibleRegion();
    using ConnectorLabelImage = itk::ImageToVTKImageFilter<TLabelImage>;
    typename ConnectorLabelImage::Pointer connectorLabel =
            ConnectorLabelImage::New();
    connectorLabel->SetInput(label_image);
    connectorLabel->Update();
    connectorLabel->UpdateLargestPossibleRegion();

    const std::vector<vtkSmartPointer<vtkImageData>> inputImages{
            {connectorInput->GetOutput()}, {connectorLabel->GetOutput()}};
    // Names for showing in the widget
    const std::vector<std::string> namesForCursorDisplay{{"input"}, {"label"}};

    // Blend images
    auto blender = blend_images_for_view(inputImages, label_opacity);

    // Setup renderers
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    // Setup render window
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetWindowName(win_title.c_str());
    renderWindow->SetSize(win_x, win_y);
    renderWindow->AddRenderer(renderer);

    // Setup render window interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkInteractorStyleRubberBand3D> style =
            vtkSmartPointer<vtkInteractorStyleRubberBand3D>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Setup the background gradient
    renderer->GradientBackgroundOn();
    renderer->SetBackground(0.5, 0.5, 0.5);
    renderer->SetBackground2(0.1, 0.1, 0.1);

    /** SLICES */
    std::array<vtkSmartPointer<sgextImagePlaneWidget>, 3> slice_planes =
            create_slice_planes(blender->GetOutput(), renderWindowInteractor,
                                inputImages, namesForCursorDisplay);

    /** Create button to show without blend **/
    auto namedColors = vtkSmartPointer<vtkNamedColors>::New();
    unsigned char color_off[4];
    namedColors->GetColor("DarkRed", color_off);
    unsigned char color_on[4];
    namedColors->GetColor("DarkGreen", color_on);
    auto button_texture_off = create_texture_for_button(color_off);
    auto button_texture_on = create_texture_for_button(color_on);
    const double button_size = 30.0;
    auto button_bounds = create_bounds_for_button(button_size);
    auto button_rep = create_on_off_representation_for_button(
            button_texture_off, button_texture_on, button_bounds);
    button_rep->SetState(1); // Starts on

    // Create an actor for the text
    const std::string caption_init = "Blender: ON";
    auto caption_actor =
            create_caption_actor_for_button(button_size, caption_init);

    renderer->AddViewProp(caption_actor);

    auto button_callback =
            vtkSmartPointer<DisableBlendButtonCallbackCommand>::New();
    button_callback->SetBlender(blender);
    button_callback->SetBlenderLabelOpacity(&label_opacity);
    button_callback->SetCaptionActor(caption_actor);
    button_callback->SetPlanes(slice_planes);

    auto button_widget = vtkSmartPointer<vtkButtonWidget>::New();
    button_widget->SetInteractor(renderWindowInteractor);
    button_widget->SetRepresentation(button_rep);
    button_widget->AddObserver(vtkCommand::StateChangedEvent, button_callback);
    button_widget->SetPriority(1.0); // So, it is always clickable
    button_widget->On();

    // Press "s" to toggle button, using SG::toggleTexturedButtonCallbackFunction.
    auto button_keypress_callback = vtkSmartPointer<vtkCallbackCommand>::New();
    button_keypress_callback->SetClientData(button_widget.GetPointer());
    button_keypress_callback->SetCallback(SG::toggleTexturedButtonCallbackFunction);
    renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, button_keypress_callback);

    vtkCamera *cam = renderer->GetActiveCamera();
    // Flip camera because VTK-ITK different direction of Y-axis.
    flip_camera(cam);
    cam->Azimuth(35);

    renderer->ResetCamera();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();
}

} // end namespace SG

#endif
