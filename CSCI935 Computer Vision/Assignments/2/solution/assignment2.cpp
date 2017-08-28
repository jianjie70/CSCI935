/*------------------------------------------------------
 Student's Name: Khai Quoc Le
 Student's number: 5096182
 Student's email address: kql832@uowmail.edu.au
 -------------------------------------------------------*/

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    
    Mat srcImage, imgRescale, imgGradient, imgBinarize, imgErode, imgMorphology, kernel, sElement;
    vector<Vec2f> lines1, lines2;
    
    sElement = getStructuringElement(MORPH_RECT, Size(3, 3));
    
    //-- load image and resize by a factor of 2
    srcImage = imread("asm2.bmp", IMREAD_GRAYSCALE );
    resize(srcImage, imgRescale, Size(), 0.5, 0.5, CV_INTER_AREA);
    
    //-- apply LPF to the rescaled image
    GaussianBlur(imgRescale, imgRescale, Size(3,3), 0, 0, BORDER_DEFAULT);
    
    //-- gradient-based boundary detection method and Hough transform
    Mat gradient, gradientX, gradientY;

    Sobel(imgRescale, gradientX, CV_32F, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    Sobel(imgRescale, gradientY, CV_32F, 0, 1, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(gradientX, gradientX);
    convertScaleAbs(gradientY, gradientY);
    
    imgGradient = gradientX + gradientY;
    imwrite("gradient.bmp", imgGradient);

    erode(imgGradient, imgGradient, sElement);
    threshold(imgGradient, imgGradient, 100, 255, THRESH_BINARY);
    HoughLines(imgGradient, lines1, 1, CV_PI/60, 90);
    
    printf("Line detection in a b/w image produced by the gradient operator:\n");
    if(!lines1.empty()){
        for(int i = 0; i < lines1.size(); i++)
        {
            float degree = (lines1[i][1]*180)/CV_PI;
            printf("\tLine %d is detected. Orientation = %.1f degrees\n", i+1, degree);
        }
    }
    else{
        printf("No straight lines detected\n");
    }

    //-- morphological boundary detection method and Hough transform
    threshold(imgRescale, imgBinarize, 100, 255, THRESH_BINARY_INV);
    
    morphologyEx(imgBinarize, imgMorphology, MORPH_GRADIENT, sElement);
    imwrite("morphology.bmp", imgMorphology);
    
    HoughLines(imgMorphology, lines2, 1, CV_PI/60, 90);
    
    printf("Line detection in a b/w image produced by the binary morphology:\n");
    if(!lines2.empty()){
        for(int i = 0; i < lines2.size(); i++)
        {
            float degree = (lines2[i][1]*180)/CV_PI;
            printf("\tLine %d is detected. Orientation = %.1f degrees\n", i+1, degree);
        }
    }
    else{
        printf("No straight lines detected\n");
    }
    
    waitKey();
    
    return 0;
}