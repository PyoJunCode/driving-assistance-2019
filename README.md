

# Advanced driver assistance system

Improving AI driver assistance system with low-level language (C, C++)

original due : 2019/10/22 ~ 2019/12/5

restored/uploaded : 2021

<br>

## Specification

C/C++ - PL

CNN - for Image recognition (fine tuned Yolo v2)

cv2 - for video image preprocessing



## Improved  surveillance camera system

<br>

### Goals

자동차 주행 중 여러가지 상황(빛의 강도 변화, 장애물, 겹침)에 대해서 보행자를 지속적으로 정확히 추적할 수 있는 알고리즘 개발



- Identify the appearance and walk out of moving objects

- Count the number of moving objects in the current  frame

- Display each information on the image like this:
  - Object 1 has entered
  - Object 2 has walked out
  - There are a total number of 3 objects in the room



### Tech

- 반응성과 FPS를 올리기 위해 매 프레임 전체 범위에서의 Detection을 하는것이 아닌 근처 범위 내에서의 Tracking 기법을 사용하여 최적화
- 여러명의 사람/물체를 정확히 Tracking 하기 위해 이전 frame들과 Euclidean Distance를 비교해 추적
- 갑작스러운 빛의 변화(Light on/off, Flashlight) 등의 상황에서 Tracking 정보 손실을 방지하기 위해 전체 영상의 Preprocessing

![cv_pj1_tech](https://user-images.githubusercontent.com/47979730/123577145-3da07600-d80e-11eb-95b7-89c04f5a72ca.png)





![cv_pj1](https://user-images.githubusercontent.com/47979730/123574318-94a44c00-d80a-11eb-8857-89b8232f4339.PNG)







## Driver assistance system

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



### Tech

CNN기반 Yolo v2 model을 사용하여 Label이 사람과 자동차인 object를 검출. 

앞차의 Object 크기를 계산해서 변화량에 따라 앞차와의 거리, 차량의 정지, 출발 등을 감지.

Houghlines를 이용해 차선의 crosspoint를 검출.





![cv_pj2_car](https://user-images.githubusercontent.com/47979730/123591236-abf13280-d826-11eb-8db5-6a11c624d4d6.PNG)

(Collision detection with front car)

![cv_pj2_ped](https://user-images.githubusercontent.com/47979730/123591241-adbaf600-d826-11eb-8d16-89a2ce9341f5.PNG)

(Collision detection with pedestrian)

