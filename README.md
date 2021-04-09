# 라즈베리파이3 기기 간 소켓 통신 틱택토(Tic-Tac-Toe) 게임
#### Socket Game(Tic-Tac-Toe) between Raspberry-Pi


![image](https://user-images.githubusercontent.com/26182769/114135869-d7116780-9944-11eb-8117-aac7b3d0a878.png)




### TIC-TAC-TOE
- 두 명이 번갈아 가며 O와 X를 3×3 판에 써서,
  같은 3개의 글자를 한 줄로 놓이도록 하는 놀이


### 사용 환경
- Raspberry-Pi3B+
- Ubuntu
- Qt
- C, C++


### 게임 방법
1. 호스트의 IP 주소를 입력 후 connect를 누른다.
2. 연결 후, 준비가 되면 start를 눌러 게임을 시작한다.
3. 놓을 곳을 누르고 put 버튼으로 차례를 넘긴다.
4. 자신의 차례일 때 항복하려면 surrender 버튼을 누른다.
5. exit 버튼으로 게임을 종료한다.


### 게임 동작
- 한 명은 Create Game으로 방을 만들어 서버가 되고,
- 한 명은 Join Game으로 방에 참가해 클라이언트가 된다.
- 호스트 간 연결이 끝나면 각 호스트는 아래와 같은 동작을 반복하게 된다.
- (종료조건 확인 → 입력 → 전송 → 종료조건 확인 → 수신 대기)
- 적절한 버튼 비활성화 및 게임 동작 구조화로 버그를 방지했다.


### GUI
![image](https://user-images.githubusercontent.com/26182769/114133049-62d4c500-9940-11eb-88ab-0a6c44332c92.png)



