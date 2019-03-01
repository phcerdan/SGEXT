/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "visualize_histo.hpp" // chart_from_histograms
#include "visualize_histograms.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QGridLayout>
#include <QVTKOpenGLWidget.h> // from vtk, requires Qt>5.4 and OpenGL2 in vtk
#include <vtkGenericOpenGLRenderWindow.h>
#include <QSurfaceFormat>
namespace SG {

void visualize_histograms(const std::vector<histo::Histo<double>> & input_histograms)
{
    // before initializing QApplication, set the default surface format.
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
    int argc(1);
    char** argv(nullptr);
    QApplication app(argc, argv);
    app.setApplicationName("Histograms");
    auto * main_window = new QMainWindow;
    auto * central_widget = new QWidget;
    main_window->setCentralWidget(central_widget);
    auto * layout = new QGridLayout;
    central_widget->setLayout(layout);
    for(auto & histo : input_histograms)
    {
        auto window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
        auto * qvtkWidget = new QVTKOpenGLWidget;
        qvtkWidget->SetRenderWindow(window.Get());
        layout->addWidget(qvtkWidget);
        // TODO allow passing different than vtkChart::LINE? ... dirty
        auto chart = histo::chart_from_histogram(histo);
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
}
