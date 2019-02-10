#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// comparison function object
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i < j );
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->leMax->setValidator( new QIntValidator(2, 100000, this) );
    ui->leMin->setValidator( new QIntValidator(1, 1000, this) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbAbrir_clicked()
{
    //abrir la imagen alv
    QString nameFileInput = QFileDialog::getOpenFileName(this,       //path QString
         tr("Open Image"),
         DEFAULT_PATH,
         tr("Image Files (*.png *.jpg *.hdr *.jpeg)"));

    if (nameFileInput == NULL)
        return;
    qDebug() << nameFileInput;
    string pathFileInput = nameFileInput.toUtf8().constData();  //same path but std string
    inMat = imread(pathFileInput);

    namedWindow("Original", 1);
    imshow("Original",inMat);

    /// Convert the image to Gray
    cvtColor( inMat, grayMat, CV_BGR2GRAY );
    //namedWindow("Gray Mat", 1);
    //imshow("Gray Mat",grayMat);

    threshold( grayMat, outMat, 0, 255, CV_THRESH_BINARY+CV_THRESH_OTSU);
}


void MainWindow::on_slBrillo_valueChanged(int value)
{
    //threshold( grayMat, outMat, 0, 255, CV_THRESH_BINARY+CV_ADAPTIVE_THRESH_MEAN_C, 5, value );
    adaptiveThreshold(grayMat,outMat,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,5,value);

    namedWindow("Threshold Mat", 1);
    imshow("Threshold Mat",outMat);
}

void MainWindow::on_slEriosion_valueChanged(int value)
{
    int erosion_size = value;
    Mat element = getStructuringElement( MORPH_CROSS,
                                           Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                           Point( erosion_size, erosion_size ) );
    erode(outMat,erodeMat,element);
    namedWindow("Erode Mat", 1);
    imshow("Erode Mat",erodeMat);
}

void MainWindow::on_slDilatacion_valueChanged(int value)
{
    int erosion_size = value;
    Mat element = getStructuringElement( MORPH_RECT,
                                           Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                           Point( erosion_size, erosion_size ) );
    dilate(erodeMat,dilateMat,element);
    namedWindow("Dilated Mat", 1);
    imshow("Dilated Mat",dilateMat);
}

void MainWindow::on_pb3_clicked()
{
    Mat src = dilateMat;
    int mi, ma, c =3, xx2=0;
    mi = ui->leMin->text().toInt();
    ma = ui->leMax->text().toInt();

    Mat labels;
    Mat stats;
    Mat centroids;
    Scalar colors[c];

    Mat image;
    bitwise_not(src,image);
    Mat image_th;
    Mat in[] = {src, src, src};
    merge(in, 3, image_th);
    cv::connectedComponentsWithStats(image, labels, stats, centroids);

    //std::cout << labels << std::endl;
    std::cout << "stats.size()=" << stats.size() << std::endl;
    //std::cout << centroids << std::endl;

    for (int i = 0; i < c; ++i) {
        colors[i] = Scalar( rand()&0xFF, rand()&0xFF, rand()&0xFF );
    }

    for(int i=0; i<stats.rows; i++)
    {
      int x = stats.at<int>(Point(0, i));
      int y = stats.at<int>(Point(1, i));
      int w = stats.at<int>(Point(2, i));
      int h = stats.at<int>(Point(3, i));
      //std::cout << "x=" << x << " y=" << y << " w=" << w << " h=" << h << " size=" << (w*h) << std::endl;

      if((w*h)<mi || (h*w)>ma){}
      else if ((x+(w/2))> 0 && (x+(w/2)) < ((inMat.cols/c)*1) ) {
          Rect rect(x,y,w,h);
          cv::rectangle(image_th, rect, colors[0], CV_CONTOURS_MATCH_I1);
          //qDebug() << "uni";
          xx2++;
      } else if ((x+(w/2))> ((inMat.cols/c)*1) && (x+(w/2)) < ((inMat.cols/c)*2) )
      {
          Rect rect(x,y,w,h);
          cv::rectangle(image_th, rect, colors[1], CV_CONTOURS_MATCH_I1);
          //qDebug() << "dos";
          xx2++;
      } else
      {
          Rect rect(x,y,w,h);
          cv::rectangle(image_th, rect, colors[2], CV_CONTOURS_MATCH_I1);
          xx2++;
      }
      namedWindow("Objetos", WINDOW_AUTOSIZE );
      imshow("Objetos", image_th);
      waitKey(1);
    }

    /*for (int contour_number=0; contour_number < stats.rows; contour_number++)
    {
        //drawContours(image_th, contours, contour_number, colour, CV_FILLED, 8, hierarchy );
    }*/
    qDebug() << "-----------------------------------------------------------------";
    qDebug() << xx2 << " Objetox";
        //
    //clasification(dilateMat, 3);
}

void MainWindow::on_pb4_clicked()
{
    Mat src = dilateMat;
    int mi, ma, c =4, xx2=0;
    mi = ui->leMin->text().toInt();
    ma = ui->leMax->text().toInt();
    Mat labels;
    Mat stats;
    Mat centroids;
    Scalar colors[c];

    Mat image;
    bitwise_not(src,image);
    Mat image_th;
    Mat in[] = {src, src, src};
    merge(in, 3, image_th);
    cv::connectedComponentsWithStats(image, labels, stats, centroids);

    //std::cout << labels << std::endl;
    std::cout << "stats.size()=" << stats.size() << std::endl;
    //std::cout << centroids << std::endl;

    for (int i = 0; i < c; ++i) {
        colors[i] = Scalar( rand()&0xFF, rand()&0xFF, rand()&0xFF );
    }

    for(int i=0; i<stats.rows; i++)
    {
      int x = stats.at<int>(Point(0, i));
      int y = stats.at<int>(Point(1, i));
      int w = stats.at<int>(Point(2, i));
      int h = stats.at<int>(Point(3, i));
      //std::cout << "x=" << x << " y=" << y << " w=" << w << " h=" << h << " size=" << (w*h) << std::endl;

      if((w*h) > ma || (h*w) < mi){}
      else if ((x+(w/2))> 0 && (x+(w/2)) < ((inMat.cols/c)*1) ) {
          Rect rect(x,y,w,h);
          cv::rectangle(image_th, rect, colors[0], CV_CONTOURS_MATCH_I1);
          //qDebug() << "uni";
          xx2++;
      } else if ((x+(w/2))> ((inMat.cols/c)*1) && (x+(w/2)) < ((inMat.cols/c)*2) )
      {
          Rect rect(x,y,w,h);
          cv::rectangle(image_th, rect, colors[1], CV_CONTOURS_MATCH_I1);
          //qDebug() << "dos";
          xx2++;
      } else if ((x+(w/2))> ((inMat.cols/c)*2) && (x+(w/2)) < ((inMat.cols/c)*3) )
      {
          Rect rect(x,y,w,h);
          cv::rectangle(image_th, rect, colors[2], CV_CONTOURS_MATCH_I1);
          //qDebug() << "tres";
          xx2++;
      } else
      {
          Rect rect(x,y,w,h);
          cv::rectangle(image_th, rect, colors[3], CV_CONTOURS_MATCH_I1);
          xx2++;
      }

      namedWindow("Objetos", WINDOW_AUTOSIZE );
      imshow("Objetos", image_th);
      waitKey(1);
    }

    for (int contour_number=0; contour_number < stats.rows; contour_number++)
    {
        //drawContours(image_th, contours, contour_number, colour, CV_FILLED, 8, hierarchy );
    }
    qDebug() << "-----------------------------------------------------------------";
    qDebug() << xx2 << " Objetoz";
        //
}

// comparison function object


void MainWindow::clasification(Mat src, int c)
{
    src = dilateMat;
    int mi, ma, x =0;
    mi = ui->leMin->text().toInt();
    ma = ui->leMax->text().toInt();
    Scalar colors[c+1];
    for (int i = 0; i < c; ++i) {
        colors[i] = Scalar( rand()&0xFF, rand()&0xFF, rand()&0xFF );
    }

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat opened_image_copy = src.clone();
    findContours(opened_image_copy,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_NONE);
    Mat contours_image = Mat::zeros(src.size(), CV_8UC3);

    // sort contours
    std::sort(contours.begin(), contours.end(), compareContourAreas);
    for (int contour_number=0; (contour_number<(int)contours.size()); contour_number++)
    {
        if(fabs( contourArea(contours[contour_number]) ) > mi && fabs( contourArea(contours[contour_number]) ) < ma) {
            //Scalar colour( rand()&0xFF, rand()&0xFF, rand()&0xFF );
            if(contour_number >= 0 && contour_number <= (contours.size()/c)*1)
                drawContours( contours_image, contours, contour_number, colors[0], CV_CONTOURS_MATCH_I1, 8, hierarchy );
            else if(contour_number > (contours.size()/c)*1 && contour_number <= (contours.size()/c)*2)
                drawContours( contours_image, contours, contour_number, colors[1], CV_FILLED, 8, hierarchy );
            else if(contour_number > (contours.size()/c)*2)
                drawContours( contours_image, contours, contour_number, colors[2], CV_FILLED, 8, hierarchy );
            x++;
            namedWindow("Objetos", WINDOW_AUTOSIZE );
            imshow("Objetos", contours_image);
            waitKey(100);
        }
    }
    qDebug() << "-----------------------------------------------------------------";

    qDebug() << x << " Objetos";
}
