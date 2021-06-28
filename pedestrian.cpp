
#include "cv.hpp"
#include <iostream>
#include <map>

using namespace cv;
using namespace std;

int findClosest(map<int, pair<vector<Rect>,int>> object, Rect curr);
float distance(Point prev, Point curr);

#if 1
int main(int argc, char *argv[]){
 
 VideoCapture cap("cv_live.mp4");
 
 float h =cap.get(CAP_PROP_FRAME_HEIGHT), w = cap.get(CAP_PROP_FRAME_WIDTH);
 float resize_factor = 3.0/4.0;
 Mat frame, m_backproj, hsv, yuv;
 Mat m_model3d;
 int c = 0;
 Rect rect;
 
 
 Mat grayframe;
 vector<Rect> people;
 
 map<int, pair<vector<Rect>,int>> object;
 
 
 
 
 float hrange[] = { 0,180 };
 float vrange[] = { 0,255 };
 float srange[] = { 0,255 };
 const float* ranges[] = { hrange, srange, vrange };
 int channels[] = { 0, 1, 2 };
 int hist_sizes[] = { 16, 16, 16 };
 bool tracking = false;
 bool is_people = false;
 
 if (!cap.isOpened()){
  cout << "can't open video file" << endl;
  return 0;
 }
 
 HOGDescriptor hog(
                   Size(48, 96),
                   Size(16, 16),
                   Size(8, 8),
                   Size(8, 8),
                   9);
 hog.setSVMDetector(HOGDescriptor::getDaimlerPeopleDetector());
 
 
 while (true){
  
  c++;
  cap >> frame;
  if(frame.empty()) break;
  
  resize(frame, frame, Size(w*resize_factor, h*resize_factor));
  
  cvtColor(frame, grayframe, COLOR_BGR2GRAY);
  
  
  cvtColor(frame, yuv,CV_BGR2YUV);
  vector<Mat> YUVchannels;
  split(yuv, YUVchannels);
  equalizeHist(YUVchannels[0], YUVchannels[0]);
  equalizeHist(YUVchannels[2], YUVchannels[2]);
  merge(YUVchannels, yuv);
  
 
  cvtColor(yuv, yuv, CV_YUV2BGR);
  
  //-----------------Hog detection---------------------------//
  
  if(!tracking || waitKey(33) == 32){
   hog.detectMultiScale(
                        yuv,
                        people,
                        1.2,
                        Size(8,8),
                        Size(16, 16),
                        1.05,
                        6);
   
   if(people.size() > 0 ){ //if detected
    
    for (int i = 0; i < (int)people.size(); i++){
     
     
     if(object[i].first.empty()){

      
      object[i].first.push_back(people[i]);
      cout <<"frame("<< c << ") obj " << i << " " << people[i].tl()<< " has created" << endl;
      is_people = true;
      break;
      
      
     }
     
     else{
      
      for(int j = 0; j < (int)object.size(); j ++){   // search objects
       
        object[i].second ++;
       
       if((int)object[i].first.size() > 0  ){
        
        int closest = findClosest(object, people[i]);
        
        if( distance((object[j].first.back().tl()+object[j].first.back().br()) / 2, (people[i].tl() + people[i].br())/2 ) > 0.01  ){
         
         if(closest == j){
          
           cout << "frame(" << c << ") " <<"p " << i << " " << people[i].tl()<< " pushed to obj " << j <<" dist: " << distance((object[j].first.back().tl()+object[j].first.back().br()) / 2, (people[i].tl() + people[i].br())/2 )*1.0 << endl;
         
                   object[j].first.push_back(people[i]);
                   object[j].second =0;
                   
          
         }

         else{
          
          cout << "frame(" << c << ") " <<"p " << i << " " << people[i].tl()<< " pushed to obj " << closest <<" dist: " << distance((object[j].first.back().tl()+object[j].first.back().br()) / 2, (people[i].tl() + people[i].br())/2 )*1.0 << endl;
                   
                   
                   object[closest].first.push_back(people[i]);
                   object[closest].second =0;
                 
          
         }
         
         is_people = true;
         
         break;
         
        }
        
        else{//  not
        
         
         cout << "frame(" << c << ") p " << i<< " " << people[i].tl()<< " not matched to obj " << j <<  " pop front " << " dist " << distance((object[j].first.back().tl()+object[j].first.back().br()) / 2, (people[i].tl() + people[i].br())/2 )<< endl;
               
         
         if(!object[i].first.empty() ){
      
          object[i].first.erase(object[i].first.begin());
         
         
         }
         
         //add new
         
         if(j == (int)object.size() -1){
          
          object[j+1].first.push_back(people[i]);
          object[j].second =0;
          is_people = true;
          
          cout << "frame(" << c << ") p "<< i<< " " << people[i].tl()<<" seems New, added to " << (int)object.size() -1 << " now Size is " << object.size()<< endl;
          
          break;
         }
         
         
        } //else
        
       }
       
       
      }
     }
     
     
    }
    
    
    
    tracking = true;
    
    
   }
   
   
  }
  
  
  //-------------------------------------------------------//
  
  
  
  cvtColor(yuv, hsv, COLOR_BGR2HSV); // equalized to hsv
  
  if (is_people) {
   
   for(int i = 0 ; i < object.size(); i++){
    
    if((int)object[i].first.size() > 0){
     Mat mask = Mat::zeros(rect.height, rect.width, CV_8U);
     ellipse(mask, Point(rect.width / 2, rect.height / 2), Size(rect.width / 2, rect.height / 2), 0, 0, 360, 255, CV_FILLED);
     Mat roi(hsv, rect);
     calcHist(&roi, 1, channels, mask, m_model3d, 3, hist_sizes, ranges);
     
     tracking = true;
     is_people = false;
    }
    
   }
  }
  
  
  //----------------------tracking---------------------------//
  
  
  if ((int)object.size() > 0){
   
   for(int i = 0 ; i < (int)object.size(); i++){
    
    if(!object[i].first.empty()&&(int)object[i].first.size() > 0 ){
     calcBackProject(&hsv, 1, channels, m_model3d, m_backproj, ranges);
     meanShift(m_backproj, object[i].first.back(), TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
     
     if((int)object.size() > 0 && object[i].second > 45){
 
         putText(frame, format("object %d has walked out", i), Point(40, 60 + (i*15)), FONT_HERSHEY_SIMPLEX,
         0.5, Scalar(255, 0, 255));
        }
     
     else{
     rectangle(frame, object[i].first.back(), Scalar(0, 0, 255), 3);
     putText(frame, format("object %d", i), object[i].first.back().tl(), FONT_HERSHEY_SIMPLEX,
             0.5, Scalar(0, 255, 0));
     
     if((int)object[i].first.size() > 3){
      putText(frame, format("object %d has entered", i), Point(40, 60 + (i*15)), FONT_HERSHEY_SIMPLEX,
              0.5, Scalar(255, 0, 0));
     }
     }
     
   
     
     if(c%3 == 0) tracking = false;
     
    }
   }
  }
  
  //---------------------------------------------------------//
  
  putText(frame, format("There are a total number of %d object in the room", object.size()), Point(40, 40), FONT_HERSHEY_SIMPLEX,
          0.5, Scalar(0, 255, 255));
  
  resize(frame, frame, Size(w, h) );
  
  imshow("object Tracking", frame);
  
  char ch = waitKey(33);
  if (ch == 27) break;
 }
 return 0;
}

float distance(Point prev, Point curr)
{
 
 float dist = 1/(1+ sqrt( pow(curr.x - prev.x, 2.0f) +
                         pow(curr.y - prev.y, 2.0f)  ) );
 
 return dist;
}


int findClosest(map<int, pair<vector<Rect>,int>> object, Rect curr){
 
 int index = 0;
 float dist = 0.0;
 for(int i = 0; i < (int)object.size() ; i ++){
  if((int)object[i].first.size() > 0){
   if(distance((object[i].first.back().tl()+object[i].first.back().br()) / 2, (curr.tl() + curr.br())/2 ) > dist){
    index = i;
    dist = distance((object[i].first.back().tl()+object[i].first.back().br()) / 2, (curr.tl() + curr.br())/2 );
   }
  }
 }
 
 return index;
}


#endif
