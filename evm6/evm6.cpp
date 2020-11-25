#include <iostream>
#include "libusb-1.0.22/include/libusb-1.0/libusb.h"
#include <stdio.h>

using namespace std;

void printfInfo(libusb_device *dev)
{
	libusb_device_descriptor desc; // ���������� ����������
	const int serial_number_size = 256;
	unsigned char serial_number[serial_number_size];
	libusb_device_handle* handle = NULL;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0)
	{
		printf("������!\n�������: libusb_get_device_descriptor\n��� ������: %d\n", r);
		return;
	}
	r = libusb_open(dev, &handle);
	if (r != LIBUSB_SUCCESS) {
		fprintf(stderr,
			"������!\n�������: libusb_open\n��� ������: %d\n", r);
		return;
	}
	//�������� �������� ����� ����������
	if (handle != NULL && desc.iSerialNumber > 0) {
		r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial_number, serial_number_size);
		if (r < 0) {
			fprintf(stderr,
				"������!\n�������: libusb_get_string_descriptor_ascii\n��� ������: %d\n", r);
			return;
		}
	}
	printf("������ ���������� �� ����������:\n");
	printf("����� ����������: %d - ", (int)desc.bDeviceClass);
	switch((int)desc.bDeviceClass)
	{
	case 1:
		printf("���������������\n");
		break;
	case 2:
		printf("���������������� ���������� (������� �������)\n");
		break;
	case 3:
		printf("���������� ����������������� ����������\n");
		break;
	case 5:
		printf("���������� ����������\n");
		break;
	case 6:
		printf("�����������\n");
		break;
	case 7:
		printf("�������\n");
		break;
	case 8:
		printf("���������� �������� ������\n");
		break;
	case 9:
		printf("������������\n");
		break;
	case 10:
		printf("CDC-Data\n");
		break;
	case 11:
		printf("Smart Card\n");
		break;
	case 13:
		printf("Content Security\n");
		break;
	case 14:
		printf("���������������\n");
		break;
	case 15:
		printf("������������ ����������� ����������\n");
		break;
	case 16:
		printf("�����- � ���������������\n");
		break;
	case 220:
		printf("��������������� ����������\n");
		break;
	case 224:
		printf("������������ ����������\n");
		break;
	case 239:
		printf("��������� ����������\n");
		break;
	case 254:
		printf("������������� ����������\n");
		break;
	default:
		printf("��� ��� (what)???\n");
		break;
	};
	printf("������������� �������������: %d\n", desc.idVendor);
	printf("������������� �������: %d\n", desc.idProduct);
	printf("�������� �����: ");
	if (desc.iSerialNumber == 0)
		printf("��� ��������\n");
	else
		printf("%s ", serial_number);
	printf("__________________________________\n");
	libusb_close(handle);
	if (r < 0)
	{
		printf("������!\n�������: libusb_close\n��� ������: %d\n", r);
		return;
	}
	return;
}

int main() {

	setlocale(LC_ALL, "Russian");

	libusb_device **devs; // ��������� �� ��������� �� ����������,
	// ������������ ��� ��������� ������ ���������
	libusb_context *ctx = NULL; // �������� ������ libusb
	int r; // ��� ������������ ��������
	ssize_t cnt; // ����� ��������� USB-���������
	ssize_t i; // ��������� ���������� ����� �������� ���� ���������
	// ���������������� ���������� libusb, ������� ������ ������ � libusb
	r = libusb_init(&ctx);
	if (r < 0) {
		fprintf(stderr,
			"������: ������������� �� ���������, ���: %d.\n", r);
		return 1;
	}
	// ������ ������� ����������� ���������� ���������
	libusb_set_debug(ctx, 3);
	// �������� ������ ���� ��������� USB- ���������
	cnt = libusb_get_device_list(ctx, &devs);
	if (cnt < 0) {
		fprintf(stderr,
			"������: ������ USB ��������� �� �������.\n", r);
		return 1;
	}
	printf("������� ���������: %d\n", cnt);
	printf("__________________________________\n");
	for (i = 0; i < cnt; i++) { // ���� �������� ���� ���������
		printf("���������� %d:\n", i+1);
		printfInfo(devs[i]); // ������ ���������� ����������
	}
	// ���������� ������, ���������� �������� ��������� ������ ���������
	libusb_free_device_list(devs, 1);
	libusb_exit(ctx); // ��������� ������ � ����������� libusb,
	// ������� ������ ������ � libusb
	return 0;
}