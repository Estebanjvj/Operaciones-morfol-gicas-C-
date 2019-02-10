#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pbAbrir_clicked();

    void on_slBrillo_valueChanged(int value);

    void on_slEriosion_valueChanged(int value);

    void on_slDilatacion_valueChanged(int value);

    void on_pb3_clicked();

    void on_pb4_clicked();

private:
    Ui::MainWindow *ui;

    const QString DEFAULT_PATH = "C:/Users/zaory/Documents/Los dataset/IA.C2.EP1 Operaciones morfologicas+temas previos";
    cv::Mat inMat, outMat, grayMat, erodeMat, dilateMat;

    void clasification(cv::Mat src, int c);
    //bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 );
};

#endif // MAINWINDOW_H
