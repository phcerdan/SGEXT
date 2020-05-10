/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fill_holes_function.hpp"
// Module ITKLabelVoting
#include <itkVotingBinaryIterativeHoleFillingImageFilter.h>
#include <itkStatisticsImageFilter.h>

namespace SG {
BinaryImageType::Pointer fill_holes_voting_iterative_function(
        const BinaryImageType::Pointer & input,
        const int & radius,
        const int & majority,
        const size_t & iterations,
        bool verbose
        )
{
  using FillingFilterType =
      itk::VotingBinaryIterativeHoleFillingImageFilter<BinaryImageType>;
  auto filler = FillingFilterType::New();
  filler->SetInput(input);
  if(verbose) {
    std::cout << "Majority: " << majority << std::endl;
    std::cout << "Iterations: " << iterations << std::endl;
    std::cout << "Radius: (" << radius << ", " << radius << ", " << radius
              << ")" << std::endl;
  }
  typedef itk::StatisticsImageFilter<BinaryImageType> StatisticsImageFilterType;
  typename StatisticsImageFilterType::Pointer statsFilter =
      StatisticsImageFilterType::New();
  statsFilter->SetInput(input);
  statsFilter->Update();
  statsFilter->UpdateLargestPossibleRegion();
  const auto max_intensity = statsFilter->GetMaximum();
  // const auto min_intensity = statsFilter->GetMinimum();

  filler->SetForegroundValue(max_intensity);
  filler->SetMajorityThreshold(majority);
  filler->SetMaximumNumberOfIterations(iterations);
  FillingFilterType::InputSizeType radius_array;
  radius_array.Fill(radius);
  filler->SetRadius(radius_array);
  filler->Update();

  return filler->GetOutput();
}
} // end ns SG
