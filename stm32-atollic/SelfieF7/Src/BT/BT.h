/*
 * BT.h
 *
 *  Created on: 19.12.2017
 *      Author: Michal
 */

#ifndef BT_BT_H_
#define BT_BT_H_

void StartBTTask(void const * argument);
void BluetoothRx_Irq(void);
void BluetoothTx_Irq(void);

#endif /* BT_BT_H_ */
