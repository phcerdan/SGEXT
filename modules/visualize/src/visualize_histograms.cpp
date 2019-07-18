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

#include "visualize_histograms.hpp"
#include "visualize_histo.hpp" // chart_from_histograms
#include <QApplication>
#include <QGridLayout>
#include <QMainWindow>
#include <QSurfaceFormat>
#include <QVTKOpenGLWidget.h> // from vtk, requires Qt>5.4 and OpenGL2 in vtk
#include <vtkGenericOpenGLRenderWindow.h>
namespace SG {

void visualize_histograms(
        const std::vector<histo::Histo<double>> &input_histograms) {
    // before initializing QApplication, set the default surface format.
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
    int argc(1);
    char **argv(nullptr);
    QApplication app(argc, argv);
    app.setApplicationName("Histograms");
    auto *main_window = new QMainWindow;
    auto *central_widget = new QWidget;
    main_window->setCentralWidget(central_widget);
    auto *layout = new QGridLayout;
    central_widget->setLayout(layout);
    for (auto &histo : input_histograms) {
        auto window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
        auto *qvtkWidget = new QVTKOpenGLWidget;
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
    main_window->resize(640, 480); // resize to something or it will fail
    main_window->show();
    app.exec();
}
} // namespace SG
