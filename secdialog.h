#ifndef SECDIALOG_H
#define SECDIALOG_H

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
#include <arpa/inet.h>

namespace Ui {
class secDialog;
}

class secDialog : public QDialog
{
    Q_OBJECT
    int socket_fd;

    bool myTurn = false;
    bool imFirst = false;

    int score[2] = { 0, 0 };
    char board[9] = { 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e' };
    int game_result;
    char check_result;
    char mychar;
    char opchar;


public:
    explicit secDialog(QWidget *parent = nullptr);
    ~secDialog();

private slots:
    void on_exit_clicked();

    void on_connect_clicked();

    void DATA_SEND_RECV(int);
    int Read(int);
    int Write(int);
    char check();
    void Print();
    void win();
    void lose();
    void draw();
    void end_init();
    void button_init();
    void button_enabled();
    void button_disabled();
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
    Ui::secDialog *ui;
};

#endif // SECDIALOG_H
