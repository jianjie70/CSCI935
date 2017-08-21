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

void ColorInterpolation(Mat imgSrc, Mat& dst);
void ColorCorrection(Mat src, Mat& dst);
void GammaCorrection(Mat src, Mat& dst, float fGamma);
void Display4Images(Mat& im1, Mat& im2, Mat& im3, Mat& im4);

int main(int argc, char* argv[]) {
  //=========STAGE 1: LOAD IMAGE==============
  if(argc != 2) {
    cout << "colorChain" << endl;
    return -1;
  }
  Mat imgSrc = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED); 
  Mat originalImage = imread(argv[1], CV_LOAD_IMAGE_COLOR); 

  if(!imgSrc.data) {
    cout << "Failed to open image." << endl;
    return -1;
  }
  cout << "image source: " << imgSrc.size() << endl;
  //=========STAGE 2: BILINEAR COLOR INTERPOLATION==============
  //CV_8UC3 unsigned char types that are 8 bit long 
  //and each pixel has three of these to form the three channels
  Mat imgCFAInterpol(imgSrc.size(), CV_8UC3);
  imgCFAInterpol = Scalar(0, 0, 0);   //BGR order. R = 255;
  ColorInterpolation(imgSrc, imgCFAInterpol);
   
  //=========STAGE 3: COLOR CORRECTION==============
  Mat imgColorCorrected(imgSrc.size(), CV_8UC3); //only accept value from 0 to 255
  ColorCorrection(imgCFAInterpol, imgColorCorrected);

  //=========STAGE 4: GAMMA CORRECTION==============
  Mat imgGammaCorrected(imgSrc.size(), CV_8UC3); //only accept value from 0 to 255
  GammaCorrection(imgColorCorrected, imgGammaCorrected, 0.5);

  //=========DISPLAY RESULTS==============
  Display4Images(originalImage, imgCFAInterpol, imgColorCorrected, imgGammaCorrected);

  waitKey(0);
  return 0;
}

void Display4Images(Mat& im1, Mat& im2, Mat& im3, Mat& im4){
  Size size = im1.size();
  Mat dst(Size(size.width * 2, size.height * 2), CV_8UC3);
  Mat roi1 = dst(Rect(0 , 0, size.width, size.height));
  im1.copyTo(roi1);
  Mat roi2 = dst(Rect(size.width , 0, size.width, size.height));
  im2.copyTo(roi2);
  Mat roi3 = dst(Rect(0 , size.height, size.width, size.height));
  im3.copyTo(roi3);
  Mat roi4 = dst(Rect(size.width , size.height, size.width, size.height));
  im4.copyTo(roi4);

  Mat final;
  resize(dst,final,size);//resize image
  // Display big mat
  namedWindow("Color Chain", CV_WINDOW_AUTOSIZE);
  imshow("Color Chain", final);
}

