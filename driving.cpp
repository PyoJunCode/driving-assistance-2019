#include "cv.hpp"
#include <iostream>
#include <opencv2/dnn.hpp>
#include <fstream>


using namespace cv;
using namespace std;
using namespace dnn;

void drawLine(vector<Vec2f> lines, float x, float y, float angle_th1, float angle_th2, Mat &result);
void drawRine(vector<Vec2f> lines, float x, float y, float angle_th1, float angle_th2, Mat &result);
string replaceAll(const string &str, const string &pattern, const string &replace);
int lPoint, rPoint, midPoint = 0;


int main() {
 
 String modelConfiguration = "deep/yolov2.cfg";
 String modelBinary = "deep/yolov2.weights";
 bool dnn = false;
 
 Net net = readNetFromDarknet(modelConfiguration, modelBinary);
 
 vector<String> classNamesVec;
 ifstream classNamesFile("deep/coco.names");
 
 if (classNamesFile.is_open()) {
  string className = "";
  while (std::getline(classNamesFile, className)) classNamesVec.push_back(className);
 }
 
 Mat frame, edge_l, edge_r, frame_roi_l, frame_roi_r, frame_roi_mid, result, grayFrame;

 int count = 0;
 int StopStatus =0;
 int GoStatus = 0;
 
 

 bool Stopped = false;
 bool pedestrian = false;
 
 
 vector<Vec2f> lines_l, lines_r;
 vector<Rect> frontCar;
 vector<Rect> person;
 Rect temp;
 
 //----------------------------------video-----------------//
 
 VideoCapture cap("Line_2.mp4");
 
 
 //-------------------------------------------------------//
 
 
 if (!cap.isOpened()) {
  cout << "can't open video file" << endl;
  return 0;
 }
 while (1) {
  count ++;
  if(count % 30 == 0) dnn = true;
  cap >> frame;
  if (frame.empty()) break;
  

  Rect rect_l(250, 250, 100, 100);
  Rect rect_r(380, 250, 100, 100);
  Rect middle(Point(220,270), Point(500,450));
  
  frame_roi_l = frame(rect_l);
  frame_roi_r = frame(rect_r);
  frame_roi_mid = frame(middle);
  
  
  Canny(frame_roi_l, edge_l, 50, 150, 3);
  Canny(frame_roi_r, edge_r, 50, 150, 3);
  
  HoughLines(edge_l, lines_l, 1, CV_PI / 180, 25);
  HoughLines(edge_r, lines_r, 1, CV_PI / 180, 25);
  

  if (frame.channels() == 4) cvtColor(frame, frame, COLOR_BGRA2BGR);
  
  if(dnn){
        
  Mat inputBlob = blobFromImage(frame_roi_mid, 1 / 255.F, Size(416, 416), Scalar(), true, false);
     net.setInput(inputBlob, "data");
     Mat detectionMat = net.forward("detection_out");
     
     float confidenceThreshold = 0.10;
   
   for (int i = 0; i < detectionMat.rows; i++) {
            const int probability_index = 5;
            const int probability_size = detectionMat.cols - probability_index;
            float* prob_array_ptr = &detectionMat.at<float>(i, probability_index);
            size_t objectClass = max_element(prob_array_ptr, prob_array_ptr + probability_size) - prob_array_ptr;
            
            float confidence = detectionMat.at<float>(i, (int)objectClass + probability_index);
            
            if (confidence > confidenceThreshold) {
                float x_center = detectionMat.at<float>(i, 0) * frame_roi_mid.cols;
                float y_center = detectionMat.at<float>(i, 1) * frame_roi_mid.rows;
                float width = detectionMat.at<float>(i, 2) * frame_roi_mid.cols;
                float height = detectionMat.at<float>(i, 3) * frame_roi_mid.rows;
                Point p1(cvRound(x_center - width / 2), cvRound(y_center - height / 2));
                Point p2(cvRound(x_center + width / 2), cvRound(y_center + height / 2));
                Rect object(p1, p2);
             String className = objectClass < classNamesVec.size() ? classNamesVec[objectClass] : cv::format("unknown(%d)", objectClass);
             String name = replaceAll(className, "\r", "");
       
             
             if((name.compare("parking meter") ==0 || (name.compare("car") ==0 ))&& object.area() > 4000){
              

              
              frontCar.push_back(object);
               
                temp = object;
             }
             
           
             
           

             
             if(name.compare("person") == 0 && object.area() > 3000){
              pedestrian = true;
             

             }
             else{
              pedestrian = false;
             
             }
             
              
            }
        }
   dnn = false;
  }
  
 //------------------------Update Car Info-------------------------------//
  if(frontCar.size() >0){ //check Status
    
   if(frontCar.size() > 2){
   if(abs(frontCar.back().area() - frontCar.at((int)frontCar.size()-2).area()) < frontCar.back().area()*0.1){
    StopStatus++;
    if(StopStatus > 10){
     Stopped = true;
     GoStatus = 0;
    }
   }
   }
   
   
   if(Stopped){
    putText(frame, "Status: Stopped", Point(550,350), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0),3);
    
    if(frontCar.back().area() - frontCar.at((int)frontCar.size()-2).area() < frontCar.back().area()* -0.2){
     cout<< "sub: " << frontCar.back().area() - frontCar.at((int)frontCar.size()-2).area() << endl;
      GoStatus++;
     putText(frame, "Warning!: Front car departure", Point(50,100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0),3);
        
    }
    
    if(GoStatus > 80){
     Stopped = false;
     StopStatus = 0;
    }
   }
  
  if(frontCar.back().area() > 5000){
   putText(frame, "Warning!: Collison with front car", Point(50,50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255),3);

   
  }
   
   

 }
  //-------------------------------------------------------//
  
 
  if(pedestrian){
    putText(frame, "Warning!: Collison with the pedestrian", Point(50,200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255),3);
  }
  
   drawLine(lines_l, 250, 250, 45, 60, frame); //return - + + -
   drawRine(lines_r, 380, 250, 115, 126, frame); //return - - + +
   
   midPoint = (lPoint + rPoint) /2;
   
  
   if((lines_l.size() < 0 || lines_r.size() < 0) || (midPoint < 350 && midPoint > 360)){
     putText(frame, "Warning!: Lane Departure ", Point(50,150), FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,255), 5);
   }
  
 
   
 
     
   

  
  
  imshow("Hough Transform", frame);

  
  waitKey(33);
 }
 
}

