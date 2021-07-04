

# Advanced driver assistance system

Improving AI driver assistance system with low-level language (C, C++)



![cv_pj2_ped](https://user-images.githubusercontent.com/47979730/124393181-33153d80-dd34-11eb-9971-b0e12c17591b.PNG)



**Problem Definition**

- 운전 중 발생할 수 있는 다양한 시야 환경 변화 (터널 진입 등) 에서 안정적인 object detection의 불가
- 점선, 페인트지워짐, 교차로 등의 다양한 도로 환경에서의 차선 검출 성능 저하
-  연산량의 증가로 인한 성능 저하

**Goal**

- 주행 중 발생할 수 있는 빛의 강도 변화, 장애물, 겹침등 다양한 환경 속에서도 정확하고 지속적으로 객체를 추적할 수 있는 알고리즘을 개발합니다. 

<br>

original due : 2019/10/22 ~ 2019/12/5

restored/uploaded : 2021

<br>

## Index

->[Pedestrian Detection](https://github.com/PyoJunCode/driving-assistance-2019#Improved -Pedestrian-Detection-system)

->[Driver Assistance](https://github.com/PyoJunCode/driving-assistance-2019#Improved-Driver-assistance-system)

->[Outcome](https://github.com/PyoJunCode/driving-assistance-2019#Outcome)

<br>

## Specification

C/C++ for PL

CNN - for Image recognition (fine tuned Yolo v2)

cv2 - for video image preprocessing

<br>

## Improved -Pedestrian-Detection-system

<br>

### Goals

자동차 주행 중 여러가지 상황(빛의 강도 변화, 장애물, 겹침)에 대해서 보행자를 지속적으로 정확히 추적할 수 있는 알고리즘 개발



- Identify the appearance and walk out of moving objects

- Count the number of moving objects in the current  frame

- Display each information on the image like this:
  - Object 1 has entered
  - Object 2 has walked out
  - There are a total number of 3 objects in the room



![ped](https://user-images.githubusercontent.com/47979730/124393238-6952bd00-dd34-11eb-9bef-a36bccdc642f.PNG)

### Tech

- 갑작스런 밝기 변화에 받는 영향을 최소화 하기 위해 frame을 YUV format으로 변환한 뒤 Y(휘도) 채널에 대해서 Histogram equalization을 실행합니다.

- 프레임에서 YOLO object detection을 통해 사람을 발견하면 object의 전체 개수, 포착 시간, Euclidean distance를 계산하여 새로운 객체인지 아닌지 판별한 뒤 번호와 함께 Labeling합니다.
  - 여러명의 사람/물체를 정확히 Tracking 하기 위해 이전 frame들과 Euclidean Distance를 비교해 추적
  - 반응성과 FPS를 올리기 위해 매 프레임 전체 범위에서의 Detection을 하는것이 아닌 근처 범위 내에서의 Tracking 기법을 사용하여 최적화

- 성능 저하를 방지하기 위해 매 프레임이 아닌 동영상의 FPS에 비례하여 일정한 간격마다 위의 과정을 수행하였다. 



<img src="https://user-images.githubusercontent.com/47979730/123577145-3da07600-d80e-11eb-95b7-89c04f5a72ca.png" alt="cv_pj1_tech" style="zoom:50%;" />

<br>

## Improved-Driver-assistance-system

<br>

### Goals

향상된 Image recognition algorithm을 사용해 주행 중 여러가지 상황에 대한 정확한 인식과 경고 메세지를 화면에 실시간으로 표시.

- Lane departure warning
- Collision warning with front car
- Collision warning with pedestrian
- Front car departure alarm
- Display each information on the image like  this
  - Warning!: Lane departure
  - Warning!: Collision with front car
  - Warning!: Collision with the pedestrian
  - Warning!: Front car departure

![dri](https://user-images.githubusercontent.com/47979730/124393315-d36b6200-dd34-11eb-9241-adebf6c289a4.PNG)

### Tech

- 운전자의 차량 기준으로 Detection을 수행해야 할 범위를 Masking합니다.

- Houghline Detection을 수행해 양쪽의 차선과 그 교차점(cross point)을 탐색합니다. 

  - 차선의 교차점의 좌표가 많이 이동하거나 한쪽 차선이 사라지면 화면에 Lane departure Warning을 표시합니다. 

- YOLO detection을 수행하며 범위 내에서 차량,사람을 발견하면 해당 객체를 추적합니다.

- 추적하는 객체의 Size를 계산하여 증가값이 threshold를 넘어가면 화면에 해당 객체와의 Collision Warning을 표시합니다.



<br>



## Outcome



- 갑작스런 빛의 세기 변화에도 Tracking중인 object를 잃지 않음.

- 새로운 Object가 등장하거나 Object끼리 겹쳐 지나가도 원래의 Label을 잘 유지함.

![outcome](https://user-images.githubusercontent.com/47979730/124393365-21806580-dd35-11eb-9fb2-11fe65dec05c.PNG)





![cv_pj2_car](https://user-images.githubusercontent.com/47979730/123591236-abf13280-d826-11eb-8db5-6a11c624d4d6.PNG)

- Collision detection with front car

![cv_pj2_ped](https://user-images.githubusercontent.com/47979730/123591241-adbaf600-d826-11eb-8d16-89a2ce9341f5.PNG)

- Collision detection with pedestrian

