import _sgext.itk as itk
import _sgext.scripts as scripts
import unittest
import os, tempfile
import numpy as np

class TestITKImagePointer(unittest.TestCase):
    # This setup only occurs once for the the whole class
    @classmethod
    def setUpClass(cls):
        # Create temp directory
        cls.test_dir = tempfile.mkdtemp()
        dirname = os.path.dirname(os.path.abspath(__file__))
        cls.input = os.path.join(dirname, '../../../images/bX3D.tif')
        if not os.path.exists(cls.input):
            raise "Input image for thin script in test_itk_image not found: " + cls.input
        cls.img =scripts.thin(input=cls.input,
                          out_folder=cls.test_dir,
                          foreground="black",
                          skel_type="end", select_type="first",
                          persistence=2,
                          visualize=False,
                          verbose=True)


    def test_basics(self):
        # Default Constructor (empty)
        image = itk.IUC3P()
        # Read-only functions
        self.assertEqual(image.dimension(), 3)
        self.assertAlmostEqual(image.size().all(), 0.)
        self.assertAlmostEqual(image.origin().all(), 0.)
        self.assertAlmostEqual(image.spacing().all(), 1.)
        self.assertAlmostEqual(image.direction()[0,0], 1.)

    def test_to_pyarray(self):
        img = self.img
        self.assertEqual(img.dimension(), 3)
        np.testing.assert_array_equal(img.size(), [50,50,7])
        self.assertAlmostEqual(img.origin().all(), 0.)
        np.testing.assert_array_almost_equal(img.spacing(), [0.352734, 0.352734, 1])
        self.assertAlmostEqual(img.direction()[0,0], 1.)

        # Fortran contiguous (Default)
        arr_f = img.to_pyarray(contig="F")
        np.testing.assert_array_equal(arr_f.shape, [7, 50, 50])
        # C contiguous
        arr_c = img.to_pyarray(contig="C")
        np.testing.assert_array_equal(arr_c.shape, img.size())

    def test_export_to_itk(self):
        try:
            import itk
        except:
            print("Warning: itk python package not found. Not testing exporting to itk")
            return

        img = self.img
        # Test with C contiguous
        arr_c = img.to_pyarray(contig='C')
        itk_img_c = itk.GetImageFromArray(arr_c)
        self.assertEqual(arr_c.shape[0], 50)
        # ITK always assume than input numpy array will be F contig (which is easier!)
        self.assertEqual(itk_img_f.GetLargestPossibleRegion().GetSize()[0], 7)
        subs = itk.subtract_image_filter(itk_img_c, itk_img_c)
        subs_np = itk.GetArrayViewFromImage(subs) # no-copy
        self.assertEqual(subs_np.max(), 0)

        # Test with Fortran contiguous
        arr_f = img.to_pyarray(contig='F')
        itk_img_f = itk.GetImageFromArray(arr_f)
        self.assertEqual(arr_f.shape[0], 7)
        self.assertEqual(itk_img_f.GetLargestPossibleRegion().GetSize()[0], 50)

        # Check usability with a filter
        itk_img_f.GetLargestPossibleRegion().GetSize()[0]
        subs = itk.subtract_image_filter(itk_img_f, itk_img_f)
        subs_np = itk.GetArrayViewFromImage(subs) # no-copy
        self.assertEqual(subs_np.max(), 0)





