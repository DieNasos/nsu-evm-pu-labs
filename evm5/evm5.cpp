#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

using namespace std;

int main()
{
	// Создается поток ввода видеоданных с первой камеры (нумерация
	// начинается с нуля). Результат операции – указатель на дескриптор
	// обрабатываемого потока видеоданных

	CvCapture *capture = cvCreateCameraCapture(0);
	if (!capture)
		return 0;

	int framesCounter = 0;
	int inputSum = 0;
	int renderSum = 0;
	int outputSum = 0;
	int timeSum = 0;

	/* Запускается циклическая обработка потока видеоданных */

	while (1)
	{
		auto beginTime = chrono::high_resolution_clock::now();

		/* На каждой итерации извлекается очередной кадр (изображение) из
		открытого потока ввода видеоданных. Результат операции – указатель на
		дескриптор растрового изображения, которое будет содержать один кадр из
		потока видеоданных */

		IplImage *frame = cvQueryFrame(capture);
		if (!frame)
			break;

		IplImage *image = cvCloneImage(frame);
		cvSmooth(frame, image, CV_BLUR, 3, 3);

		auto inputTime = chrono::high_resolution_clock::now();

		CvPoint2D32f srcTri[3], dstTri[3];
		// матрицы трансформации
		CvMat* rot_mat = cvCreateMat(2, 3, CV_32FC1);
		CvMat* warp_mat = cvCreateMat(2, 3, CV_32FC1);

		//
		// трансформация по точкам
		//

		// по заданным точкам 
		// вычислим матрицу преобразования
		srcTri[0].x = 0;          //src Top left
		srcTri[0].y = 0;
		srcTri[1].x = frame->width - 1;    //src Top right
		srcTri[1].y = 0;
		srcTri[2].x = 0;          //src Bottom left
		srcTri[2].y = frame->height - 1;
		//- - - - - - - - - - - - - - -//
		dstTri[0].x = frame->width*0.0;    //dst Top left
		dstTri[0].y = frame->height*0.33;
		dstTri[1].x = frame->width*0.85; //dst Top right
		dstTri[1].y = frame->height*0.25;
		dstTri[2].x = frame->width*0.15; //dst Bottom left
		dstTri[2].y = frame->height*0.7;
		// получаем матрицу
		cvGetAffineTransform(srcTri, dstTri, warp_mat);
		// выполняем трансформацию
		cvWarpAffine(frame, image, warp_mat);

		//
		// поворот изображения
		//

		// рассчёт матрицы вращения
		CvPoint2D32f center = cvPoint2D32f(frame->width / 2, frame->height / 2);
		double angle = -180.0;   // на 180 градусов по часовой стрелке
		double scale = 3.0;             // масштаб
		cv2DRotationMatrix(center, angle, scale, rot_mat);

		// выполняем вращение
		cvWarpAffine(frame, image, rot_mat);

		for (int y = 0; y < image->height; y++)
		{
			uchar *ptr = (uchar*)(image->imageData + y * image->widthStep);
			for (int x = 0; x < image->width; x++)
			{
				// ptr[3 * x + 2] = 0; // Red
				ptr[3 * x + 1] = 0;	// Green
				ptr[3 * x] = 0; // Blue
			}
		}

		auto renderTime = chrono::high_resolution_clock::now();

		/* Текущий кадр выводится в окно с именем test (при первом вызове
		создается окно с таким именем) */

		cvShowImage("test", image);

		auto outputTime = chrono::high_resolution_clock::now();

		framesCounter++;
		inputSum +=
			chrono::duration_cast<chrono::milliseconds>(inputTime - beginTime).count();
		renderSum +=
			chrono::duration_cast<chrono::milliseconds>(renderTime - inputTime).count();
		outputSum +=
			chrono::duration_cast<chrono::milliseconds>(outputTime - renderTime).count();
		timeSum +=
			chrono::duration_cast<chrono::milliseconds>(outputTime - beginTime).count();

		/* В течение 33 миллисекунд ожидается нажатие клавиши пользователем
		(если клавиша не была нажата, вызов срабатывает как задержка на 33 мс, что
		обеспечивает частоту показа примерно 30 кадров в сек) */

		char c = cvWaitKey(33);

		/* Если пользователь нажал клавишу Esc, выйти из цикла обработки и показа
		кадров видеопотока */

		if (c == 27)
			break;

		/* Перейти к следующей итерации цикла */
	}

	/* В эту точку программы можно попасть в двух случаях: во-первых, когда в
	видеопотоке больше нет кадров(конец видео); во-вторых, когда пользователь
	нажал клавишу Esc. Здесь производится удаление потока ввода видеоданных,
	освобождение занятых им ресурсов, а также удаление окна, в которое
	выводились кадры потока видеоданных */

	cvReleaseCapture(&capture);
	cvDestroyWindow("test");

	cout << "FPS: " << framesCounter * 1000 / timeSum << endl;
	cout << "INPUT: " <<
		(float) inputSum / framesCounter << " ms, " <<
		(float) inputSum / timeSum * 100 << "%" << endl;
	cout << "RENDER: " <<
		(float) renderSum / framesCounter << " ms, " <<
		(float) renderSum / timeSum * 100 << "%" << endl;
	cout << "OUTPUT: " <<
		(float) outputSum / framesCounter << " ms, " <<
		(float) outputSum / timeSum * 100 <<  "%"<< endl;

	system("pause");

	return 0;
}