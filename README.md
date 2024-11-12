1. speed dxl
이 코드는 Dynamixel 모터의 속도를 제어하고 실행 시간을 측정하는 프로그램입니다. 주요 흐름을 설명하면 다음과 같습니다:

시그널 핸들러 설정:

signal(SIGINT, ctrlc);를 통해 Ctrl+C 신호를 받으면 ctrlc() 함수가 호출되어 ctrl_c_pressed 변수를 true로 설정합니다. 이를 통해 프로그램을 안전하게 종료할 수 있습니다.
Dynamixel 장치 열기:

Dxl 객체 mx의 open() 함수를 호출해 장치를 엽니다. 만약 열리지 않으면 오류 메시지를 출력하고 프로그램을 종료합니다.
반복 루프 (속도 입력 및 설정):

while 루프가 계속되며 사용자가 왼쪽과 오른쪽 모터 속도를 입력하도록 요청합니다.
mx.setVelocity(leftspeed, rightspeed);를 호출해 입력된 속도를 Dynamixel 모터에 설정합니다.
usleep(20*1000);로 20ms 대기합니다.
gettimeofday로 루프 실행 시간을 측정하고 출력합니다.
ctrl_c_pressed가 true일 경우 루프를 종료합니다.
Dynamixel 장치 닫기:

루프가 끝나면 mx.close();를 호출하여 Dynamixel 장치를 닫고 프로그램을 종료합니다.

2.
