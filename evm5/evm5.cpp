#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

using namespace std;

int main()
{
	// ��������� ����� ����� ����������� � ������ ������ (���������
	// ���������� � ����). ��������� �������� � ��������� �� ����������
	// ��������������� ������ �����������

	CvCapture *capture = cvCreateCameraCapture(0);
	if (!capture)
		return 0;

	int framesCounter = 0;
	int inputSum = 0;
	int renderSum = 0;
	int outputSum = 0;
	int timeSum = 0;

	/* ����������� ����������� ��������� ������ ����������� */

	while (1)
	{
		auto beginTime = chrono::high_resolution_clock::now();

		/* �� ������ �������� ����������� ��������� ���� (�����������) ��
		��������� ������ ����� �����������. ��������� �������� � ��������� ��
		���������� ���������� �����������, ������� ����� ��������� ���� ���� ��
		������ ����������� */

		IplImage *frame = cvQueryFrame(capture);
		if (!frame)
			break;

		IplImage *image = cvCloneImage(frame);
		cvSmooth(frame, image, CV_BLUR, 3, 3);

		auto inputTime = chrono::high_resolution_clock::now();

		CvPoint2D32f srcTri[3], dstTri[3];
		// ������� �������������
		CvMat* rot_mat = cvCreateMat(2, 3, CV_32FC1);
		CvMat* warp_mat = cvCreateMat(2, 3, CV_32FC1);

		//
		// ������������� �� ������
		//

		// �� �������� ������ 
		// �������� ������� ��������������
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
		// �������� �������
		cvGetAffineTransform(srcTri, dstTri, warp_mat);
		// ��������� �������������
		cvWarpAffine(frame, image, warp_mat);

		//
		// ������� �����������
		//

		// ������� ������� ��������
		CvPoint2D32f center = cvPoint2D32f(frame->width / 2, frame->height / 2);
		double angle = -180.0;   // �� 180 �������� �� ������� �������
		double scale = 3.0;             // �������
		cv2DRotationMatrix(center, angle, scale, rot_mat);

		// ��������� ��������
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

		/* ������� ���� ��������� � ���� � ������ test (��� ������ ������
		��������� ���� � ����� ������) */

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

		/* � ������� 33 ����������� ��������� ������� ������� �������������
		(���� ������� �� ���� ������, ����� ����������� ��� �������� �� 33 ��, ���
		������������ ������� ������ �������� 30 ������ � ���) */

		char c = cvWaitKey(33);

		/* ���� ������������ ����� ������� Esc, ����� �� ����� ��������� � ������
		������ ����������� */

		if (c == 27)
			break;

		/* ������� � ��������� �������� ����� */
	}

	/* � ��� ����� ��������� ����� ������� � ���� �������: ��-������, ����� �
	����������� ������ ��� ������(����� �����); ��-������, ����� ������������
	����� ������� Esc. ����� ������������ �������� ������ ����� �����������,
	������������ ������� �� ��������, � ����� �������� ����, � �������
	���������� ����� ������ ����������� */

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