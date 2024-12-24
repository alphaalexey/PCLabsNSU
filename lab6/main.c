#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libusb.h>

#define BUFFER_SIZE 512

const char *device_class_to_str(uint8_t device_class) {
    switch (device_class) {
    case 0x00:
        return "(Defined at Interface level)";
    case 0x01:
        return "Audio";
    case 0x02:
        return "Communications";
    case 0x03:
        return "Human Interface Device";
    case 0x05:
        return "Physical Interface Device";
    case 0x06:
        return "Imaging";
    case 0x07:
        return "Printer";
    case 0x08:
        return "Mass Storage";
    case 0x09:
        return "Hub";
    case 0x0a:
        return "CDC Data";
    case 0x0b:
        return "Chip/SmartCard";
    case 0x0d:
        return "Content Security";
    case 0x0e:
        return "Video";
    case 0x0f:
        return "Personal Healthcare";
    case 0x10:
        return "Audio/Video";
    case 0x11:
        return "Billboard";
    case 0x12:
        return "Type-C Bridge";
    case 0x13:
        return "Bulk Display";
    case 0x14:
        return "MCTCP over USB";
    case 0x3c:
        return "I3C";
    case 0x58:
        return "Xbox";
    case 0xdc:
        return "Diagnostic";
    case 0xe0:
        return "Wireless";
    case 0xef:
        return "Miscellaneous Device";
    case 0xfe:
        return "Application Specific Interface";
    case 0xff:
        return "Vendor Specific Class";

    default:
        return "UNKNOWN";
    }
}

void print_dev(libusb_device *dev) {
    struct libusb_device_descriptor desc; // дескриптор устройства
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
        fprintf(stderr, "Ошибка[%d]: дескриптор устройства не получен.\n", r);
        return;
    }

    printf("USB version %x\n", desc.bcdUSB);
    printf("idVendor: %x\n", desc.idVendor);
    printf("idProduct: %x\n", desc.idProduct);
    printf("bDeviceClass: %s (%x)\n", device_class_to_str(desc.bDeviceClass),
           desc.bDeviceClass);

    libusb_device_handle *handle;
    r = libusb_open(dev, &handle);
    if (r != 0) {
        fprintf(stderr, "Ошибка[%d]: не удалось открыть устройство.\n", r);
        return;
    }

    uint8_t *buffer = malloc(BUFFER_SIZE * sizeof(buffer[0]));
    r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buffer,
                                           BUFFER_SIZE);
    if (r > 0) {
        printf("Серийный номер: %s (%.2d)\n", buffer, desc.iSerialNumber);
    }
    r = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buffer,
                                           BUFFER_SIZE);
    if (r > 0) {
        printf("Производитель: %s (%.2d)\n", buffer, desc.iManufacturer);
    }
    r = libusb_get_string_descriptor_ascii(handle, desc.iProduct, buffer,
                                           BUFFER_SIZE);
    if (r > 0) {
        printf("Устройство: %s (%.2d)\n", buffer, desc.iProduct);
    }
    free(buffer);

    struct libusb_config_descriptor *config; // дескриптор конфигурации объекта
    // получить конфигурацию устройства
    r = libusb_get_config_descriptor(dev, 0, &config);
    if (r != 0) {
        fprintf(stderr,
                "Ошибка[%d]: Не удалось получить конфигурацию устройства.", r);
        goto quit;
    }

    printf("bNumInterfaces: %.2d\n", config->bNumInterfaces);
    for (uint8_t i = 0; i < config->bNumInterfaces; i++) {
        const struct libusb_interface *inter = &config->interface[i];
        printf("-   num_altsetting: %.2d\n", inter->num_altsetting);
        for (int j = 0; j < inter->num_altsetting; j++) {
            const struct libusb_interface_descriptor *interdesc =
                &inter->altsetting[j];
            printf("-   -   bInterfaceClass: %s (%x)\n",
                   device_class_to_str(desc.bDeviceClass), desc.bDeviceClass);
            printf("-   -   bInterfaceNumber: %.2d, bNumEndpoints: %.2d\n",
                   interdesc->bInterfaceNumber, interdesc->bNumEndpoints);
            for (uint8_t k = 0; k < interdesc->bNumEndpoints; k++) {
                const struct libusb_endpoint_descriptor *epdesc =
                    &interdesc->endpoint[k];
                printf("-   -   -   bDescriptorType: %.2d, bEndpointAddress: "
                       "%.9d\n",
                       epdesc->bDescriptorType, epdesc->bEndpointAddress);
            }
        }
    }

    libusb_free_config_descriptor(config);

quit:
    libusb_close(handle);
}

const char *line =
    "===========================================================";

int main(int argc, char *argv[]) {
    // инициализировать библиотеку libusb, открыть сессию работы с libusb
    libusb_context *ctx = NULL; // контекст сессии libusb
    int r = libusb_init(&ctx);
    if (r < 0) {
        fprintf(stderr, "Ошибка: инициализация не выполнена, код: %d.\n", r);
        return EXIT_FAILURE;
    }
    // задать уровень подробности отладочных сообщений
    libusb_set_debug(ctx, 3);

    // получить список всех найденных USB-устройств
    libusb_device **devs;
    size_t cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        fprintf(stderr, "Ошибка[%d]: список USB устройств не получен.\n", r);
        return EXIT_FAILURE;
    }

    printf("Найдено устройств: %zd\n", cnt);
    puts(line);
    for (size_t i = 0; i < cnt; i++) { // цикл перебора всех устройств
        print_dev(devs[i]); // печать параметров устройства
        puts(line);
    }

    // освободить память, выделенную функцией получения списка устройств
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx); // завершить работу с библиотекой libusb,
    // закрыть сессию работы с libusb
    return EXIT_SUCCESS;
}
