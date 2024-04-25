#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <thread>

using namespace std;
using namespace cv;
using namespace raspicam;

Mat frame, Matrix, framePers, frameGray, frameThresh, frameEdge, frameFinal, frameFinalDuplicate;
Mat ROILane;
int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result, serialPort;

RaspiCam_Cv Camera;

stringstream ss;


vector<int> histrogramLane;

Point2f Source[] = {Point2f(20,160), Point2f(367,160), Point2f(0,200), Point2f(387,200)}; 
Point2f Destination[] = {Point2f(100,0),Point2f(280,0),Point2f(100,240), Point2f(280,240)};


void SetupSlaveDevice(){
    const char *device = "/dev/ttyUSB0";
    serialPort = open(device, O_RDWR);

    if (serialPort == -1) {
        std::cerr << "Unable to open serial port." << std::endl;
        
    }
    struct termios options;
    tcgetattr(serialPort, &options);
    cfsetispeed(&options, B9600);  // Set input baud rate
    cfsetospeed(&options, B9600);  // Set output baud rate
    tcsetattr(serialPort, TCSANOW, &options);

}

 void Setup ( int argc,char **argv, RaspiCam_Cv &Camera )
  {
    Camera.set ( CAP_PROP_FRAME_WIDTH,  ( "-w",argc,argv,400 ) );
    Camera.set ( CAP_PROP_FRAME_HEIGHT,  ( "-h",argc,argv,240 ) );
    Camera.set ( CAP_PROP_BRIGHTNESS, ( "-br",argc,argv,50 ) );
    Camera.set ( CAP_PROP_CONTRAST ,( "-co",argc,argv,50 ) );
    Camera.set ( CAP_PROP_SATURATION,  ( "-sa",argc,argv,50 ) );
    Camera.set ( CAP_PROP_GAIN,  ( "-g",argc,argv ,50 ) );
    Camera.set ( CAP_PROP_FPS,  ( "-fps",argc,argv,0));

}

void Capture()
{
	Camera.grab();
    Camera.retrieve( frame);
    cvtColor(frame, frame, COLOR_BGR2RGB);
}

//Birds Eye View
void Perspective()
{
	line(frame,Source[0], Source[1], Scalar(0,0,255), 2);
	line(frame,Source[1], Source[3], Scalar(0,0,255), 2);
	line(frame,Source[3], Source[2], Scalar(0,0,255), 2);
	line(frame,Source[2], Source[0], Scalar(0,0,255), 2);
	
	
	Matrix = getPerspectiveTransform(Source, Destination);
	warpPerspective(frame, framePers, Matrix, Size(400,240));
}

void Threshold()
{
	cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
	inRange(frameGray, 200, 255, frameThresh);
	Canny(frameGray,frameEdge, 800, 480, 3, false);
	add(frameThresh, frameEdge, frameFinal);
	cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
	cvtColor(frameFinal, frameFinalDuplicate, COLOR_RGB2BGR);   //used in histrogram function only
	
}

void Histrogram()
{
    histrogramLane.resize(400);
    histrogramLane.clear();
    
    for(int i=0; i<400; i++)       //frame.size().width = 400
    {
	ROILane = frameFinalDuplicate(Rect(i,140,1,100));
	divide(255, ROILane, ROILane);
	histrogramLane.push_back((int)(sum(ROILane)[0])); 
    }
}

//finding lane lines 
void LaneFinder()
{
    vector<int>:: iterator LeftPtr;
    LeftPtr = max_element(histrogramLane.begin(), histrogramLane.begin() + 150);
    LeftLanePos = distance(histrogramLane.begin(), LeftPtr); 
    
    vector<int>:: iterator RightPtr;
    RightPtr = max_element(histrogramLane.begin() +250, histrogramLane.end());
    RightLanePos = distance(histrogramLane.begin(), RightPtr);
    
    line(frameFinal, Point2f(LeftLanePos, 0), Point2f(LeftLanePos, 240), Scalar(0, 255,0), 2);
    line(frameFinal, Point2f(RightLanePos, 0), Point2f(RightLanePos, 240), Scalar(0,255,0), 2); 
}

