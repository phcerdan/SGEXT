/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "create_distance_map_function.hpp"

#include <chrono>

// DGtal
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/images/ImageContainerByITKImage.h>
#include <DGtal/images/imagesSetsUtils/SetFromImage.h>
#include <DGtal/images/imagesSetsUtils/ImageFromSet.h>
#include <DGtal/geometry/volumes/distance/ExactPredicateLpSeparableMetric.h>
#include <DGtal/geometry/volumes/distance/DistanceTransformation.h>
#include <DGtal/images/SimpleThresholdForegroundPredicate.h>

// ITK
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkChangeInformationImageFilter.h>
#include <itkInvertIntensityImageFilter.h>
// Compute DMap using ITK instead of DGtal
#include <itkApproximateSignedDistanceMapImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkAbsImageFilter.h>

// Boost Filesystem
#include <boost/filesystem.hpp>

namespace SG {


typename FloatImageType::Pointer create_distance_map_function_with_itk(
        const typename BinaryImageType::Pointer & input_img,
        const bool verbose)
{
    using DistanceMapFilter =
            itk::ApproximateSignedDistanceMapImageFilter<BinaryImageType,
                                                         FloatImageType>;
    auto dmap_filter = DistanceMapFilter::New();
    dmap_filter->SetOutsideValue(0);
    dmap_filter->SetInsideValue(255);
    dmap_filter->SetInput(input_img);

    // Theshold and abs filters to be analogous of the DGtal results of DMap.
    // Not interested in dmap values outside of the object.
    using ThresholdFilterType = itk::ThresholdImageFilter<FloatImageType>;
    auto threshold_filter = ThresholdFilterType::New();
    threshold_filter->SetInput(dmap_filter->GetOutput());
    threshold_filter->SetOutsideValue(0);
    threshold_filter->ThresholdAbove(0);

    // The dmap values inside the object are negative
    using AbsFilterType = itk::AbsImageFilter<FloatImageType, FloatImageType>;
    auto abs_filter = AbsFilterType::New();
    abs_filter->SetInput(threshold_filter->GetOutput());
    abs_filter->InPlaceOn();
    abs_filter->Update();
    return abs_filter->GetOutput();
}

typename FloatImageType::Pointer create_distance_map_function_with_dgtal(
        const typename BinaryImageType::Pointer & input_img,
        const bool verbose
        )
{
    // Image Typedefs
    // ITK
    using ItkImageType = SG::BinaryImageType;
    using PixelType = ItkImageType::PixelType;
    constexpr auto Dim = ItkImageType::ImageDimension;
    // DGtal
    using Domain = DGtal::Z3i::Domain;
    using Image = DGtal::ImageContainerByITKImage<Domain, PixelType>;
    using Space = DGtal::Z3i::Space;

    // Convert to DGtal Container
    Image image(input_img);

    auto start = std::chrono::system_clock::now();
    if(verbose) DGtal::trace.beginBlock("Create Distance Map");
    using Predicate = DGtal::functors::SimpleThresholdForegroundPredicate<Image>;
    Predicate aPredicate(image, 0);
    using L3Metric = DGtal::ExactPredicateLpSeparableMetric<Space, 3>;
    using DT = DGtal::DistanceTransformation<Space, Predicate, L3Metric>;
    L3Metric l3;
    DT dt(image.domain(), aPredicate, l3);
    if(verbose) DGtal::trace.endBlock();
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    if(verbose) std::cout << "Time elapsed: " << elapsed.count() << std::endl;

    using ItkFloatImageType = SG::FloatImageType;
    using FloatImage = DGtal::ImageContainerByITKImage<Domain, SG::FloatImageType::PixelType>;

    FloatImage dt_itk_image(dt.domain());
    std::copy(dt.constRange().begin(), dt.constRange().end(),
            dt_itk_image.range().outputIterator());

    using ChangeInformationFilter =
        itk::ChangeInformationImageFilter<ItkFloatImageType>;
    auto changeInfo = ChangeInformationFilter::New();
    changeInfo->ChangeAll();
    changeInfo->SetOutputOrigin(input_img->GetOrigin());
    changeInfo->SetOutputSpacing(input_img->GetSpacing());
    changeInfo->SetOutputDirection(input_img->GetDirection());
    changeInfo->SetInput(dt_itk_image.getITKImagePointer());
    changeInfo->Update();

    return changeInfo->GetOutput();
}

typename FloatImageType::Pointer
create_distance_map_function(const typename BinaryImageType::Pointer &input_img,
                             const bool use_itk_approximate,
                             const bool verbose) {
    if (use_itk_approximate) {
        return create_distance_map_function_with_itk(input_img, verbose);
    } else {
        return create_distance_map_function_with_dgtal(input_img, verbose);
    }
}

typename FloatImageType::Pointer create_distance_map_function_io(
        const std::string & input_filename,
        const std::string & outputFolder,
        const std::string & foreground,
        const bool use_itk_approximate,
        const bool verbose
        )
{
    if(!(foreground == "white" ||  foreground == "black")) {
        throw std::runtime_error(
                "foreground string not valid: " + foreground + ". Valid options: "
                " white | black."
                );
    }
    namespace fs = boost::filesystem;
    // Get filename without extension (and without folders).
    const fs::path input_stem = fs::path(input_filename).stem();
    // Output filename is fixed based on inut file
    const fs::path output_file_path = fs::path(input_stem.string() + "_DMAP");
    // Get full path for output
    const fs::path output_folder_path{outputFolder};
    const fs::path output_full_path =
        output_folder_path / fs::path(output_file_path.string() + ".nrrd");

    // ITK Image
    using ItkImageType = SG::BinaryImageType;
    using PixelType = ItkImageType::PixelType;
    constexpr auto Dim = ItkImageType::ImageDimension;
    // DGtal Image
    using Domain = DGtal::Z3i::Domain;
    using Image = DGtal::ImageContainerByITKImage<Domain, PixelType>;

    // Read Image using ITK
    using ReaderType = itk::ImageFileReader<ItkImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(input_filename);
    reader->Update();

    // Invert Filter using ITK.
    const bool invert_image = (foreground == "black") ? true : false;
    using InverterType =
        itk::InvertIntensityImageFilter<ItkImageType, ItkImageType>;
    auto inverter = InverterType::New();
    if(invert_image) {
        inverter->SetInput(reader->GetOutput());
        inverter->Update();
    }


    Image::ITKImagePointer handle_out =
        (invert_image) ? Image::ITKImagePointer(inverter->GetOutput())
        : Image::ITKImagePointer(reader->GetOutput());

    using ItkFloatImageType = SG::FloatImageType;
    const ItkFloatImageType::Pointer output_float_img =
        create_distance_map_function(handle_out, use_itk_approximate, verbose);

    // Write the image
    using ITKImageWriter = itk::ImageFileWriter<ItkFloatImageType>;
    auto writer = ITKImageWriter::New();
    try {
        writer->SetFileName(output_full_path.string().c_str());
        writer->SetInput(output_float_img);
        writer->UseCompressionOn();
        writer->Update();
    } catch(itk::ExceptionObject& e) {
        std::cerr << "Failure writing file: " << output_full_path.string()
            << std::endl;
        DGtal::trace.error() << e;
        throw DGtal::IOException();
    }

    return output_float_img;
}

} // end ns
