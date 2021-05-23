#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

//  Эта функция используется для проверки непрерывности Mat
//  Возвращает TRUE - если непрерывный
//  FALSE - если не непрерывный
bool check_Cont_Mat(Mat mVar)
{
  if(mVar.isContinuous())
  {
    cout << "The Matrix data is continious" << endl;
    return true;
  }
  else
  {
    cout << "The Matrix data is NOT continious" << endl;
    return false;
  }
}

// Эта функция используется для считывания изображения в форму Mat
// Возвращает Mat 'mat'
Mat read_Input_ImageData(string filePath)
{
  Mat mat = imread(filePath,IMREAD_ANYDEPTH|IMREAD_ANYCOLOR);
  return mat;
}

// Эта функция используется для отображения свойств изображения
// Возвращает void
void display_Image_prop(Mat mVar)
{
  int d = mVar.depth();
    string var;
    switch(d)
    {
      case 0 :
      {
        var = "8-bit unsigned";
        break;
      }
      case 1 :
      {
        var = "8-bit signed";
        break;
      }
      case 2 :
      {
        var = "16-bit unsigned";
        break;
      }
      case 3 :
      {
        var = "16-bit signed";
        break;
      }
      case 4 :
      {
        var = "32-bit signed";
      }
      case 5 :
      {
        var = "32-bit floating-point";
      }
      case 6 :
      {
        var = "64-bit floating-point";
      }
      default :
      {
        var = "None of the case";
        break;
      }
    }
    cout << "The number of channels/depth for the image is : "<< var << " " << mVar.channels() << " channel" << endl;
    cout << "Rows = "<<mVar.rows<< " Columns = "<<mVar.cols<< endl;
}

// Эта функция используется для вычисления средней интенсивности пикселей
// Возвращает double 'api'
double avg_Pixel_Intensity(Mat m)
{
  double api,tot=0;
  double *arrayVar;
  arrayVar = new double[m.rows*m.cols];
  for(int i=0; i<m.rows; i++)
  {
    for(int j=0; j<m.cols; j++)
    {
      arrayVar[i+m.rows*j]=double(m.at<uchar>(i,j));
    }
  }
  for (int i=0;i<m.rows*m.cols;i++)
   {
    tot+= arrayVar[i];
   }
    api = tot/(m.rows*m.cols);
    cout << "The average pixel intensity of the image is "<< api <<endl;
  return api;
}

// Эта функция используется для генерации диагональной матрицы
// Возвращает Mat 'm'
Mat generate_Diag_Matrix(Mat W, Mat matObj)
{
  Mat m;
  m = matObj.zeros(W.rows,W.rows,CV_64FC1);
  for(int i=0;i<W.rows;i++)
  {
    m.at<double>(i,i)=W.at<double>(i);
  }
  return m;
}

int main(int argc, char *argv[])
{
  Mat myImage, myImage_gray, myImage_svd, W, U, VT, Wdiag, compressedImage;
  SVD svdObj;
  Mat matObj;
  Range rangeObj;
  bool checkCont, checkCont_gray;
  int rank;
  //double api, original_Api;

  // Чтение файла изображения
  myImage = read_Input_ImageData(argv[1]);

  //original_Api = avg_Pixel_Intensity(myImage_gray);
  // Преобразование изображения в серое
  cvtColor(myImage,myImage_gray,COLOR_RGB2GRAY);

  // проверка, загружены ли данные изображения или нет
  if(myImage.empty())
  {
    cout << "Loading the given image has FAILED !!!" << endl;
    return 0;
  }
  else
  {
    // Печать свойств изображений
    display_Image_prop(myImage);
    display_Image_prop(myImage_gray);

    // Проверка непрерывности данных изображения или нет
    checkCont = check_Cont_Mat(myImage);
    checkCont_gray = check_Cont_Mat(myImage_gray);
    if(checkCont && checkCont_gray)
    {
      // Изменение свойства изображения
      myImage_gray.convertTo(myImage_svd, CV_64F,1.0/255);

      // Вычисление SVD (разложенеи по сингулярным значениям)
      svdObj.compute(myImage_svd,W,U,VT);

      // Вычисление диагональной матрицы
      Wdiag =  generate_Diag_Matrix(W,matObj);
      rank = atoi(argv[2]);

      // Выбор конкретного диапазона значений
      Wdiag=Wdiag(Range(0,rank),Range(0,rank));
      U=U(rangeObj.all(),Range(0,rank));
      VT=VT(Range(0,rank),rangeObj.all());

      // Восстановление данных изображения
      compressedImage = U*Wdiag*VT;

      //api = avg_Pixel_Intensity(compressedImage);
      cout << "Properties of W : # of rows = "<< W.rows << " # of cols = "<< W.cols <<endl;
      cout << "Properties of U : # of rows = "<< U.rows << " # of cols = "<< U.cols <<endl;
      cout << "Properties of VT : # of rows = "<< VT.rows << " # of cols = "<< VT.cols <<endl;

      stringstream x1,x2;
      string sVar = "Reconstructed for N : ";
      string sVar1 = "ReconImg";
      x1 << sVar << rank;
      x2 << sVar1 << rank << ".jpeg";
      String disp = x1.str();
      String pDisp = x2.str();

      // Создание окна для отображения
      namedWindow("Original Image",WINDOW_AUTOSIZE);
      namedWindow("Input Gray Image",WINDOW_AUTOSIZE);
      namedWindow(disp,WINDOW_AUTOSIZE);

      // Отображение изображения в окне
      imshow("Original Image",myImage);
      imshow("Input Gray Image",myImage_gray);
      imshow(disp,compressedImage);

      // Сохраненеи изображения
      compressedImage.convertTo(compressedImage, CV_8UC3, 255.0);
      imwrite(pDisp,compressedImage);

      // Ожидает нажатия клавиши и убирает окно
      waitKey(-1);
      destroyWindow("Original Image");
      destroyWindow("Input Gray Image");
      destroyWindow(disp);
    }
    else
    {
      cout << "Failed to convert!!!" << endl;
      return 0;
    }
  }
  return 0;
}
