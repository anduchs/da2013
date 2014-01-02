/* Compile via: 
   gcc -std=gnu99 -g da2013.c $(pkg-config --cflags --libs libusb-1.0)
 */
 
#include <libusb-1.0/libusb.h>
#include <stdio.h>
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
	struct libusb_context *libusb_ctx;
	struct libusb_device **devlist, *dev;
	ssize_t devcount;
	struct libusb_device_descriptor devdesc;
	struct libusb_device_handle *devhandle = NULL;
	int r;
	int buff_len = 90;
	uint8_t buff[buff_len];
	memset(buff, 0, buff_len);
	
	printf("Welcome to deathadder2013 dpi switcher.\n");
	
	r = libusb_init(&libusb_ctx);
	libusb_set_debug(libusb_ctx, 3);
	
	//Let's find our device
	devcount = libusb_get_device_list(libusb_ctx, &devlist);
	if (devcount < 0) {
		fprintf(stderr, "libusb_get_device_list: Failed to get device list (%zd)\n", devcount);
		return 1;
	}
	for (int i = 0; i < devcount; i++) {
		dev = devlist[i];
		r = libusb_get_device_descriptor(dev, &devdesc);
		if (r != 0) {
			fprintf(stderr, "libusb_get_device_descriptor: Failed to get descriptor (%d)\n", r);
			continue;
		}
		fprintf(stderr, "Found: %04x:%04x.\n", devdesc.idVendor, devdesc.idProduct);
		if (devdesc.idVendor == 0x1532 && devdesc.idProduct == 0x0037) {
			r = libusb_open(dev, &devhandle);
			if (r != 0) {
				fprintf(stderr, "libusb_open: Failed to open device (%d)\n", r);
				break;
			}
			break;
		}
	}
	libusb_free_device_list(devlist, true);
	if (devhandle == NULL) {
		fprintf(stderr, "No device found.\n");
		goto exit;
	}
	
	fprintf(stderr, "Found Device %04x:%04x.\n", devdesc.idVendor, devdesc.idProduct);
	//Let's use our device

	r = libusb_detach_kernel_driver(devhandle, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_detach_kernel_driver: Failed (%d)\n", r);
		goto close;
	}
	r = libusb_claim_interface(devhandle, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_claim_interface: Failed (%d)\n", r);
		goto close;
	}

	
/*	memset(buff, 0, buff_len);
	//Static header:
	buff[1] = 0x78;
	buff[5] = 0x03;
	//Command (Unknown):
	buff[6] = 0x0b;
	buff[7] = 0x03;
	//Content (Unknown):
	buff[8] = 0x00;
	buff[9] = 0x04;
	//Footer:
	buff[88] = 0x0f;
	fprintf(stderr, "OUT: ");
	printbuff(buff, buff_len);
	r = libusb_control_transfer(devhandle,
			LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
			LIBUSB_REQUEST_SET_CONFIGURATION, 0x300, 0, buff, buff_len, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_control_transfer: Failed to call device (%d)\n", r);
		goto release;
	}



	
	memset(buff, 0, buff_len);
	r = libusb_control_transfer(devhandle,
			LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
			LIBUSB_REQUEST_CLEAR_FEATURE, 0x300, 0, buff, buff_len, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_control_transfer: Failed to call device (%d)\n", r);
		goto release;
	}
	fprintf(stderr, "IN: ");
	printbuff(buff, buff_len);
	
*/


	
	memset(buff, 0, buff_len);
	//Static header:
	buff[1] = 0x78;
	buff[5] = 0x03;
	//Command (LED):
	buff[6] = 0x03;
	buff[7] = 0x00;
	//LED-Static:
	buff[8] = 0x01;
	//LED-Choice:
	buff[9] = 0x01;
	//LED-Status:
	buff[10] = 0x00;
	//Footer:
	buff[88] = 0x00;
	fprintf(stderr, "OUT: ");
	printbuff(buff, buff_len);
	r = libusb_control_transfer(devhandle,
			LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
			LIBUSB_REQUEST_SET_CONFIGURATION, 0x300, 0, buff, buff_len, 0);
	if (r != buff_len) {
		fprintf(stderr, "libusb_control_transfer: Failed to call device (%d)\n", r);
		goto release;
	}

	sleep(2);

	memset(buff, 0, buff_len);
	//Static header:
	buff[1] = 0x78;
	buff[5] = 0x03;
	//Command (LED):
	buff[6] = 0x03;
	buff[7] = 0x00;
	//LED-Static:
	buff[8] = 0x01;
	//LED-Choice:
	buff[9] = 0x01;
	//LED-Status:
	buff[10] = 0x01;
	//Footer:
	buff[88] = 0x01;
	fprintf(stderr, "OUT: ");
	printbuff(buff, buff_len);
	r = libusb_control_transfer(devhandle,
			LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
			LIBUSB_REQUEST_SET_CONFIGURATION, 0x300, 0, buff, buff_len, 0);
	if (r != buff_len) {
		fprintf(stderr, "libusb_control_transfer: Failed to call device (%d)\n", r);
		goto release;
	}



/*

	memset(buff, 0, buff_len);
	r = libusb_control_transfer(devhandle,
			LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
			LIBUSB_REQUEST_CLEAR_FEATURE, 0x300, 0, buff, buff_len, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_control_transfer: Failed to call device (%d)\n", r);
		goto release;
	}
	fprintf(stderr, "IN: ");
	printbuff(buff, buff_len);
*/	




	release:
	r = libusb_release_interface(devhandle, 0);
	if (r != 0) {
		fprintf(stderr, "libusb_release_interface: Failed (%d)\n", r);
		goto close;
	}
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
