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

#ifndef VISUALIZE_HISTO_HPP
#define VISUALIZE_HISTO_HPP

#include "histo.hpp"
#include <vtkSmartPointer.h>
#include <vtkChartXY.h>

#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkTable.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAxis.h>
#include <vtkPlot.h>

namespace histo {

template <typename THisto>
vtkSmartPointer<vtkChartXY> chart_from_histogram(
    const THisto& input_histo, vtkIdType chart_type = vtkChart::LINE) {
  auto chart = vtkSmartPointer<vtkChartXY>::New();
  vtkNew<vtkTable> table;
  table->SetNumberOfRows(input_histo.bins);
  vtkNew<vtkFloatArray> xArray;
  xArray->SetName("Bins");
  xArray->SetNumberOfValues(input_histo.bins);
  table->AddColumn(xArray.GetPointer());
  vtkNew<vtkFloatArray> yArray;
  yArray->SetName("Counts");
  yArray->SetNumberOfValues(input_histo.bins);
  table->AddColumn(yArray.GetPointer());

  auto centers = input_histo.ComputeBinCenters();
  for(size_t j = 0; j != input_histo.bins; j++) {
    xArray->SetValue(j, centers[j]);
    yArray->SetValue(j, input_histo.counts[j]);
  }
  // auto const &xAxis = chart->GetAxis(vtkAxis::BOTTOM) ;
  // xAxis->SetRange(input_histo.range.first, input_histo.range.second);
  // xAxis->SetTitle(input_histo.name);
  // xAxis->SetBehavior(vtkAxis::FIXED);
  // chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
  // chart->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);
  chart->GetAxis(vtkAxis::LEFT)->SetTitle("#");
  chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("bins");
  chart->SetTitle(input_histo.name);
  auto points = chart->AddPlot(chart_type);
  points->SetInputData(table, 0, 1);
  return chart;
}
/**
 * Visualize histogram using VTK chart.
 *
 * @param input histo
 */
template <typename THisto>
void visualize_histo(const THisto& input_histo,
                     vtkIdType chart_type = vtkChart::LINE, size_t size_x = 640,
                     size_t size_y = 480) {
  auto chart = chart_from_histogram(input_histo, chart_type);
  // Set up the view
  auto view = vtkSmartPointer<vtkContextView>::New();
  view->GetScene()->AddItem(chart);
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(size_x, size_y);
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();
}

}  // namespace histo
#endif
