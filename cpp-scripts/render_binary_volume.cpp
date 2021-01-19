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
#include "visualize_common.hpp" // for flip_camera
#include "image_types.hpp" // for BinaryImageType

// itk
#include <itkImageFileReader.h>
// vtk
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace po = boost::program_options;

int main(int argc, char *const argv[]) {
    /*-------------- Parse command line -----------------------------*/
    po::options_description opt_desc("Allowed options are: ");
    opt_desc.add_options()("help,h", "display this message.");
    opt_desc.add_options()("input,i", po::value<std::string>()->required(),
                           "Input binary image.");
    opt_desc.add_options()("label,l",
                           po::value<unsigned char>()->default_value(255),
                           "label of the binary image (defaults to 255)");
    opt_desc.add_options()("opacity,p",
                           po::value<double>()->default_value(0.8),
                           "opacity of the volume.");
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, opt_desc), vm);
        if (static_cast<bool>(vm.count("help")) || argc <= 1) {
            std::cout << "Basic usage:\n" << opt_desc << "\n";
            return EXIT_SUCCESS;
        }
        po::notify(vm);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = vm["input"].as<std::string>();
    unsigned char label = vm["label"].as<unsigned char>();
    double opacity = vm["opacity"].as<double>();
    // read image
    using ReaderType = itk::ImageFileReader<SG::BinaryImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(filename);
    reader->Update();
    auto binary_image = reader->GetOutput();

    auto volume_actor = SG::volume_actor_for_binary_image(binary_image, label, opacity);

    // Render the actor with VTK window
    vtkNew<vtkRenderer> renderer ;
    vtkNew<vtkRenderWindow> renWin ;
    renWin->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> iren ;
    iren->SetRenderWindow(renWin);

    renderer->AddVolume(volume_actor);

    // Flip camera because VTK-ITK different corner for origin.
    vtkCamera *cam = renderer->GetActiveCamera();
    SG::flip_camera(cam);
    renderer->ResetCamera();

    renWin->Render();
    iren->Start();
    return EXIT_SUCCESS;
}
