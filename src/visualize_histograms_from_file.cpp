/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * See LICENSE on top of this repository. */

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "spatial_histograms.hpp"
#include "visualize_histograms.hpp"
#include "visualize_histo.hpp" // chart_from_histograms
#include <QApplication>
#include <QMainWindow>
#include <QGridLayout>
#include <QVTKOpenGLWidget.h> // from vtk, requires Qt>5.4 and OpenGL2 in vtk
#include <vtkGenericOpenGLRenderWindow.h>
#include <QSurfaceFormat>

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
  // // Approach with visualize_histograms (hard to tweak)
  // {
  //     std::vector<histo::Histo<double>> histograms;
  //     for( size_t i = 0; i < num_histograms; ++i)
  //     {
  //         histograms.emplace_back(SG::read_histogram(inFile));
  //     }
  //     SG::visualize_histograms(histograms);
  // }
  std::vector<histo::Histo<double>> histograms;
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
  int qargc(1);
  char** qargv(nullptr);
  QApplication app(qargc, qargv);
  app.setApplicationName("Histograms");
  auto * main_window = new QMainWindow;
  auto * central_widget = new QWidget;
  main_window->setCentralWidget(central_widget);
  auto * layout = new QGridLayout;
  central_widget->setLayout(layout);
  // Degrees
  {
      histograms.emplace_back(SG::read_histogram(inFile));
      auto window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
      auto * qvtkWidget = new QVTKOpenGLWidget;
      qvtkWidget->SetRenderWindow(window.Get());
      layout->addWidget(qvtkWidget);
      auto chart = histo::chart_from_histogram(histograms[0]);
      chart->GetAxis(vtkAxis::LEFT)->SetLogScale(true);
      auto view = vtkSmartPointer<vtkContextView>::New();
      view->SetRenderWindow(window.Get());
      view->GetScene()->AddItem(chart);
      view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
      // Connect vtk with qt
      view->SetInteractor(qvtkWidget->GetInteractor());
      qvtkWidget->SetRenderWindow(view->GetRenderWindow());
  }
  // Ete Distances
  {
      histograms.emplace_back(SG::read_histogram(inFile));
      auto window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
      auto * qvtkWidget = new QVTKOpenGLWidget;
      qvtkWidget->SetRenderWindow(window.Get());
      layout->addWidget(qvtkWidget);
      // TODO allow passing different than vtkChart::LINE? ... dirty
      auto chart = histo::chart_from_histogram(histograms[1]);
      chart->GetAxis(vtkAxis::LEFT)->SetLogScale(true);
      chart->GetAxis(vtkAxis::BOTTOM)->SetLogScale(true);
      auto view = vtkSmartPointer<vtkContextView>::New();
      view->SetRenderWindow(window.Get());
      view->GetScene()->AddItem(chart);
      view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
      // Connect vtk with qt
      view->SetInteractor(qvtkWidget->GetInteractor());
      qvtkWidget->SetRenderWindow(view->GetRenderWindow());
  }
  // Angles
  {
      histograms.emplace_back(SG::read_histogram(inFile));
      auto window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
      auto * qvtkWidget = new QVTKOpenGLWidget;
      qvtkWidget->SetRenderWindow(window.Get());
      layout->addWidget(qvtkWidget);
      auto chart = histo::chart_from_histogram(histograms[2]);
      auto view = vtkSmartPointer<vtkContextView>::New();
      view->SetRenderWindow(window.Get());
      view->GetScene()->AddItem(chart);
      view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
      // Connect vtk with qt
      view->SetInteractor(qvtkWidget->GetInteractor());
      qvtkWidget->SetRenderWindow(view->GetRenderWindow());
  }
  // Cosines
  {
      histograms.emplace_back(SG::read_histogram(inFile));
      auto window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
      auto * qvtkWidget = new QVTKOpenGLWidget;
      qvtkWidget->SetRenderWindow(window.Get());
      layout->addWidget(qvtkWidget);
      auto chart = histo::chart_from_histogram(histograms[3]);
      auto view = vtkSmartPointer<vtkContextView>::New();
      view->SetRenderWindow(window.Get());
      view->GetScene()->AddItem(chart);
      view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
      // Connect vtk with qt
      view->SetInteractor(qvtkWidget->GetInteractor());
      qvtkWidget->SetRenderWindow(view->GetRenderWindow());
  }
  main_window->resize(640, 480);// resize to something or it will fail
  main_window->show();
  app.exec();
}
