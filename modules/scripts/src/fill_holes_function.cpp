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
