/* Compile via: 
   gcc -std=gnu99 -g da2013.c $(pkg-config --cflags --libs libusb-1.0) -o da2013
 */
 
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>


void printbuff(uint8_t *buff, int len) {
	for (int i = 0; i < len; i++) {
		fprintf(stderr, "%02x", buff[i]);
	}
	fprintf(stderr, "\n");
}


int main(int argc, char **argv) {
	if (argc != 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		printf("Usage: %s <1..255>\n", argv[0]);
		return 1;
	}
	int sen = atoi(argv[1]);
	if (sen <= 0 || sen > 255) {
		printf("Wrong parameter.\n");
		return 1;
	}
	
	struct libusb_context *libusb_ctx;
	struct libusb_device_handle *devhandle = NULL;
	int r;
	int buff_len = 90;
	uint8_t buff[buff_len];
	memset(buff, 0, buff_len);
	
	printf("Welcome to deathadder2013 dpi switcher.\n");
	
	r = libusb_init(&libusb_ctx);
	libusb_set_debug(libusb_ctx, 3);
	
	//Let's find our device
	devhandle = libusb_open_device_with_vid_pid(libusb_ctx, 0x1532, 0x0037);
	if (devhandle == NULL) {
		fprintf(stderr, "No device found.\n");
		goto exit;
	}

	//Let's use our device
	r = libusb_detach_kernel_driver(devhandle, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_detach_kernel_driver: Failed (%d)\n", r);
		goto close;
	}
	r = libusb_claim_interface(devhandle, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_claim_interface: Failed (%d)\n", r);
		goto detach;
	}

	memset(buff, 0, buff_len);
	//Static header:
	buff[1] = 0x78;
	buff[5] = 0x03;
	//Command (DPI):
	buff[6] = 0x04;
	buff[7] = 0x01;
	//Speed:
	//buff[8] = 0x10;
	//buff[9] = 0x10;
	buff[8] = sen;
	buff[9] = sen;
	//Footer:
	buff[88] = 0x06;
	fprintf(stderr, "OUT: ");
	printbuff(buff, buff_len);
	r = libusb_control_transfer(devhandle,
			LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
			LIBUSB_REQUEST_SET_CONFIGURATION, 0x300, 0, buff, buff_len, 0);
	if (r != buff_len) {
		fprintf(stderr, "libusb_control_transfer: Failed to call device (%d)\n", r);
		goto release;
	}

	release:
	r = libusb_release_interface(devhandle, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_release_interface: Failed (%d)\n", r);
		goto close;
	}
	
	detach:
	r = libusb_attach_kernel_driver(devhandle, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_attach_kernel_driver: Failed (%d)\n", r);
		goto close;
	}

	close:
	libusb_close(devhandle);
	
	exit:
	libusb_exit(libusb_ctx);
	printf("Good Bye.\n");
	return 0;
}
