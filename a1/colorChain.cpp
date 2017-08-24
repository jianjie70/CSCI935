/**
 * CSCI935 - Assignment 1
 * Name:      Van Tam PHAN
 * ID:        5322169
 * Username:  vtp990 
 */
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
using namespace cv;
using namespace std;

void ColorInterpolation(Mat imgSrc, Mat & dst);
void ColorCorrection(Mat src, Mat & dst);
void GammaCorrection(Mat src, Mat & dst, float fGamma);
void Display4Images(Mat & im1, Mat & im2, Mat & im3, Mat & im4);

int main(int argc, char *argv[]) {
  //=========STAGE 1: LOAD IMAGE==============
  if (argc != 2) {
    cout << "colorChain" << endl;
    return -1;
  }
  Mat imgSrc = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);
  Mat originalImage = imread(argv[1], CV_LOAD_IMAGE_COLOR);

  if (!imgSrc.data) {
    cout << "Failed to open image." << endl;
    return -1;
  }
  cout << "image source: " << imgSrc.size() << endl;
  //=========STAGE 2: BILINEAR COLOR INTERPOLATION==============
  //CV_8UC3 unsigned char types that are 8 bit long 
  //and each pixel has three of these to form the three channels
  Mat imgCFAInterpol(imgSrc.size(), CV_8UC3);
  imgCFAInterpol = Scalar(0, 0, 0);  //BGR order. R = 255;
  ColorInterpolation(imgSrc, imgCFAInterpol);

  //=========STAGE 3: COLOR CORRECTION==============
  Mat imgColorCorrected(imgSrc.size(), CV_8UC3); //only accept value from 0 to 255
  ColorCorrection(imgCFAInterpol, imgColorCorrected);

  //=========STAGE 4: GAMMA CORRECTION==============
  Mat imgGammaCorrected(imgSrc.size(), CV_8UC3); //only accept value from 0 to 255
  GammaCorrection(imgColorCorrected, imgGammaCorrected, 0.5);

  //=========DISPLAY RESULTS==============
  Display4Images(originalImage, imgCFAInterpol, imgColorCorrected,
      imgGammaCorrected);

  waitKey(0);
  return 0;
}

void Display4Images(Mat & im1, Mat & im2, Mat & im3, Mat & im4) {
  Size size = im1.size();
  Mat dst(Size(size.width * 2, size.height * 2), CV_8UC3);
  Mat roi1 = dst(Rect(0, 0, size.width, size.height));
  im1.copyTo(roi1);
  Mat roi2 = dst(Rect(size.width, 0, size.width, size.height));
  im2.copyTo(roi2);
  Mat roi3 = dst(Rect(0, size.height, size.width, size.height));
  im3.copyTo(roi3);
  Mat roi4 = dst(Rect(size.width, size.height, size.width, size.height));
  im4.copyTo(roi4);

  Mat final;
  resize(dst, final, size);  //resize image
  // Display big mat
  namedWindow("Color Chain", CV_WINDOW_AUTOSIZE);
  imshow("Color Chain", final);
}

