#include <vtkInteractorStyleTrackballCamera.h>

class vtkInteractorStyleTrackballCameraGraph
    : public vtkInteractorStyleTrackballCamera {
public:
  static vtkInteractorStyleTrackballCameraGraph* New();
  vtkTypeMacro(vtkInteractorStyleTrackballCameraGraph,
               vtkInteractorStyleTrackballCamera)

  virtual void OnChar() override;
};
