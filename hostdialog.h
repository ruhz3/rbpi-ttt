#ifndef HOSTDIALOG_H
#define HOSTDIALOG_H

#include <QDialog>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdbool.h>

namespace Ui {
class hostDialog;
}

class hostDialog : public QDialog
{
    Q_OBJECT
    int sock_flag, conn_flag;
    socklen_t length;
    struct sockaddr_in server_addr, client_addr;
    bool myTurn = false;
    bool imFirst = true;
    bool tcp = false;

    int score[2] = { 0, 0 };
    char board[9] = { 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e' };
    int game_result;
    char check_result;
    char mychar;
    char opchar;

public:
    explicit hostDialog(QWidget *parent = nullptr);
    ~hostDialog();

private slots:
    void on_exit_clicked();

    void on_connect_clicked();

    void DATA_SEND_RECV(int);
    int Read(int);
    int Write(int);
    char check();
    void Print();
    void button_disabled();
    void button_enabled();
    void win();
    void lose();
    void draw();
    void end_init();
    void button_init();
    void Put();

    void on_board_1_clicked();

    void on_board_2_clicked();

    void on_board_3_clicked();

    void on_board_4_clicked();

    void on_board_5_clicked();

    void on_board_6_clicked();

    void on_board_7_clicked();

    void on_board_8_clicked();

    void on_board_9_clicked();

    void on_start_clicked();

    void on_put_clicked();

    void on_surrender_clicked();

private:
    Ui::hostDialog *ui;
};

#endif // HOSTDIALOG_H
