import sys as _sys
import sgext as _sgext

def itk_to_sgext(itk_image):
    try:
        import itk as _itk
    except ModuleNotFoundError as e:
        print("This function needs itk, pip install itk")
        raise e

    # Dev: it fails without View (copying it), probably because python
    # doesn't handle the memory of the copy as we might expect.
    # Other option would be to perform the copy here, and let ImportImageFilter
    # (used in from_pyarray function) to handle the memory.
    np_array = _itk.GetArrayViewFromImage(itk_image)
    if np_array.ndim != 3:
        raise TypeError("itk_image has dimension {}. Valid type is {}".format(np_array.ndim, 3))

    dtype = np_array.dtype
    valid_dtypes = ["uint8", "float"]
    if dtype not in valid_dtypes:
        raise TypeError("dtype of the itk_image {} not valid. Valid types are {}".format(dtype, valid_dtypes))

    if dtype == "float":
        sgext_image = _sgext.itk.IF3P()
    elif dtype == "uint8":
        sgext_image = _sgext.itk.IUC3P()
    sgext_image.from_pyarray(np_array)
    origin = itk_image.GetOrigin()
    sgext_image.set_origin(_itk.numpy.array([origin[0], origin[1], origin[2]]))
    spacing = itk_image.GetSpacing()
    sgext_image.set_spacing(_itk.numpy.array([spacing[0], spacing[1], spacing[2]]))
    sgext_image.set_direction(_itk.GetArrayFromMatrix(itk_image.GetDirection()))
    return sgext_image

def sgext_to_itk(sgext_image):
    try:
        import itk as _itk
    except ModuleNotFoundError as e:
        print("This function needs itk, pip install itk")
        raise e

    # Populate the image pixels
    itk_image = _itk.GetImageFromArray(sgext_image.to_pyarray())
    # Populate metadata (origin, spacing, direction)
    PixelType = _itk.F
    Dimension = 3
    PointType = _itk.Point[PixelType, Dimension]

    sgext_origin = sgext_image.origin()
    origin = PointType()
    sgext_spacing = sgext_image.spacing()
    spacing = PointType()

    for d in range(Dimension):
        origin[d] = sgext_origin[d]
        spacing[d] = sgext_spacing[d]

    itk_image.SetOrigin(origin)
    itk_image.SetSpacing(spacing)
    # direction_matrix = _itk.GetVnlMatrixFromArray(sgext_image.direction())
    itk_image.SetDirection(sgext_image.direction())

    return itk_image
