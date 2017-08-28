//============================================================================
// Name        : opencv_workshop.cpp
// Author      : Philip Ogunbona
// Version     :0.9
// Copyright   : Copyright 2016, Philip Ogunbona. This program is free software:
// you can redistribute it and/or modify it under the terms of the GNU General
// Public License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
// This program is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
// Description : C++, Ansi-style
// Compilation and linking: g++ for compilation and ensure that your
// compiler can see the header files.
// Ensure that you link against the following libraries:
// opencv_core, opencv_imgcodecs, opencv_highgui, opencv_imgproc,
//
//============================================================================

#include <iostream>
#include <cstdlib>
#include <core.hpp>
#include <highgui.hpp>
#include <imgproc.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
	cout << "OpenCV workshop - computer vision examples" << endl; // prints OpenCV workshop - computer vision examples

	Mat srcImage, dstImage;
	vector<Mat> pyramid;

	//const String keys =
	//		"{help h usage ?  |       |  print this message}"
	//		"{@image1         |       |  image1 for compare }"
	//		"{@image2         |       |  image2 for compare }"
	//		"{@repeat         |1      |  number }"
	//		"{path            |.      |  path to file }"
	//		"{fps             | -1.0  |  fps for output video }"
	//		"{N count         |100    |  count of objects  }"
	//		"{ts timestamp    |       |   use time stamp }"
	//;
		const String keys =
			"{help h usage ?   |       |  print this message}"
			"{@image1         |       |  image1 - input }"
			"{@image2         |       |  image2 - output }"
	;


	/*
	 * Process the command line arguments
	 */
		CommandLineParser parser(argc, argv, keys);
		if (parser.has("help")|| parser.has("?")||parser.has("h")){
			parser.about("OpenCV workshop - tests");
			parser.printMessage();
			exit(EXIT_FAILURE);
		}

		String filename = parser.get<String>(0); // get first positional parameter
		srcImage = imread(filename);//, IMREAD_GRAYSCALE);

		String outfile = parser.get<String>(1); // get second positional parameter

		/*vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);

		imwrite(outfile, dstImage, compression_params);// write the output


		imshow("Original", srcImage);
		waitKey();*/



	/*
	 * Simple Gaussian filtering
	 * GaussianBlur(InputArray src, OutputArray dst, Size ksize, double sigmaX, double sigmaY=0,
	int borderType=BORDER_DEFAULT )
	 */

/*
		GaussianBlur(srcImage, dstImage, cv::Size(11,11), 1.5, 1.5, BORDER_REFLECT );

	//	resize(dstImage, dstImage, Size(), 0.64, 0.64, INTER_CUBIC);

		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);

		imwrite(outfile, dstImage, compression_params);// write the output


	imshow("Original", srcImage);
	waitKey();

	imshow("Blurred Image", dstImage);
	waitKey();
*/

		/*
		 * Testing the effect of adding the median of an image to itself
		 */
/*
	imshow("Original", srcImage);
   	medianBlur(srcImage, dstImage, 3);
		//Laplacian(srcImage,dstImage,8,1,1,0,BORDER_DEFAULT);


		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);

		imwrite(outfile,dstImage, compression_params);// write the output

		imshow("Median image", dstImage);
		waitKey();

    	cv::addWeighted(srcImage, 0.6, dstImage, 0.4, 0.0, dstImage, -1);

		imshow("Median+ image", dstImage);
		waitKey();
*/
	/*
	 * buildPyramid
	Constructs the Gaussian pyramid for an image.
	void buildPyramid(InputArray src, OutputArrayOfArrays dst,   int maxlevel,  int border-Type=BORDER_DEFAULT )
	 *
	 */


		buildPyramid(srcImage, pyramid, 4, BORDER_DEFAULT);


		 imshow("Level 1 input image", pyramid[0]);
		imshow("Level 2", pyramid[1]);
		imshow("Level 3", pyramid[2]);
		imshow("Level 4", pyramid[3]);

		waitKey();


	return 0;
}