void ColorInterpolation(Mat src, Mat& dst){
  int m, n;
  Mat inRGB(src.size(), CV_8UC1);
  Mat outR(src.size(), CV_8UC1), outG(src.size(), CV_8UC1), outB(src.size(), CV_8UC1);
  m = src.rows;
  n = src.cols;
  
  inRGB = src;
  //R channel for Green Pixel in horizontal
  for(int j = 1; j <= m-1; j += 2) {
    for(int i = 3; i <= n-1; i += 2) {
      outR.at<uchar>(j, i) = .5 * (inRGB.at<uchar>(j, i-1) + inRGB.at<uchar>(j, i+1));
    }
  }
  // R channel for Green Pixel in vertical 
  for(int j = 2; j <= m-2; j += 2) {
    for(int i = 2; i <= n; i += 2) {
      outR.at<uchar>(j, i) = .5 * (inRGB.at<uchar>(j-1, i) + inRGB.at<uchar>(j+1, i));
    }
    // R channel for Blue Pixel 
    for(int i = 4; i <= n; i += 2) {
      outR.at<uchar>(j, i-1) = .25 * (inRGB.at<uchar>(j-1, i-2) + inRGB.at<uchar>(j-1, i) + inRGB.at<uchar>(j+1, i-2) + inRGB.at<uchar>(j+1, i));
    }
  }

  //Bounderies
  //R channel for Blue pixels at the left boundery 
  for(int j = 2; j <= m-2; j += 2) {
    outR.at<uchar>(j, 1) = 0.5 * (inRGB.at<uchar>(j-1, 2) + inRGB.at<uchar>(j+1, 2));
  }

  //R channel for the bottom left B pixel of the left boundery
  outR.at<uchar>(m, 1) = inRGB.at<uchar>(m-1, 2);
  
  //R channel for Blue pixels at the bottom boundery 
  for(int i = 3; i <= n-2; i += 2) {
    outR.at<uchar>(m, i) = 0.5 * (inRGB.at<uchar>(m-1, i-1) + inRGB.at<uchar>(m - 1, i+1));
  }

  //R channel for Green pixels at the left boundery 
  for(int j = 1; j <= m-2; j += 2) {
    outR.at<uchar>(j, 1) = inRGB.at<uchar>(j, 2);
  }

  //R channel for Green pixels at the bottom boundery 
  for(int i = 2; i <= n-2; i += 2) {
    outR.at<uchar>(m, i) = inRGB.at<uchar>(m -1, i);
  }


  namedWindow("outR", CV_WINDOW_AUTOSIZE);
  imshow("outR", outR);

  // B channel for Green pixel in horizontal
  for(int j = 2; j <= m; j += 2) {
    for(int i = 2; i <= n-2; i += 2) {
      outB.at<uchar>(j, i) = .5 * (inRGB.at<uchar>(j, i-1) + inRGB.at<uchar>(j, i+1));
    }
  }

  // B channel for Green Pixel in vertical  
  for(int j = 3; j <= m-1; j += 2) {
    for(int i = 1; i <= n-1; i += 2) {
      outB.at<uchar>(j, i) = .5 * (inRGB.at<uchar>(j-1, i) + inRGB.at<uchar>(j+1, i));
      
    }
    // B channel for Red Pixel 
    for(int i = 3; i <= n-1; i += 2) {
      outB.at<uchar>(j, i-1) = .25 * (inRGB.at<uchar>(j-1, i-2) + inRGB.at<uchar>(j-1, i) + inRGB.at<uchar>(j+1, i-2) + inRGB.at<uchar>(j+1, i));
    }
  }
  
  //Bounderies
  //B channel for R pixels at the top boundery 
  for(int i = 2; i <= n-2; i += 2) {
    outB.at<uchar>(1, i) = 0.5 * (inRGB.at<uchar>(2, i-1) + inRGB.at<uchar>(2, i+1));
  }
  //B channel for the last R pixel of top right corner
  outB.at<uchar>(1, n) = inRGB.at<uchar>(2, n-1);

  //B channel for R pixel of the right boundery
  for(int j = 3; j <= m-2; j += 2) {
    outB.at<uchar>(j, n) = 0.5 * (inRGB.at<uchar>(j-1, n-1) + inRGB.at<uchar>(j+1, n-1));
  }

  //B channel for G pixel of the top boundery
  for(int i = 1; i <= n-2; i += 2) {
    outB.at<uchar>(1, i) = inRGB.at<uchar>(2, i);
  }
  // B channel for G pixels of the right boundery
  for(int j = 2; j <= m-2; j += 2) {
    outB.at<uchar>(j, n) = inRGB.at<uchar>(j, n-1);
  }

  namedWindow("outB", CV_WINDOW_AUTOSIZE);
  imshow("outB", outB);

  // G channel for R pixel
  for(int j = 2; j <= m-2; j += 2) {
    for(int i = 3; i <= n-1; i += 2) {
      outG.at<uchar>(j, i) = .25 * (inRGB.at<uchar>(j, i-1) + inRGB.at<uchar>(j, i+1) + inRGB.at<uchar>(j-1, i) + inRGB.at<uchar>(j+1, i));
    }
  }
  // G channel for Blue pixel
  for(int j = 3; j <= m-1; j += 2) {
    for(int i = 2; i <= n-2; i += 2) {
      outG.at<uchar>(j, i) = .25 * (inRGB.at<uchar>(j, i-1) + inRGB.at<uchar>(j, i+1) + inRGB.at<uchar>(j-1, i) + inRGB.at<uchar>(j+1, i+1));
    }
  }

  // Bounderies of G channel
  // G channel for R pixels on top
  for(int i = 2; i <= n-2; i += 2) {
    outG.at<uchar>(1, i) = (1/3) * (inRGB.at<uchar>(1, i-1) + inRGB.at<uchar>(3, i+1) + inRGB.at<uchar>(2, i));
  }
  // G channel for the last R pixel of top right corner
  outG.at<uchar>(1, n) = .5 * (inRGB.at<uchar>(1, n-1) + inRGB.at<uchar>(2, n));
  
  //G channel for R pixels of the right boundery
  for(int j = 3; j <= m-1; j += 2) {
    outG.at<uchar>(j, n) = (1/3) * (inRGB.at<uchar>(j-1, n) + inRGB.at<uchar>(j+1, n) + inRGB.at<uchar>(j, n-1));
  }

  //G channel for B pixels of the left boundery
  for(int j = 2; j <= m-2; j += 2) {
    outG.at<uchar>(j, 1) = (1/3) * (inRGB.at<uchar>(j-1, 1) + inRGB.at<uchar>(j+1, 1) + inRGB.at<uchar>(j, 2));
  }
  // G channel for the bottom left B pixel of the left boundery
  outG.at<uchar>(m, 1) = .5 * (inRGB.at<uchar>(m-1, 1) + inRGB.at<uchar>(m, 2));
  
  // G channel for B pixels of the bottom boundery
  for(int i = 3; i <= n-1; i += 2) {
    outG.at<uchar>(m, i) = (1/3) * (inRGB.at<uchar>(m, i-1) + inRGB.at<uchar>(m, i+1) + inRGB.at<uchar>(m-1, i));
  }

  for(int i = 0; i < m; ++i) {
    for(int j = 0; j < n; ++j) {
      dst.at<Vec3b>(i, j)[0] = outB.at<uchar>(i, j); //B
      dst.at<Vec3b>(i, j)[1] = outG.at<uchar>(i, j); //G
      dst.at<Vec3b>(i, j)[2] = outR.at<uchar>(i, j); //R
    }
  }

  namedWindow("outG", CV_WINDOW_AUTOSIZE);
  imshow("outG", outG);  
}