//calculating lane center and offset
void LaneCenter()
{
    laneCenter = (RightLanePos-LeftLanePos)/2 +LeftLanePos;
    frameCenter = 188;
    
    line(frameFinal, Point2f(laneCenter,0), Point2f(laneCenter,240), Scalar(0,255,0), 3);
    line(frameFinal, Point2f(frameCenter,0), Point2f(frameCenter,240), Scalar(255,0,0), 3);

    Result = laneCenter-frameCenter;
}

void detectObjects(const Mat& inputFrame, CascadeClassifier& cascade, vector<Rect>& detectedObjects)
{
    cascade.detectMultiScale(inputFrame, detectedObjects, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
}

int main(int argc, char** argv)
{
    CascadeClassifier trafficSign_cascade;
    CascadeClassifier trafficLight_cascade;
    String trafficSign_cascade_path = "<path for traffic sign cascade classifier >";
    String trafficLight_cascade_path = "<path for traffic light cascade classifier>";
    if (!trafficSign_cascade.load(trafficSign_cascade_path) || !trafficLight_cascade.load(trafficLight_cascade_path))
    {
        cerr << "Error loading Haar Cascade file!" << endl;
        return -1;
    }

    vector<Rect> trafficSign;
    vector<Rect> trafficLight;

    SetupSlaveDevice();
    Setup(argc, argv, Camera);
    cout << "Connecting to camera" << endl;
    if (!Camera.open())
    {
        cout << "Failed to Connect" << endl;
        return -1;
    }

    cout << "Camera Id = " << Camera.getId() << endl;

    // Create threads for object detection
    thread detectSignThread(detectObjects, std::ref(frame), std::ref(trafficSign_cascade), std::ref(trafficSign));
    thread detectLightThread(detectObjects, std::ref(frame), std::ref(trafficLight_cascade), std::ref(trafficLight));

    while (1)
    {
        auto start = std::chrono::system_clock::now();

        Capture();
        Perspective();
        Threshold();
        Histrogram();
        LaneFinder();
        LaneCenter();
        // Wait for the detection threads to finish
        detectSignThread.join();
        detectLightThread.join();

        // Draw rectangles around detected signs
        for (size_t i = 0; i < trafficSign.size(); i++)
        {
            rectangle(frame, trafficSign[i].tl(), trafficSign[i].br(), Scalar(255, 0, 0), 2);
        }
        // Draw rectangles around detected lights
        for (size_t i = 0; i < trafficLight.size(); i++)
        {
            rectangle(frame, trafficLight[i].tl(), trafficLight[i].br(), Scalar(0, 255, 0), 2);
        }
        
        // Send data to slave device (Arduino)
        
        std::string strData = std::to_string(Result) + "\n";
        write(serialPort, strData.c_str(), strData.length());

        ss.str(" ");
        ss.clear();
        ss << "Result = " << Result;
        putText(frame, ss.str(), Point2f(1, 50), 0, 1, Scalar(0, 0, 255), 2);

        namedWindow("orignal", WINDOW_KEEPRATIO);
        imshow("orignal", frame);

        namedWindow("Perspective", WINDOW_KEEPRATIO);
        imshow("Perspective", framePers);

        namedWindow("Final", WINDOW_KEEPRATIO);
        imshow("Final", frameFinal);

        

        // Restart the detection threads
        detectSignThread = thread(detectObjects, std::ref(frame), std::ref(trafficSign_cascade), std::ref(trafficSign));
        detectLightThread = thread(detectObjects, std::ref(frame), std::ref(trafficLight_cascade), std::ref(trafficLight));

        waitKey(1);

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        float t = elapsed_seconds.count();
        int FPS = 1 / t;
        cout << "FPS = " << FPS << endl;
    }

    return 0;
}
