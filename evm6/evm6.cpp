#include <iostream>
#include "libusb-1.0.22/include/libusb-1.0/libusb.h"
#include <stdio.h>

using namespace std;

void printfInfo(libusb_device *dev)
{
	libusb_device_descriptor desc; // дескриптор устройства
	const int serial_number_size = 256;
	unsigned char serial_number[serial_number_size];
	libusb_device_handle* handle = NULL;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0)
	{
		printf("ОШИБКА!\nФУНКЦИЯ: libusb_get_device_descriptor\nКОД ОШИБКИ: %d\n", r);
		return;
	}
	r = libusb_open(dev, &handle);
	if (r != LIBUSB_SUCCESS) {
		fprintf(stderr,
			"ОШИБКА!\nФУНКЦИЯ: libusb_open\nКОД ОШИБКИ: %d\n", r);
		return;
	}
	//получить серийный номер устройства
	if (handle != NULL && desc.iSerialNumber > 0) {
		r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial_number, serial_number_size);
		if (r < 0) {
			fprintf(stderr,
				"ОШИБКА!\nФУНКЦИЯ: libusb_get_string_descriptor_ascii\nКОД ОШИБКИ: %d\n", r);
			return;
		}
	}
	printf("ПЕЧАТЬ ИНФОРМАЦИИ ОБ УСТРОЙСТВЕ:\n");
	printf("КЛАСС УСТРОЙСТВА: %d - ", (int)desc.bDeviceClass);
	switch((int)desc.bDeviceClass)
	{
	case 1:
		printf("аудиоустройство\n");
		break;
	case 2:
		printf("коммуникационное устройство (сетевой адаптер)\n");
		break;
	case 3:
		printf("устройство пользовательского интерфейса\n");
		break;
	case 5:
		printf("физическое устройство\n");
		break;
	case 6:
		printf("изображения\n");
		break;
	case 7:
		printf("принтер\n");
		break;
	case 8:
		printf("устройство хранения данных\n");
		break;
	case 9:
		printf("концентратор\n");
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
		printf("видеоустройство\n");
		break;
	case 15:
		printf("персональное медицинское устройство\n");
		break;
	case 16:
		printf("аудио- и видеоустройства\n");
		break;
	case 220:
		printf("диагностическое устройство\n");
		break;
	case 224:
		printf("беспроводный контроллер\n");
		break;
	case 239:
		printf("различные устройства\n");
		break;
	case 254:
		printf("специфическое устройство\n");
		break;
	default:
		printf("это что (what)???\n");
		break;
	};
	printf("ИДЕНТИФИКАТОР ПРОИЗВОДИТЕЛЯ: %d\n", desc.idVendor);
	printf("ИДЕНТИФИКАТОР ИЗДЕЛИЯ: %d\n", desc.idProduct);
	printf("СЕРИЙНЫЙ НОМЕР: ");
	if (desc.iSerialNumber == 0)
		printf("нет сведений\n");
	else
		printf("%s ", serial_number);
	printf("__________________________________\n");
	libusb_close(handle);
	if (r < 0)
	{
		printf("ОШИБКА!\nФУНКЦИЯ: libusb_close\nКОД ОШИБКИ: %d\n", r);
		return;
	}
	return;
}

int main() {

	setlocale(LC_ALL, "Russian");

	libusb_device **devs; // указатель на указатель на устройство,
	// используется для получения списка устройств
	libusb_context *ctx = NULL; // контекст сессии libusb
	int r; // для возвращаемых значений
	ssize_t cnt; // число найденных USB-устройств
	ssize_t i; // индексная переменная цикла перебора всех устройств
	// инициализировать библиотеку libusb, открыть сессию работы с libusb
	r = libusb_init(&ctx);
	if (r < 0) {
		fprintf(stderr,
			"Ошибка: инициализация не выполнена, код: %d.\n", r);
		return 1;
	}
	// задать уровень подробности отладочных сообщений
	libusb_set_debug(ctx, 3);
	// получить список всех найденных USB- устройств
	cnt = libusb_get_device_list(ctx, &devs);
	if (cnt < 0) {
		fprintf(stderr,
			"Ошибка: список USB устройств не получен.\n", r);
		return 1;
	}
	printf("НАЙДЕНО УСТРОЙСТВ: %d\n", cnt);
	printf("__________________________________\n");
	for (i = 0; i < cnt; i++) { // цикл перебора всех устройств
		printf("УСТРОЙСТВО %d:\n", i+1);
		printfInfo(devs[i]); // печать параметров устройства
	}
	// освободить память, выделенную функцией получения списка устройств
	libusb_free_device_list(devs, 1);
	libusb_exit(ctx); // завершить работу с библиотекой libusb,
	// закрыть сессию работы с libusb
	return 0;
}