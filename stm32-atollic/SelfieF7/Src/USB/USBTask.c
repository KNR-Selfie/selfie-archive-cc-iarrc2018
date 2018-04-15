/*
 * USBTask.c
 *
 *  Created on: 14.04.2018
 *      Author: mice
 */

#include "main.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "usbd_cdc_if.h"
#include "string.h"
#include "stdbool.h"

extern void MX_USB_DEVICE_Init(void);

xSemaphoreHandle USBSemaphore = NULL;
extern osThreadId USBTaskHandle;



uint32_t USB_linepos = 0;


extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t usbTxBuffer[256];
uint8_t usbDATA[256];
uint32_t len;
uint8_t able_to_receive = 0;

void StartUSBTask(void const * argument) {
	vSemaphoreCreateBinary(USBSemaphore);
	osSemaphoreWait(USBSemaphore, osWaitForever);

	/* init code for USB_DEVICE */
	MX_USB_DEVICE_Init();

	while (1) {
		if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {

			static uint16_t cnt00 = 0;
			if (cnt00++ > 9) {
				HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
				cnt00 = 0;
			}

		}
		osDelay(10);
	}
}
int8_t MAIN_USB_Receive(uint8_t* Buf, uint32_t *Len) {
	uint8_t startbyte = 0, endbyte = 0;
	uint8_t code = 0;
	uint8_t length = 0;
	/* start -> code -> length -> DATA[length] -> length -> code -> stop */
	/* 6 + length */
	if ((*Len) > 6) {
		startbyte = Buf[0];
		code = Buf[1];
		length = Buf[2];

		for (uint8_t index = 0; index < length; index++) {
			usbDATA[index] = Buf[3 + index];
		}
		endbyte = Buf[5 + length];

		switch (code) {
		case 0x64: {
			uint8_t is_left = usbDATA[0];
			if (is_left)
				usbTxBuffer[3] = 0;
			else
				usbTxBuffer[3] = 1;

			break;
		}
		case 0x65: {
			uint8_t is_right = usbDATA[0];
			if (is_right)
				usbTxBuffer[3] = 0;
			else
				usbTxBuffer[3] = 1;
			break;
		}
		case 0x66: {
			USB_linepos = usbDATA[0] << 24 | usbDATA[1] << 16 | usbDATA[2] << 8 | usbDATA[3];
			uint32_t line_pos = USB_linepos + 1;
			usbTxBuffer[3] = (uint8_t) (line_pos >> 24 & 0xff);
			usbTxBuffer[4] = (uint8_t) (line_pos >> 16 & 0xff);
			usbTxBuffer[5] = (uint8_t) (line_pos >> 8 & 0xff);
			usbTxBuffer[6] = (uint8_t) (line_pos & 0xff);
			break;
		}
		case 0x67: {
			float left_angle = usbDATA[0] << 24 | usbDATA[1] << 16
					| usbDATA[2] << 8 | usbDATA[3];
			left_angle -= 0.15f;
			uint8_t* array;
			array = (uint8_t*)(&left_angle);

			usbTxBuffer[3] = array[0];
			usbTxBuffer[4] = array[1];
			usbTxBuffer[5] = array[2];
			usbTxBuffer[6] = array[3];
			break;
		}
		case 0x68: {
			float right_angle = usbDATA[0] << 24 | usbDATA[1] << 16
					| usbDATA[2] << 8 | usbDATA[3];
			right_angle += 0.15f;
			uint8_t* array;
			array = (uint8_t*) (&right_angle);

			usbTxBuffer[3] = array[0];
			usbTxBuffer[4] = array[1];
			usbTxBuffer[5] = array[2];
			usbTxBuffer[6] = array[3];
			break;
		}
		case 0xc8: {
			uint8_t do_reset = usbDATA[0];
			if (do_reset)
				NVIC_SystemReset();
			break;
		}
		}

		usbTxBuffer[0] = startbyte;
		usbTxBuffer[1] = code;
		usbTxBuffer[2] = length;

		usbTxBuffer[3 + length] = length;
		usbTxBuffer[4 + length] = code;
		usbTxBuffer[5 + length] = endbyte;

		CDC_Transmit_FS(usbTxBuffer, 6 + length);
		}

	return 0;
}
