# Self Driving Car
## Abstract
Self-Driving Car, in general, is a hardware and software-based project which is specifically designed to revolutionize transportation and address various challenges. This project aims to create an autonomous system capable of navigating its surroundings without human intervention. To achieve this goal, real-time visual data from the Pi camera will be analyzed, and computer vision algorithms developed with OpenCV will be used.

The project consists of three key parts: hardware configuration, software creation, and algorithm execution. The Raspberry Pi will act as the main computing platform, offering the required processing speed and interfaces for integrating sensors. The Pi camera will serve as the main visual sensor, continuously taking pictures of the surroundings around the car. Various Computer vision techniques, including picture segmentation, object detection, and tracking, will be used to process and evaluate these images. The Arduino then uses a variety of signals produced by the Pi to trace and control the movement of the automobile in its surroundings.

The self-driving car's potential uses go beyond the purview of this project. It can be applied in numerous domains where autonomous navigation is required, such as transportation, industrial automation, surveillance, and many more.

## Methodology
### System Block Diagram
![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/693deea5-e1d7-446c-b511-e93035aa767c)
### Principle of Operation
Principle of lane detection
Lane detection is one of the core functions of computer vision and image processing. Finding and extracting the lane lines is the main target of this system. The steps involved in lane detection includes:

Step 1: Image Preprocessing 
	At first the input image was converted into gray scale. Converting the image to gray scale further simplifies processing by reducing color channels to a single intensity value per pixel.
The following formula can be used to determine the grayscale intensity (I) at a pixel (x, y) in an input color image having red (R), green (G), and blue (B) channels:
                           I (x,y) = 0.299 * R (x,y) + 0.587* G (x,y) +0.144*B (x,y)        (4.2.1)
The coefficients (0.299, 0.587, and 0.114) are weights that nearly match the luminance of each color channel as seen by the human eye. These weights are based on the luminosity function, which depicts the sensitivity of the human eye to different colors. This process converts (8*8*8) bit color representation into 8-bit gray color representation ranging from 0 to 255.

![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/2fb62b73-07ca-4b92-89bc-c317b182313e)
![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/5068eb87-ff93-4c44-9aeb-1ae2567f4a07)

Once image was normalized, the noise in the image was reduced by introducing Gaussian Blur to make subsequent edge detection more robust. It was achieved by applying a weighted average to the pixel values. This weighted average is calculated using a convolution operation between Gaussian kernel and an input image I (x, y).
![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/12a04a15-a4a7-4bb3-9a36-515a48ce4a72)

After that, edges were found using cunning edge detection. The methodology for identifying a sharp shift in intensity involves comparing each pixel to its neighbor.
![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/3c1373aa-58a1-43c8-bfe0-ca14c09ff133)

Step 2: Finding ROI and Masking
The ROI was defined to focus on the area where the lane markings are expected. This helped in ignoring irrelevant parts of the image, such as the sky and surrounding scenery.
Once ROI was defined, a binary mask was applied to the edge-detected image to zero out the pixels outside the defined ROI.
![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/3369d9dd-afa6-4e30-96b6-775eb52282f2)

Step 3: Perspective Transformation (Bird’s eye view)
By using perspective transformation, the ROI that was selected from the original image may be seen from above. This transformation requires creating a mapping between the points in the original image and their corresponding points in the converted image. The transformation is often shown as a matrix.

Step 4: Hough Transform
In image processing, the Hough Transform is a method for identifying shapes—mostly lines, but also other geometric patterns. In 1962, Paul Hough invented the transform, mainly for the purpose of locating lines in binary images. The Hough transform algorithm was used to draw lane lines when there are multiple breakpoints.  
Standard Hough Transform for Line detection includes:
	Parameterization: 
	Lines in polar coordinates are represented by the Hough Transform. It represents a line using the polar form, which is equivalent to xcos(θ)+ysin(θ), rather than the standard Cartesian coordinates (x, y).
	Accumulator Space:
	By generating an accumulator space, sometimes referred to as the Hough space, the Hough Transform works. This area is a grid, with each cell representing a potential line in the picture.
	r and θ are represented by the two grid dimensions. The r axis usually extends from negative to positive, and the θ axis typically lies between 0 and 180 degrees.
	Voting:
	The Hough Transform casts votes in the accumulator space for all potential lines that could pass through each edge pixel in the image.
	The accumulator grid's matching cells are increased to represent the vote. In the Hough space, every edge pixel adds to a sinusoidal curve.
	Finding Peaks:
	Once all edge pixels have voted, the peaks of the accumulator space are located. These peaks coincide with the image's line parameters (r, θ).
	With increasing peak height, there is a greater chance of a line with the matching parameters appearing in the image.
	Thresholding:
	A threshold is used to remove weak lines from the accumulator space. Only peaks that rise above a given threshold are regarded as detected lines.
	Conversion to Cartesian Coordinates:
	After that, the lines that have been identified in Hough space are converted back to Cartesian coordinates to yield the equations of the lines in the image.
 
![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/95102380-c5ab-4fd6-9d21-02610992ca69)

![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/7ea67a8b-2bd1-450d-b8d7-9ec7cde405c6)

Step 5: Drawing lanes on original image 
Once lane was detected an image was created with the detected lanes overlaying the original image.

![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/8afe7eb6-bc66-4801-8705-ec1c741df9e6)

Step 6: Decision Making
To make decisions regarding navigation of car, it is necessary to calculate middle points so that it could be taken as reference for navigation. It was calculated using mathematical expression:
                             (x_mid,y_mid) = ((x1 + x2)/2,(y1 + y2)/2)                              
After calculating middle points, the offset of car with respect to middle line was calculated so that the decision regarding moving it left or right could be made. It was calculated using:
                             offset = lane center-carcenter                              

 
![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/5276c67d-f419-45ff-bd46-8f9d091ca0d1)

![image](https://github.com/KingMaker960/Self-Driving-Car/assets/85979695/665c5b42-541a-4ce2-87d6-6035a3276988)
