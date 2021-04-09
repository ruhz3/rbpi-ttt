#include "hostdialog.h"
#include "ui_hostdialog.h"
#include <QMessageBox>
#include <QColorDialog>
#include <QFont>

#define PORT 4250
#define DATA_LEN 9

hostDialog::hostDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::hostDialog)
{
    ui->setupUi(this);

    ui->start->setEnabled(false);
    ui->put->setEnabled(false);
    ui->surrender->setEnabled(false);
    button_disabled();

    // placeholder setting
    ui->myscore->setPlaceholderText(QString::number(score[0]));
    ui->opscore->setPlaceholderText(QString::number(score[1]));
    ui->myscore->setEnabled(false);
    ui->opscore->setEnabled(false);
    ui->debugging->setReadOnly(true);
    ui->explain->setReadOnly(true);
    ui->ip->setReadOnly(true);
    ui->ip->setPlaceholderText("No need to type!");
}

hostDialog::~hostDialog()
{
    delete ui;
}

void hostDialog::on_exit_clicked()
{
    ::close(conn_flag);
    this->close();
}

void hostDialog::on_connect_clicked()
{
    // socket init
    if ((sock_flag = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        ui->debugging->append("Socket 생성 실패...");
        printf("Socket 생성 실패...");
        exit(0);
    }
    else
        ui->debugging->append("Socket 생성 성공...");

    memset(&server_addr, 0x00, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if ((::bind(sock_flag, (struct sockaddr*)&server_addr, sizeof(server_addr))) != 0)
    {
        ui->debugging->append("소켓 바인딩 실패...");
        printf("Socket binding 실패...\n");
        exit(0);
    }
    else
        ui->debugging->append("소켓 바인딩 성공...");

    if ((::listen(sock_flag, 1)) != 0)
    {
        ui->debugging->append("연결 대기 실패...");
        exit(0);
    }

    ui->debugging->append("enter connect");
    length = sizeof(client_addr);

    if ((conn_flag = ::accept(sock_flag, (struct sockaddr*)&client_addr, &length)) < 0) {
        ui->debugging->append("서버-클라이언트 연결 실패");
        printf("Socket connection 실패...");
        exit(0);
    }
    else
        ui->debugging->append("서버-클라이언트 연결 성공");

    tcp=true;
    ui->connect->setEnabled(false); // connect button disabled
    ui->start->setEnabled(true);
    ui->exit->setEnabled(true);
    ui->ip->setEnabled(false);

}

void hostDialog :: DATA_SEND_RECV(int socket_fd)
{
    ui->debugging->append("round start");

    // 만약 내 차례가 됐다면,
    if (myTurn) {
        ui->debugging->append("my turn");
        button_enabled();
        ui->surrender->setEnabled(true);
    }
    // not my turn -> Read
    else {
        if (Read(socket_fd) > 0) {
            myTurn = true;

            // 1. print screen
            Print();

            // 2. 종료조건을 검사한다.
            check_result = check();
            ui->debugging->append("check complete");

            // 3. 점수 올리고 종료
            if (check_result == 's') { // 상대가 항복한 경우
                score[0] += 1;
                win();
                end_init();
            }
            else if (check_result == mychar) { // 이긴 경우
                score[0] += 1;
                win();
                end_init();
            }
            else if (check_result == opchar) { // 진 경우
                score[1] += 1;
                lose();
                end_init();
            }
            else if (check_result == 'd') { // 비긴 경우
                draw();
                end_init();
            }
            button_enabled();
            ui->put->setEnabled(false);
            ui->surrender->setEnabled(true);
        }
    }
}

int hostDialog :: Read(int socket_fd) {
    char data[DATA_LEN];
    memset(board, 0x00, DATA_LEN);
    int readbyte = read(socket_fd, data, DATA_LEN);
    if (readbyte < 0) {
        ui->debugging->append("READ socket error!");
        return readbyte;
    }
    else {
        ui->debugging->append("READ socket success!");
        for (int i = 0; i < DATA_LEN; i++) {
            board[i] = data[i];
        }
        return readbyte;
    }
}
int hostDialog :: Write(int socket_fd) {
    int writebyte = write(socket_fd, board, DATA_LEN);
    if (writebyte < 0) {
        ui->debugging->append("WRITE socket error!");
        return writebyte;
    }
    else {
        ui->debugging->append("WRITE socket success!");
        return writebyte;
    }
}
void hostDialog :: button_init(){
    ui->board_1->setText(QString(" "));
    ui->board_2->setText(QString(" "));
    ui->board_3->setText(QString(" "));
    ui->board_4->setText(QString(" "));
    ui->board_5->setText(QString(" "));
    ui->board_6->setText(QString(" "));
    ui->board_7->setText(QString(" "));
    ui->board_8->setText(QString(" "));
    ui->board_9->setText(QString(" "));
}

void hostDialog :: end_init() {
    ui->myscore->setText(QString::number(score[0]));
    ui->opscore->setText(QString::number(score[1]));
    imFirst = !imFirst;
    memset(board, 'e', DATA_LEN);
    ui->start->setEnabled(true);
    ui->exit->setEnabled(true);
    ui->put->setEnabled(false);
    ui->surrender->setEnabled(false);
    button_disabled();
}
void hostDialog :: win() {
    ui->debugging->append("win\n");
    QMessageBox msgBox;
    msgBox.setWindowTitle("Result");
    msgBox.setStyleSheet("color: black; background-color: white;");
    msgBox.setText("WIN");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
void hostDialog :: lose() {
    ui->debugging->append("lose\n");
    QMessageBox msgBox;
    msgBox.setWindowTitle("Result");
    msgBox.setStyleSheet("color: black; background-color: white;");
    msgBox.setText("LOSE");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
void hostDialog :: draw() {
    ui->debugging->append("draw\n");
    QMessageBox msgBox;
    msgBox.setWindowTitle("DRAW");
    msgBox.setStyleSheet("color: black; background-color: white;");
    msgBox.setText("DRAW");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
void hostDialog ::Print() {
    for(int i=0;i<9;i++){
        if(board[i]=='x'){
            if(i==0) ui->board_1->setText(QString("X"));
            else if(i==1) ui->board_2->setText(QString("X"));
            else if(i==2) ui->board_3->setText(QString("X"));
            else if(i==3) ui->board_4->setText(QString("X"));
            else if(i==4) ui->board_5->setText(QString("X"));
            else if(i==5) ui->board_6->setText(QString("X"));
            else if(i==6) ui->board_7->setText(QString("X"));
            else if(i==7) ui->board_8->setText(QString("X"));
            else if(i==8) ui->board_9->setText(QString("X"));
        }
        else if(board[i]=='o'){
            if(i==0) ui->board_1->setText(QString("O"));
            else if(i==1) ui->board_2->setText(QString("O"));
            else if(i==2) ui->board_3->setText(QString("O"));
            else if(i==3) ui->board_4->setText(QString("O"));
            else if(i==4) ui->board_5->setText(QString("O"));
            else if(i==5) ui->board_6->setText(QString("O"));
            else if(i==6) ui->board_7->setText(QString("O"));
            else if(i==7) ui->board_8->setText(QString("O"));
            else if(i==8) ui->board_9->setText(QString("O"));
        }
        else if(board[i]=='e'){
            if(i==0) ui->board_1->setText(QString(" "));
            else if(i==1) ui->board_2->setText(QString(" "));
            else if(i==2) ui->board_3->setText(QString(" "));
            else if(i==3) ui->board_4->setText(QString(" "));
            else if(i==4) ui->board_5->setText(QString(" "));
            else if(i==5) ui->board_6->setText(QString(" "));
            else if(i==6) ui->board_7->setText(QString(" "));
            else if(i==7) ui->board_8->setText(QString(" "));
            else if(i==8) ui->board_9->setText(QString(" "));
        }

    }

}
void hostDialog :: button_disabled() {
    ui->board_1->setEnabled(false);
    ui->board_2->setEnabled(false);
    ui->board_3->setEnabled(false);
    ui->board_4->setEnabled(false);
    ui->board_5->setEnabled(false);
    ui->board_6->setEnabled(false);
    ui->board_7->setEnabled(false);
    ui->board_8->setEnabled(false);
    ui->board_9->setEnabled(false);
}
void hostDialog :: button_enabled() {
    ui->board_1->setEnabled(true);
    ui->board_2->setEnabled(true);
    ui->board_3->setEnabled(true);
    ui->board_4->setEnabled(true);
    ui->board_5->setEnabled(true);
    ui->board_6->setEnabled(true);
    ui->board_7->setEnabled(true);
    ui->board_8->setEnabled(true);
    ui->board_9->setEnabled(true);
}
char hostDialog :: check() {
    int cnt = 0;
    if (strncmp(board, "endgame", 7) == 0) {
        return 's';
    }
    else {
        for (int i = 0; i < DATA_LEN; i++) {
            if (board[i] == 'e')
                cnt++;
        }
        if ((board[0] == board[1]) && (board[1] == board[2]) && (board[0] != 'e')) {
            return board[0];
        }
        else if ((board[3] == board[4]) && (board[4] == board[5]) && (board[3] != 'e')) {
            return board[3];
        }
        else if ((board[6] == board[7]) && (board[7] == board[8]) && (board[6] != 'e')) {
            return board[6];
        }
        else if ((board[0] == board[3]) && (board[3] == board[6]) && (board[0] != 'e')) {
            return board[0];
        }
        else if ((board[1] == board[4]) && (board[4] == board[7]) && (board[1] != 'e')) {
            return board[1];
        }
        else if ((board[2] == board[5]) && (board[5] == board[8]) && (board[2] != 'e')) {
            return board[2];
        }
        else if ((board[0] == board[4]) && (board[4] == board[8]) && (board[0] != 'e')) {
            return board[0];
        }
        else if ((board[2] == board[4]) && (board[4] == board[6]) && (board[2] != 'e')) {
            return board[2];
        }
        else if (cnt == 0) {
            return 'd';
        }
        else {
            return 'e';
        }
    }
}

void hostDialog::on_board_1_clicked()
{
    if(myTurn && board[0]=='e'){
        board[0] = mychar;
        button_disabled();
        Print();
        ui->put->setEnabled(true);
        ui->surrender->setEnabled(false);
    }
    else if(myTurn == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Not your Turn");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else if(board[0]!='e'){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Can't put!");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
}

void hostDialog::on_board_2_clicked()
{
    if(myTurn && board[1]=='e'){
        board[1] = mychar;
        button_disabled();
        Print();
        ui->put->setEnabled(true);
        ui->surrender->setEnabled(false);
    }
    else if(myTurn == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Not your Turn");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else if(board[1]!='e'){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Can't put!");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
}

void hostDialog::on_board_3_clicked()
{
    if(myTurn && board[2]=='e'){
        board[2] = mychar;
        button_disabled();
        Print();
        ui->put->setEnabled(true);
        ui->surrender->setEnabled(false);
    }
    else if(myTurn == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Not your Turn");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else if(board[2]!='e'){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Can't put!");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
}

void hostDialog::on_board_4_clicked()
{
    if(myTurn && board[3]=='e'){
        board[3] = mychar;
        Print();
        button_disabled();
        ui->put->setEnabled(true);
        ui->surrender->setEnabled(false);
    }
    else if(myTurn == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Not your Turn");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else if(board[3]!='e'){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Can't put!");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
}

void hostDialog::on_board_5_clicked()
{
    if(myTurn && board[4]=='e'){
        board[4] = mychar;
        Print();
        button_disabled();
        ui->put->setEnabled(true);
        ui->surrender->setEnabled(false);
    }
    else if(myTurn == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Not your Turn");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else if(board[4]!='e'){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Can't put!");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
}

void hostDialog::on_board_6_clicked()
{
    if(myTurn && board[5]=='e'){
        board[5] = mychar;
        Print();
        button_disabled();
        ui->put->setEnabled(true);
        ui->surrender->setEnabled(false);
    }
    else if(myTurn == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Not your Turn");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else if(board[5]!='e'){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Can't put!");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
}

void hostDialog::on_board_7_clicked()
{
    if(myTurn && board[6]=='e'){
        board[6] = mychar;
        Print();
        button_disabled();
        ui->put->setEnabled(true);
        ui->surrender->setEnabled(false);
    }
    else if(myTurn == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Not your Turn");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else if(board[6]!='e'){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Can't put!");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
}

void hostDialog::on_board_8_clicked()
{
    if(myTurn && board[7]=='e'){
        board[7] = mychar;
        Print();
        button_disabled();
        ui->put->setEnabled(true);
        ui->surrender->setEnabled(false);
    }
    else if(myTurn == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Not your Turn");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else if(board[7]!='e'){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Can't put!");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
}

void hostDialog::on_board_9_clicked()
{
    if(myTurn && board[8]=='e'){
        board[8] = mychar;
        Print();
        button_disabled();
        ui->put->setEnabled(true);
        ui->surrender->setEnabled(false);
    }
    else if(myTurn == false){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Not your Turn");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
    else if(board[8]!='e'){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Alert");
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("Can't put!");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
    }
}

void hostDialog::on_start_clicked()
{
    button_init();
    ui->debugging->append("game start");

    // 내가 선공이라면, O를 사용한다.
    if (imFirst) {
        mychar = 'o';
        opchar = 'x';
        QMessageBox msgBox;
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("You First!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        myTurn=true;
    }
    else {
        mychar = 'x';
        opchar = 'o';
        QMessageBox msgBox;
        msgBox.setStyleSheet("color: black; background-color: white;");
        msgBox.setText("You Last!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        myTurn = false;
    }

    ui->exit->setEnabled(false);
    ui->start->setEnabled(false);

    DATA_SEND_RECV(conn_flag);
}

void hostDialog::Put(){
    Write(conn_flag);

    myTurn = false;
    // 종료조건 검사
    check_result = check();
    ui->debugging->append("check success");

    if (check_result == 's') { // 내가 항복한 경우
        score[1] += 1;
        lose();
        end_init();
        return;
    }
    else if (check_result == mychar) { // 이긴 경우
        score[0] += 1;
        win();
        end_init();
        return;
    }
    else if (check_result == opchar) { // 진 경우
        score[1] += 1;
        lose();
        end_init();
        return;
    }
    else if (check_result == 'd') { // 비긴 경우
        draw();
        end_init();
        return;
    }

    if (Read(conn_flag) > 0) {
        myTurn = true;

        // 1. print screen
        Print();

        // 2. 종료조건을 검사한다.
        check_result = check();
        ui->debugging->append("check complete");

        // 3. 점수 올리고 종료
        if (check_result == 's') { // 상대가 항복한 경우
            score[0] += 1;
            win();
            end_init();
            ui->surrender->setEnabled(false);
            return;
        }
        else if (check_result == mychar) { // 이긴 경우
            score[0] += 1;
            win();
            end_init();
            return;
        }
        else if (check_result == opchar) { // 진 경우
            score[1] += 1;
            lose();
            end_init();
            return;
        }
        else if (check_result == 'd') { // 비긴 경우
            draw();
            end_init();
            return;
        }
        button_enabled();
        ui->put->setEnabled(false);
        ui->surrender->setEnabled(true);
    }
}

void hostDialog::on_put_clicked()
{
    Put();
}

void hostDialog::on_surrender_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Would you like to surrender?");
    msgBox.setStyleSheet("color: black; background-color: white;");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    button_disabled();

    if(msgBox.exec()==QMessageBox::Yes){
        memset(board,0x00,DATA_LEN);
        strcpy(board,"endgame");
        Put();
        ui->surrender->setEnabled(false);
    }
    else{
        button_enabled();
    }
}
