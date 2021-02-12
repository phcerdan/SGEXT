#include <vtkAbstractPicker.h>
#include <vtkInteractorStyleTrackballCameraGraph.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>

vtkStandardNewMacro(vtkInteractorStyleTrackballCameraGraph)

void vtkInteractorStyleTrackballCameraGraph::OnChar() {
    switch (this->Interactor->GetKeyCode()) {
    case 'p':
        std::cout << "Picking pixel: "
                  << this->Interactor->GetEventPosition()[0] << " "
                  << this->Interactor->GetEventPosition()[1] << std::endl;
        this->Interactor->GetPicker()->Pick(
                this->Interactor->GetEventPosition()[0],
                this->Interactor->GetEventPosition()[1],
                0, // always zero.
                this->Interactor->GetRenderWindow()
                        ->GetRenderers()
                        ->GetFirstRenderer());
        double picked[3];
        this->Interactor->GetPicker()->GetPickPosition(picked);
        std::cout << "Picked value: " << picked[0] << " " << picked[1] << " "
                  << picked[2] << std::endl;
        this->Interactor->Render();
        break;
    }

    // Forward events
    vtkInteractorStyleTrackballCamera::OnChar();
}
