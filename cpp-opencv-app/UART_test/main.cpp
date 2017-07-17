#pragma pack(1)
#include <iostream>
#include <stdio.h>
#include <bitset>

using namespace std;

struct data_frame
{
	unsigned char sync_byte;
	unsigned short data_0 : 11;
	unsigned short data_1 : 11;
	unsigned short data_2 : 11;
	unsigned short data_3 : 11;
	unsigned short data_4 : 11;
	unsigned short data_5 : 11;
	unsigned short data_6 : 11;
	unsigned short data_7 : 11;
	unsigned char flags;
	unsigned char end_byte;
};

union frame
{
	data_frame dane;
	unsigned char data_to_send[18];
};

int main()
{
	frame unia;

	unia.dane.sync_byte = 0xff;
	unia.dane.data_0 = 10;
	unia.dane.data_1 = 11;
	unia.dane.data_2 = 12;
	unia.dane.data_3 = 13;
	unia.dane.data_4 = 14;
	unia.dane.data_5 = 15;
	unia.dane.data_6 = 16;
	unia.dane.data_7 = 17;
	unia.dane.flags = 0b00000000;
	unia.dane.end_byte = 0xff;

	//Show results
	for (int i = 0; i <= 18; i++)
	{
		cout << bitset<8>(unia.data_to_send[i]) << endl;
	}

	cout << endl << endl;

	cout << bitset<8>(unia.dane.sync_byte) << endl;
	cout << bitset<11>(unia.dane.data_0) << endl;
	cout << bitset<11>(unia.dane.data_1) << endl;
	cout << bitset<11>(unia.dane.data_2) << endl;
	cout << bitset<11>(unia.dane.data_3) << endl;
	cout << bitset<11>(unia.dane.data_4) << endl;
	cout << bitset<11>(unia.dane.data_5) << endl;
	cout << bitset<11>(unia.dane.data_6) << endl;
	cout << bitset<11>(unia.dane.data_7) << endl;
	cout << bitset<8>(unia.dane.flags) << endl;
	cout << bitset<8>(unia.dane.end_byte) << endl;

	system("pause");

	return 0;
}