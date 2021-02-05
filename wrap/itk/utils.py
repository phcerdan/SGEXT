import sys as _sys
import sgext as _sgext

def dicom_filenames(folder, series_name = None):
    """
    Return a list with filenames from a serie of images from a dicom folder.
    By default, it assumes the dicom folder only contains one serie of images.
    If there are more than one, this function will raise an error
    printing all the series_names found in the folder.
    Pick one of those and re-run this function.

    Parameters:
    ----------
    folder: str
        Path to a dicom folder
    series_name: str [None]
        Specify the name of the serie, only used if input folder contains more than one.

    Example:
        filenames = sgext.utils.dicom_filenames(folder="/path/to/dicom_folder")
        itk_image = itk.imread(filenames, pixel_type = itk.F) # or itk.UC if BinaryImage
        sgext_image = sgext.from_to.itk_to_sgext(itk_image)
    """
    try:
        import itk as _itk
    except ModuleNotFoundError as e:
        print("This function needs itk, pip install itk")
        raise e

    import os as _os
    if not _os.path.isdir(folder):
        raise ValueError("Input folder does not exists.")

    namesGenerator = _itk.GDCMSeriesFileNames.New()
    namesGenerator.SetUseSeriesDetails(True)
    namesGenerator.AddSeriesRestriction("0008|0021")
    namesGenerator.SetGlobalWarningDisplay(False)
    namesGenerator.SetDirectory(folder)
    seriesUids = namesGenerator.GetSeriesUIDs()
    if series_name is None:
        num_series_ids = len(seriesUids)
        if num_series_ids == 1:
            identifier = seriesUids[0]
        elif num_series_ids == 0:
            raise ValueError("Input folder does not contain any dicom series.")
        else:
            message = "More than one identifier in the dicom folder, " \
            "specify one from:\nseriesUids: {}".format(seriesUids)
            raise ValueError(message)
    else: # series_name provided
        identifier = str(series_name)

    dicom_filenames = namesGenerator.GetFileNames(identifier)
    return dicom_filenames

def metadata(image, verbose = False):
    """
    Return the physical space metadata of input image as a tuple of numpy arrays:
    [origin, spacing, direction]

    image can be a sgext image (binary or float) or an itk image of any pixel type.

    Parameters:
    ----------
    image: sgext or itk image
        input image

    verbose: Bool
        Print the type of the image and the metadata with full precision.

    Returns:
    -------
    3-tuple of numpy.arrays:
        [origin, spacing, direction]
    """
    try:
        import numpy as _np
    except ModuleNotFoundError as e:
        print("This function needs numpy.")
        raise e

    try:
        import itk as _itk
    except ModuleNotFoundError as e:
        print("This function needs package itk.")
        raise e

    type_str = str(type(image))
    if verbose:
        print("type of image: ", type_str)

    if type_str == "<class 'sgext._sgext.itk.IUC3P'>" or type_str == "<class 'sgext._sgext.itk.IF3P'>":
        origin = image.origin()
        spacing = image.spacing()
        direction = image.direction()
    else:
        origin = _np.array(image.GetOrigin())
        spacing = _np.array(image.GetSpacing())
        direction = _itk.GetArrayFromMatrix(image.GetDirection())

    if verbose:
        with _np.printoptions(precision=20):
            print("origin: {}".format(origin))
            print("spacing: {} ".format(spacing))
            print("direction:\n{} ".format(direction))

    return [origin, spacing, direction]