void drawLine(vector<Vec2f> lines, float x, float y, float angle_th1, float angle_th2, Mat &result){
 float rho, theta, a, b, x0, y0;
 float avr_rho=0. , avr_theta=0.;
 int count = 0;
 Point p1, p2;
 
 for (int i = 0; i < lines.size(); i++) {
  rho = lines[i][0];
  theta = lines[i][1];
  
 
  
  if (theta < CV_PI / 180 * angle_th1 || theta > CV_PI / 180 * angle_th2) continue;
  
  avr_rho += rho;
  avr_theta += theta;
  count++;
 }
 
 avr_rho /= count;
 avr_theta /= count;
 a = cos(avr_theta);
 b = sin(avr_theta);
 
 x0 = a * avr_rho;
 y0 = b * avr_rho;
 
 p1 = Point(cvRound(x0 + 1000 * (-b)) + x, cvRound(y0 + 1000 * a) + y);
 p2 = Point(cvRound(x0 - 1000 * (-b)) + x, cvRound(y0 - 1000 * a) + y);
 
 lPoint = (p1.x + p2.x) /2;

 line(result, p1, p2, Scalar(0, 255, 255), 3, 8);
}

void drawRine(vector<Vec2f> lines, float x, float y, float angle_th1, float angle_th2, Mat &result){
 float rho, theta, a, b, x0, y0;
 float avr_rho=0. , avr_theta=0.;
 int count = 0;
 Point p1, p2;
 
 for (int i = 0; i < lines.size(); i++) {
  rho = lines[i][0];
  theta = lines[i][1];
  
 
  
  if (theta < CV_PI / 180 * angle_th1 || theta > CV_PI / 180 * angle_th2) continue;
  
  avr_rho += rho;
  avr_theta += theta;
  count++;
 }
 
 avr_rho /= count;
 avr_theta /= count;
 a = cos(avr_theta);
 b = sin(avr_theta);
 
 x0 = a * avr_rho;
 y0 = b * avr_rho;
 
 p1 = Point(cvRound(x0 + 1000 * (-b)) + x, cvRound(y0 + 1000 * a) + y);
 p2 = Point(cvRound(x0 - 1000 * (-b)) + x, cvRound(y0 - 1000 * a) + y);
 
 rPoint = (p1.x + p2.x) /2;

 line(result, p1, p2, Scalar(0, 255, 255), 3, 8);
}

string replaceAll(const string &str, const string &pattern, const string &replace)
{
string result = str;
string::size_type pos = 0;
string::size_type offset = 0;
while((pos = result.find(pattern, offset)) != string::npos)
{
result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
offset = pos + replace.size();
}
return result;
}
