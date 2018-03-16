/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * See LICENSE on top of this repository. */

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "spatial_histograms.hpp"
#include "visualize_histograms.hpp"

int main(int argc, char* const argv[]){

  if ( argc != 2)
    {
    std::cerr << "Usage: " << argv[0] << " filename.histo " << std::endl;
    return EXIT_FAILURE;
    }

  const std::string filename  = argv[1];
  std::ifstream inFile(filename.c_str());
  size_t nlines = std::count(std::istreambuf_iterator<char>(inFile),
             std::istreambuf_iterator<char>(), '\n');
  // Reset the file
  inFile.clear();
  inFile.seekg(0, std::ios::beg);
  // Assume it will be nlines/4 histograms. See print_histograms.
  size_t num_histograms = nlines / 4;
  std::cout << num_histograms << " histograms in " << filename << std::endl;
  std::vector<histo::Histo<double>> histograms;
  for( size_t i = 0; i < num_histograms; ++i)
  {
      histograms.emplace_back(SG::read_histogram(inFile));
  }
  SG::visualize_histograms(histograms);
}