void ColorInterpolation(Mat src, Mat & dst) {
  Mat outR(src.size(), CV_8UC1), outG(src.size(), CV_8UC1), outB(src.size(),
      CV_8UC1);
  int height = src.rows;
  int width = src.cols;
  int row, col;
  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++) {
      //the row with green and red positions
      if (row % 2 == 0) {
        if (col % 2 == 0) // green positions
            {
          if (col == 0) // green at column 0
              {
            if (row == 0) // green at position (0, 0)
              outB.at<uchar>(row, col) = src.at<uchar>(row + 1, col);
            else
              outB.at<uchar>(row, col) = (src.at<uchar>(row - 1, col)
                  + src.at<uchar>(row + 1, col)) / 2;
            outR.at<uchar>(row, col) = src.at<uchar>(row, col + 1);
          } else    // green at other columns not 0
          {
            if (row == 0)
              outB.at<uchar>(row, col) = src.at<uchar>(row + 1, col);
            else
              outB.at<uchar>(row, col) = (src.at<uchar>(row - 1, col)
                  + src.at<uchar>(row + 1, col)) / 2;
            outR.at<uchar>(row, col) = (src.at<uchar>(row, col - 1)
                + src.at<uchar>(row, col + 1)) / 2;
          }
          outG.at<uchar>(row, col) = src.at<uchar>(row, col);
        } else    // red positions
        {
          if (col == width - 1) // red at column width-1
              {
            if (row == 0) // red at position (0, width-1)
                {
              outG.at<uchar>(row, col) = (src.at<uchar>(row, col - 1)
                  + src.at<uchar>(row + 1, col)) / 2;
              outB.at<uchar>(row, col) = src.at<uchar>(row + 1, col - 1);
            } else {
              outG.at<uchar>(row, col) = (src.at<uchar>(row - 1, col)
                  + src.at<uchar>(row, col - 1) + src.at<uchar>(row + 1, col))
                  / 3;
              outB.at<uchar>(row, col) = (src.at<uchar>(row - 1, col - 1)
                  + src.at<uchar>(row + 1, col - 1)) / 2;
            }
          } else    // red at other columns not width-1
          {
            if (row == 0) {
              outG.at<uchar>(row, col) = (src.at<uchar>(row, col - 1)
                  + src.at<uchar>(row, col + 1) + src.at<uchar>(row + 1, col))
                  / 3;
              outB.at<uchar>(row, col) = (src.at<uchar>(row + 1, col - 1)
                  + src.at<uchar>(row + 1, col + 1)) / 2;
            } else {
              outG.at<uchar>(row, col) = (src.at<uchar>(row - 1, col)
                  + src.at<uchar>(row + 1, col) + src.at<uchar>(row, col - 1)
                  + src.at<uchar>(row, col + 1)) / 4;
              outB.at<uchar>(row, col) = (src.at<uchar>(row - 1, col - 1)
                  + src.at<uchar>(row - 1, col + 1)
                  + src.at<uchar>(row + 1, col + 1)
                  + src.at<uchar>(row + 1, col - 1)) / 4;
            }
          }
          outR.at<uchar>(row, col) = src.at<uchar>(row, col);
        }
      }
      //the row with blue and green positions
      else {
        if (col % 2 == 0) // blue positions
            {
          if (col == 0) // blue at column 0
              {
            if (row == height - 1)  // blue at position (height-1, 0)
                {
              outR.at<uchar>(row, col) = src.at<uchar>(row - 1, col + 1);
              outG.at<uchar>(row, col) = (src.at<uchar>(row - 1, col)
                  + src.at<uchar>(row, col + 1)) / 2;
            } else {
              outR.at<uchar>(row, col) = (src.at<uchar>(row - 1, col + 1)
                  + src.at<uchar>(row + 1, col + 1)) / 2;
              outG.at<uchar>(row, col) = (src.at<uchar>(row - 1, col)
                  + src.at<uchar>(row + 1, col) + src.at<uchar>(row, col + 1))
                  / 3;
            }
          } else    // blue at other columns not 0
          {
            if (row == height - 1) {
              outR.at<uchar>(row, col) = (src.at<uchar>(row - 1, col - 1)
                  + src.at<uchar>(row - 1, col + 1)) / 2;
              outG.at<uchar>(row, col) = (src.at<uchar>(row, col - 1)
                  + src.at<uchar>(row, col + 1) + src.at<uchar>(row - 1, col))
                  / 3;
            } else {
              outR.at<uchar>(row, col) = (src.at<uchar>(row - 1, col - 1)
                  + src.at<uchar>(row - 1, col + 1)
                  + src.at<uchar>(row + 1, col + 1)
                  + src.at<uchar>(row + 1, col - 1)) / 4;
              outG.at<uchar>(row, col) = (src.at<uchar>(row - 1, col)
                  + src.at<uchar>(row + 1, col) + src.at<uchar>(row, col - 1)
                  + src.at<uchar>(row, col + 1)) / 4;
            }
          }
          outB.at<uchar>(row, col) = src.at<uchar>(row, col);
        } else    // green positions
        {
          if (col == width - 1) // green at column width-1
              {
            if (row == height - 1)  // green at position (height-1, width-1)
              outR.at<uchar>(row, col) = src.at<uchar>(row - 1, col);
            else
              outR.at<uchar>(row, col) = (src.at<uchar>(row - 1, col)
                  + src.at<uchar>(row + 1, col)) / 2;
            outB.at<uchar>(row, col) = src.at<uchar>(row, col - 1);
          } else    // green at other columns not width-1
          {
            if (row == height - 1)
              outR.at<uchar>(row, col) = src.at<uchar>(row - 1, col);
            else
              outR.at<uchar>(row, col) = (src.at<uchar>(row - 1, col)
                  + src.at<uchar>(row + 1, col)) / 2;
            outB.at<uchar>(row, col) = (src.at<uchar>(row, col - 1)
                + src.at<uchar>(row, col + 1)) / 2;
          }
          outG.at<uchar>(row, col) = src.at<uchar>(row, col);
        }
      }
    }
  vector<Mat> array_to_merge;

  array_to_merge.push_back(outB);
  array_to_merge.push_back(outG);
  array_to_merge.push_back(outR);

  merge(array_to_merge, dst);
}

void ColorCorrection(Mat src, Mat & dst) {
  int rows = src.rows;
  int cols = src.cols;
  Mat correctionKernel =
      (Mat_<float>(3, 3) << -0.18, -0.44, 1.62, -0.24, 1.29, -0.05, 1.18, -0.05, -0.13);

  cout << "color correction matrix: " << correctionKernel << endl;
  Mat tempImg;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      Mat(src.at<Vec3b>(i, j)).convertTo(tempImg, CV_32FC3);
      dst.at<Vec3b>(i, j) = Mat(correctionKernel * tempImg);
    }
  }

}

void GammaCorrection(Mat src, Mat & dst, float fGamma) {
  //image pixel intensities in the range [0, 255] 
  uchar lut[256];
  for (int i = 0; i < 256; i++) {
    // The formula is O=(I/255)^N3C255, 
    // gotten from this link http://docs.opencv.org/trunk/d3/dc1/tutorial_basic_linear_transform.html
    lut[i] = saturate_cast<uchar>(pow((float) (i / 255.0), fGamma) * 255.0f);
  }
  dst = src.clone();
  MatIterator_<Vec3b> it, end;
  for (it = dst.begin<Vec3b>(), end = dst.end<Vec3b>(); it != end; it++) {
    (*it)[0] = lut[((*it)[0])];
    (*it)[1] = lut[((*it)[1])];
    (*it)[2] = lut[((*it)[2])];
  }
}
