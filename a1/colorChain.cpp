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
  Size s = src.size();
  
  //Add extra 1 pixel around 4 bounderies to handle boundary poblem. 
  Mat imgSrc = Mat::zeros(Size(s.width + 2, s.height + 2), src.type());
  Mat roi = imgSrc(Rect(1 , 1, s.width, s.height));
  src.copyTo(roi);
  cout << "image source: " << imgSrc.size() << endl;

  int rows, cols;
  //Mat inR(imgSrc.size(), CV_8UC1), inG(imgSrc.size(), CV_8UC1), inB(imgSrc.size(), CV_8UC1);
  Mat outR(imgSrc.size(), CV_8UC1), outG(imgSrc.size(), CV_8UC1), outB(imgSrc.size(), CV_8UC1);
  rows = imgSrc.rows;
  cols = imgSrc.cols;

  // R channel for Green Pixel in horizontal
  for(int r = 2; r <= rows-1; r += 2) {
    for(int c = 2; c <= cols-1; c += 2) {
      outR.at<uchar>(r, c) = .5 * (imgSrc.at<uchar>(r, c-1) + imgSrc.at<uchar>(r, c+1));
    }
  }

   // R channel for Green Pixel in vertical 
  for(int r = 3; r <= rows-2; r += 2) {
    for(int c = 3; c <= cols; c += 2) {
      outR.at<uchar>(r, c) = .5 * (imgSrc.at<uchar>(r-1, c) + imgSrc.at<uchar>(r+1, c));
  
      // R channel for Blue Pixel  
      outR.at<uchar>(r, c-1) = .25 * (imgSrc.at<uchar>(r-1, c-2) + imgSrc.at<uchar>(r-1, c) + imgSrc.at<uchar>(r+1, c-2) + imgSrc.at<uchar>(r+1, c));
    }
  }
  //Display image
  namedWindow( "outR", WINDOW_AUTOSIZE );
  //imshow( "outR", outR ); 

  // B channel for Green pixel in horizontal
  for(int r = 2; r <= rows-1; r += 2) {
    for(int c = 2; c <= cols-1; c += 2) {
      outB.at<uchar>(r, c) = .5 * (imgSrc.at<uchar>(r-1, c) + imgSrc.at<uchar>(r + 1, c));
    }
  } 
   // B channel for Green Pixel in vertical 
  for(int r = 3; r <= rows-2; r += 2) {
    for(int c = 3; c <= cols; c += 2) {
      outB.at<uchar>(r, c) = .5 * (imgSrc.at<uchar>(r-1, c) + imgSrc.at<uchar>(r+1, c));
      
      // B channel for Red Pixel 
      outB.at<uchar>(r-1, c) = .25 * (imgSrc.at<uchar>(r-2, c-1) + imgSrc.at<uchar>(r, c-1) + imgSrc.at<uchar>(r-2, c+1) + imgSrc.at<uchar>(r, c + 1));
    }
  }
  //Display image
  namedWindow( "outB", WINDOW_AUTOSIZE );// Create a window for display.
  imshow( "outB", outB ); 

  // G channel for R pixel
  for(int r = 2; r <= rows-2; r += 2) {
    for(int c = 3; c <= cols-1; c += 2) {
      outG.at<uchar>(r, c) = .25 * (imgSrc.at<uchar>(r, c-1) + imgSrc.at<uchar>(r, c+1) + imgSrc.at<uchar>(r-1, c) + imgSrc.at<uchar>(r+1, c));
    }
  }

  // G channel for Blue pixel
  for(int r = 3; r <= rows-1; r += 2) {
    for(int c = 2; c <= cols-2; c += 2) {
      outG.at<uchar>(r, c) = .25 * (imgSrc.at<uchar>(r, c-1) + imgSrc.at<uchar>(r, c+1) + imgSrc.at<uchar>(r-1, c) + imgSrc.at<uchar>(r+1, c));
    }
  }
  //Display image
  namedWindow( "outG", WINDOW_AUTOSIZE );// Create a window for display.
  //imshow( "outG", outG ); 

  Mat combineImage(imgSrc.size(), CV_8UC3);
    //Combine 3 RGB channels
  for(int i = 0; i < rows; ++i) {
    for(int j = 0; j < cols; ++j) {
      combineImage.at<Vec3b>(i, j)[0] = outB.at<uchar>(i, j); //B
      combineImage.at<Vec3b>(i, j)[1] = outG.at<uchar>(i, j); //G
      combineImage.at<Vec3b>(i, j)[2] = outR.at<uchar>(i, j); //R
    }
  }
  // remove extra boundaries
  // Setup a rectangle to define your region of interest
  Rect myROI(1, 1, src.cols, src.rows);
  // Crop the full image to that image contained by the rectangle myROI
  // Note that this doesn't copy the data
  dst = combineImage(myROI);
  cout << "image result: " << combineImage.size << endl;
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
    // The formular is O=(I/255)^γ×255, 
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