/*------------------------------------------------------
 Student's Name: Khai Quoc Le
 Student's number: 5096182
 Student's email address: kql832@uowmail.edu.au
 -------------------------------------------------------*/

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    Mat originalFrame, fgFrame, filteredFrame, colorFrame, finalFrame, sElement, labelFrame;
    Ptr<BackgroundSubtractorKNN> bKNN = createBackgroundSubtractorKNN();
    
    VideoCapture vcap(argv[1]);
    int width = vcap.get(CAP_PROP_FRAME_WIDTH);
    int height = vcap.get(CAP_PROP_FRAME_HEIGHT);
    double fps = 1000/vcap.get(CAP_PROP_FPS);
    
    //Create big frame to diplay four different types of frames
    finalFrame = Mat(Size(width+1, height+1), CV_8UC3, Scalar(255,255,255)); //add 1 for 1 pixel border
    
    // Vector of colors to fill in labelled objects
    vector<Vec3b> colors(90);
    colors[0] = Vec3b(0, 0, 0);//background with black
    for(int i = 1; i < 50; i++){
        colors[i] = Vec3b((rand()&255), (rand()&255), (rand()&255));
    }
    
    while(vcap.read(originalFrame)){
        // Resize the original fram by a factor of 2
        resize(originalFrame, originalFrame, Size(), 0.5, 0.5, cv::INTER_AREA);
        // Copy the original frame into a big final displaying frame
        originalFrame.copyTo(finalFrame(Rect(0, 0, width/2, height/2)));
        
        // Update the background model
        bKNN->apply(originalFrame, fgFrame);
        bKNN->setDetectShadows(false); //No shawdows detected
        
        // Apply opening morphological operatior to reduce some noise
        sElement = getStructuringElement(MORPH_RECT, Size(3, 1), Point(0,0));
        morphologyEx(fgFrame, filteredFrame, MORPH_OPEN, sElement);
        
        // Copy the foreground frame into a big final displaying frame
        cvtColor(fgFrame,fgFrame,CV_GRAY2BGR);
        fgFrame.copyTo(finalFrame(Rect(width/2+1, 0, width/2, height/2)));
        
        // Apply connected component anlysis to reduce noise, with connected component has area less than 25 is consider noise
        vector<vector<Point>> objContours;
        vector<Vec4i> h;
        int thres = 25; // threashold specifying minimum area of a blob
        
        // Find all contours in the binary image
        findContours(filteredFrame.clone(), objContours, h, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        
        //Fill-in all small blobs with zeros whose area is less than threshold
        if (!objContours.empty()){
            for (int i=0; i<objContours.size(); ++i){
                if (contourArea(objContours[i]) < thres)
                    drawContours(filteredFrame, objContours, i, Scalar(0), CV_FILLED, 0);
            }
        }

        // Label connected components which belong to an object
        connectedComponents(filteredFrame, labelFrame, 8);
        
        // Copy the filtered frame into a big final displaying frame
        cvtColor(filteredFrame,filteredFrame,CV_GRAY2BGR);
        filteredFrame.copyTo(finalFrame(Rect(0, height/2+1, width/2, height/2)));
        
        colorFrame = filteredFrame.clone();

        // Fill-in all labelled objects with different colors
        for(int row = 0; row < colorFrame.rows; row++){
            for(int col = 0; col < colorFrame.cols; col++){
                Vec3b &pixel = colorFrame.at<Vec3b>(row, col);
                pixel = colors[labelFrame.at<int>(row, col)];
            }
        }
        
        // Copy the colored frame into a big final displaying frame
        colorFrame.copyTo(finalFrame(Rect(width/2+1, height/2+1, width/2, height/2)));
        
        //show the result frame
        imshow("Moving Objects Detection", finalFrame);

        waitKey(fps);
    }
    
    vcap.release();

    return 0;
}
