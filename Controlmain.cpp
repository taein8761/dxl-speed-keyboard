#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "dxl.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

bool ctrl_c_pressed;

// Ctrl+C 입력 시 종료 처리 함수
void ctrlc(int) {
    ctrl_c_pressed = true;
}

int main(void) {
    // 카메라 스트리밍을 위한 GStreamer 파이프라인 설정
    string src = "nvarguscamerasrc sensor-id=0 ! \
                  video/x-raw(memory:NVMM), width=(int)640, height=(int)360, \
                  format=(string)NV12, framerate=(fraction)30/1 ! \
                  nvvidconv flip-method=0 ! video/x-raw, \
                  width=(int)640, height=(int)360, format=(string)BGRx ! \
                  videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    VideoCapture source(src, CAP_GSTREAMER);

    if (!source.isOpened()) {
        cout << "카메라 오류" << endl;
        return -1;
    }

    // UDP 스트리밍을 위한 VideoWriter 생성
    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
                   nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
                   h264parse ! rtph264pay pt=96 ! \
                   udpsink host=203.234.58.156 port=8001 sync=false";
    VideoWriter writer1(dst1, 0, (double)30, Size(640, 360), true);

    if (!writer1.isOpened()) {
        cerr << "UDP 스트리밍을 위한 Writer 열기 실패!" << endl;
        return -1;
    }

    // 동영상 파일로 저장하기 위한 VideoWriter 생성
    VideoWriter videoFile("output_video.mp4", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30.0, Size(640, 360), true);
    if (!videoFile.isOpened()) {
        cerr << "동영상 파일 저장을 위한 Writer 열기 실패!" << endl;
        return -1;
    }

    Mat frame, gray;

    // 다이나믹셀 제어
    Dxl mx;
    struct timeval start, end1;
    double diff1;
    int vel1 = 0, vel2 = 0;
    int goal1 = 0, goal2 = 0;

    signal(SIGINT, ctrlc);  // 시그널 핸들러 지정

    if (!mx.open()) {
        cout << "다이나믹셀 열기 실패" << endl;
        return -1;
    }

    // 장치 열기 및 프레임 처리
    while (true) {
        gettimeofday(&start, NULL);
        source >> frame;

        if (frame.empty()) {
            cerr << "프레임이 비어 있습니다!" << endl;
            break;
        }

        // 프레임을 UDP 스트리밍에 전송
        writer1 << frame;

        // 프레임을 동영상 파일로 저장
        videoFile << frame;

        // 키보드 입력 처리
        if (mx.kbhit()) {
            char c = mx.getch();
            switch (c) {
                case 's':  // 정지
                    goal1 = 0;
                    goal2 = 0;
                    break;
                case ' ':
                    goal1 = 0;
                    goal2 = 0;
                    break;
                case 'f':  // 전진
                    goal1 = 50;
                    goal2 = -50;
                    break;
                case 'b':  // 후진
                    goal1 = -50;
                    goal2 = 50;
                    break;
                case 'l':  // 좌회전
                    goal1 = -50;
                    goal2 = -50;
                    break;
                case 'r':  // 우회전
                    goal1 = 50;
                    goal2 = 50;
                    break;
                default:
                    goal1 = 0;
                    goal2 = 0;
                    break;
            }
        }

        // 가속 및 감속 동작 처리
        if (goal1 > vel1) vel1 += 5;
        else if (goal1 < vel1) vel1 -= 5;
        else vel1 = goal1;

        if (goal2 > vel2) vel2 += 5;
        else if (goal2 < vel2) vel2 -= 5;
        else vel2 = goal2;

        // 다이나믹셀에 속도 설정
        if (!mx.setVelocity(vel1, vel2)) {
            cout << "속도 설정 오류" << endl;
            return -1;
        }

        // Ctrl+C가 눌렸으면 종료
        if (ctrl_c_pressed) break;

        usleep(20 * 1000);  // 20ms 대기

        gettimeofday(&end1, NULL);
        diff1 = end1.tv_sec + end1.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
        cout << "vel1: " << vel1 << ", vel2: " << vel2 << ", 시간: " << diff1 << endl;
    }

    mx.close();  // 다이나믹셀 연결 종료
    videoFile.release();  // 비디오 파일 저장 종료
    return 0;
}