void ColorCorrection(Mat src, Mat& dst){
  int rows = src.rows;
  int cols = src.cols;
  //Mat correctionKernel = (Mat_<float>(3,3) << 1.18, -0.05, -0.13, -0.24, 1.29, -0.05, -0.18, -0.44, 1.62);
  Mat correctionKernel = (Mat_<float>(3,3) << -0.18, -0.44, 1.62, -0.24, 1.29, -0.05, 1.18, -0.05, -0.13);

  cout << "color correction matrix: " << correctionKernel << endl;
  Mat tempImg;
  for(int i = 0; i < rows; ++i) {
    for(int j = 0; j < cols; ++j) {
      Mat(src.at<Vec3b>(i, j)).convertTo(tempImg, CV_32FC3);
      dst.at<Vec3b>(i, j) = Mat(correctionKernel * tempImg);
    }
  }

}

void GammaCorrection(Mat src, Mat& dst, float fGamma){
  //image pixel intensities in the range [0, 255] 
  uchar lut[256]; 
  for (int i = 0; i < 256; i++){
    // The formula is O=(I/255)^γ×255, 
    // gotten from this link http://docs.opencv.org/trunk/d3/dc1/tutorial_basic_linear_transform.html
    lut[i] = saturate_cast<uchar>(pow((float)(i / 255.0), fGamma) * 255.0f);
  }
  dst = src.clone();
  MatIterator_<Vec3b> it, end;
  for (it = dst.begin<Vec3b>(), end = dst.end<Vec3b>(); it != end; it++){
    (*it)[0] = lut[((*it)[0])];
    (*it)[1] = lut[((*it)[1])];
    (*it)[2] = lut[((*it)[2])];
  }
}