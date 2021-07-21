#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QKeyEvent>
#define BLOCK_01_X_POS 300
#define BLOCK_01_Y_POS 40
#define BLOCK_02_X_POS 2300
#define BLOCK_02_Y_POS 110
#define BLOCK_03_X_POS 900
#define BLOCK_03_Y_POS 190
#define BLOCK_04_X_POS 2800
#define BLOCK_04_Y_POS 260
#define BLOCK_05_X_POS 1700
#define BLOCK_05_Y_POS 310
#define BLOCK_06_X_POS 1500
#define BLOCK_06_Y_POS 340
/*
0 => init 遊戲被開啟時
1 => playing 遊戲遊玩時
2 => pause 遊戲暫停時
3 => timeout 時間到，遊戲結束時
4 => die 碰到障礙物，遊戲結束時
*/

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game_status = 0;

    ui->block_01->setGeometry(QRect(-100, 0, 64, 64));
    ui->block_02->setGeometry(QRect(-100, 0, 64, 64));
    ui->block_03->setGeometry(QRect(-100, 0, 64, 64));
    ui->block_04->setGeometry(QRect(-100, 0, 64, 64));
    ui->block_05->setGeometry(QRect(-100, 0, 64, 64));
    ui->block_06->setGeometry(QRect(-100, 0, 64, 64));

    //移動物體
    bgm_pos = 0;
    object_timer = new QTimer(this);    //建立計時器
    connect(object_timer, SIGNAL(timeout()), this, SLOT(update_object())); //連接信號
    object_timer->start(10);    //每隔10毫秒更新一次

    //移動時間
    time = 30;
    clock_timer = new QTimer(this);    //建立計時器
    connect(clock_timer, SIGNAL(timeout()), this, SLOT(update_time())); //連接信號
    //clock_timer->start(1000);    //每隔1000毫秒更新一次
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_object()
{
    bgm_pos -= 1;//背景每次往左移動1

    ui->background->setGeometry(QRect(bgm_pos, 0, 4800, 512));
    if(bgm_pos == -32)
    {
        bgm_pos = 0;//當移動一圈背景從頭顯示
    }

    if(game_status == 1)
    {//playing
        car_distance += 1;
        ui->label_distance->setText("行駛距離: " + QString::number(car_distance, 'f', 0) + "公尺");  //顯示移動距離
        move_car();
        move_blocks();
        detect_blocks();
    }
}

void MainWindow::update_time()
{
    time -= 1;//時間每次減少1

    ui->lcd_clock->display((time));
    if(time == 0)
    {
        game_status = 3;
        game_stop();
    }
}

void MainWindow::game_stop()
{
    clock_timer->stop();
    object_timer->stop();

    if(game_status == 3)//timeout
        ui->label_title->setPixmap(QPixmap(":/src/game_timeout.png"));

    else if(game_status == 4)//die
        ui->label_title->setPixmap(QPixmap(":/src/game_falied.png"));

    ui->label_title->setVisible(true);
    ui->pushButton_start_game->setVisible(true);
}

void MainWindow::on_pushButton_start_game_clicked()
{
    game_start();
}

void MainWindow::game_start()
{
    game_status = 1;

    ui->label_title->setVisible(false);             //隱藏遊戲標題
    ui->pushButton_start_game->setVisible(false);   //隱藏開始遊戲按鈕

    clock_timer->start(1000);                       //每1秒更新一次
    time = 30;                                      //重設遊戲時間
    ui->lcd_clock->display(time);                   //顯示遊戲時間

    car_pos = 220;                                  //重設車位置(中央)
    car_distance = 0;                               //重設移動距離
    car_direction = 0;                              //設定初始方向

    bgm_pos = 0;                                    //重設背景位置

    if(!object_timer->isActive())
        object_timer->start(10);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Up:
        {
            car_direction = -1;
            break;
        }
        case Qt::Key_Down:
        {
            car_direction = 1;
            break;
        }
        case Qt::Key_Space:
        {
            game_pause();
            break;
        }
    }
}

void MainWindow::move_car()
{
    int car_new_pos = car_pos + car_direction;

    if((car_new_pos >=30) && (car_new_pos <= 410))
    {//未超線
        car_pos = car_new_pos;  //變更車位置

        ui->car->setGeometry(QRect(10, car_pos, 237, 71));//設置車位置
    }
    else
    {
        game_status = 4;    //die
        game_stop();
    }
}

void MainWindow::move_blocks()
{
    ui->block_01->setGeometry(QRect(BLOCK_01_X_POS - car_distance, BLOCK_01_Y_POS, 64, 64));
    ui->block_02->setGeometry(QRect(BLOCK_02_X_POS - car_distance, BLOCK_02_Y_POS, 64, 64));
    ui->block_03->setGeometry(QRect(BLOCK_03_X_POS - car_distance, BLOCK_03_Y_POS, 64, 64));
    ui->block_04->setGeometry(QRect(BLOCK_04_X_POS - car_distance, BLOCK_04_Y_POS, 64, 64));
    ui->block_05->setGeometry(QRect(BLOCK_05_X_POS - car_distance, BLOCK_05_Y_POS, 64, 64));
    ui->block_06->setGeometry(QRect(BLOCK_06_X_POS - car_distance, BLOCK_06_Y_POS, 64, 64));
}

void MainWindow::detect_blocks()
{
    int tmp_padding_x,tmp_padding_y;

    tmp_padding_x = BLOCK_01_X_POS - car_distance;
    tmp_padding_y = BLOCK_01_Y_POS - ui->car->y();
    if(is_collision(tmp_padding_x,tmp_padding_y))
        return;

    tmp_padding_x = BLOCK_02_X_POS - car_distance;
    tmp_padding_y = BLOCK_02_Y_POS - ui->car->y();
    if(is_collision(tmp_padding_x,tmp_padding_y))
        return;

    tmp_padding_x = BLOCK_03_X_POS - car_distance;
    tmp_padding_y = BLOCK_03_Y_POS - ui->car->y();
    if(is_collision(tmp_padding_x,tmp_padding_y))
        return;

    tmp_padding_x = BLOCK_04_X_POS - car_distance;
    tmp_padding_y = BLOCK_04_Y_POS - ui->car->y();
    if(is_collision(tmp_padding_x,tmp_padding_y))
        return;

    tmp_padding_x = BLOCK_05_X_POS - car_distance;
    tmp_padding_y = BLOCK_05_Y_POS - ui->car->y();
    if(is_collision(tmp_padding_x,tmp_padding_y))
        return;

    tmp_padding_x = BLOCK_06_X_POS - car_distance;
    tmp_padding_y = BLOCK_06_Y_POS - ui->car->y();
    if(is_collision(tmp_padding_x,tmp_padding_y))
        return;
}

bool MainWindow::is_collision(int x, int y)
{
    if(x<247 && x>-50)
    {
        if(y<55 && y>-55)
        {
            game_status = 4;
            game_stop();
            return true;
        }
    }
    return false;
}

void MainWindow::game_pause()
{
    if(game_status == 1)
    {
        game_status = 2;
        clock_timer->stop();
        object_timer->stop();
        ui->label_title->setPixmap(QPixmap(":/src/game_pause.png"));
        ui->label_title->setVisible(true);
    }

    else if(game_status == 2)
    {
        game_status = 1;
        clock_timer->start(1000);
        object_timer->start(10);
        ui->label_title->setVisible(false);
    }
}
