#include "stdafx.h"
#include <iostream>

#include <stdio.h>
#include <conio.h>

#include <bitset>

//---------------------------------------------------------------------------
using namespace std;

char Tab_Z[8][16] =
{
	{ 4,10,9,2,13,8,0,14,6,11,1,12,7,15,5,3 },
	{ 14,11,4,12,6,13,15,10,2,3,8,1,0,7,5,9 },
	{ 5,8,1,13,10,3,4,2,14,15,12,7,6,0,9,11 },
	{ 7,13,10,1,0,8,9,15,14,4,6,12,11,2,5,3 },
	{ 6,12,7,1,5,15,13,8,4,10,9,14,0,3,11,2 },
	{ 4,11,10,0,7,2,1,13,3,6,8,5,9,12,15,14 },
	{ 13,11,4,1,3,15,5,9,0,10,14,7,6,8,2,12 },
	{ 1,15,13,0,5,7,10,4,9,2,3,14,6,11,8,12 }
};

char sync[8] = {1, 2, 3, 4, 5, 6, 7, 8};

unsigned long key[8] = { 1,	2, 3, 4, 5, 6, 7, 8 };


long filesize(FILE *);

int basic_step(int , int );

void coding(unsigned long &, unsigned long &);

void imitation(unsigned long &, unsigned long &);

int main()
{
	char input_file[255], output_file[255];
	char mode;
	FILE *f_begin, *f_end;
	char file_input[255];
	char file_output[255];

	setlocale(LC_ALL, "Russian");
	cout << "ГОСТ 28147-89 гамирование с обратной связью с выработкой имитовставки" << endl;
	cout << "---------------------------------------------------------------------" << endl;
	while (true) {
		cout << "Выберите режим кодирования или декодирования" << endl;
		cout << "1 - режим кодирования" << endl;
		cout << "2 - режим декодирования" << endl;
		cout << "3 - для выхода из программы" << endl;
		cin >> mode;
		if (mode == '3') {
			return 0;
		}

		unsigned long n1 = 0, n2 = 0, SUM232 = 0;
		unsigned long sync1 = 0, sync2 = 0;
		unsigned long imit1 = 0, imit2 = 0;
		unsigned long file_size = 0;
		unsigned long test_imit = 0;
		char buff[8];

		cout << "---------------------------------------------------------------------" << endl;
		switch (mode) {
		case '1':
			cout << "Введите имя файла, который будет закодирован" << endl;
			cin >> file_input;
			cout << "Введите имя файла, в который будет записан результат" << endl;
			cin >> file_output;
			f_begin = fopen(file_input, "rb");
			f_end = fopen(file_output, "wb");

			sync1 = *((unsigned long *)&sync[0]);
			sync2 = *((unsigned long *)&sync[4]);

			char buff[8];

			file_size = filesize(f_begin);

			for (int i = 0; i < file_size; i += 8) {
				coding(sync1, sync2);
				for (int k = 0; k < 8; k++) {
					buff[k] = false;
				}
				if (file_size - i >= 8) {
					fread(buff, 8, 1, f_begin);
				}
				else {
					fread(buff, file_size - i, 1, f_begin);
				}
				n1 = *((unsigned long *)&buff[0]);
				n2 = *((unsigned long *)&buff[4]);
				
				imit1 ^= n1;
				imit2 ^= n2;
				imitation(imit1, imit2);

				n1 ^= sync1;
				n2 ^= sync2;
				sync1 = n1;
				sync2 = n2;

				char sym_rez;
				if (file_size - i >= 8) {
					for (int q = 0; q < 4; q++)
					{
						sym_rez = *((char *)&n1 + q);
						fprintf(f_end, "%c", sym_rez);
					}
					for (int q = 0; q < 4; q++)
					{
						sym_rez = *((char *)&n2 + q);
						fprintf(f_end, "%c", sym_rez);
					}
				}
				else {
					int itt = file_size - i;
					for (int q = 0; q < 4; q++)
					{
						if (itt == 0) {
							break;
						}
						itt--;
						sym_rez = *((char *)&n1 + q);
						fprintf(f_end, "%c", sym_rez);
					}
					for (int q = 0; q < 4; q++)
					{
						if (itt == 0) {
							break;
						}
						itt--;
						sym_rez = *((char *)&n2 + q);
						fprintf(f_end, "%c", sym_rez);
					}
				}
			}
			for (int q = 0; q < 4; q++) {
				fprintf(f_end, "%c", *((char *)&imit1 + q));
			}
			cout << "---------------------------------------------------------------------" << endl;
			cout << "Кодирование выполнено успешно" << endl;
			cout << "---------------------------------------------------------------------" << endl;
			fclose(f_begin);
			fclose(f_end);
			break;

		case '2':
			cout << "Введите имя файла, который будет декодироваться" << endl;
			cin >> file_input;
			cout << "Введите имя файла, в который будет записан результат" << endl;
			cin >> file_output;
			f_begin = fopen(file_input, "rb");
			f_end = fopen(file_output, "wb");
			
			file_size = filesize(f_begin) - 4;

			sync1 = *((unsigned long *)&sync[0]);
			sync2 = *((unsigned long *)&sync[4]);

			for (int i = 0; i < file_size; i += 8) {
				coding(sync1, sync2);
				for (int k = 0; k < 8; k++) {
					buff[k] = false;
				}
				if (file_size - i >= 8) {
					fread(buff, 8, 1, f_begin);
				}
				else {
					fread(buff, file_size - i, 1, f_begin);
				}

				n1 = *((unsigned long *)&buff[0]);
				n2 = *((unsigned long *)&buff[4]);
				
				unsigned long sync11 = n1, sync22 = n2;

				n1 ^= sync1;
				n2 ^= sync2;
				sync1 = sync11;
				sync2 = sync22;

				char sym_rez;
				if (file_size - i >= 8) {
					for (int q = 0; q < 4; q++)
					{
						sym_rez = *((char *)&n1 + q);
						fprintf(f_end, "%c", sym_rez);
					}
					for (int q = 0; q < 4; q++)
					{
						sym_rez = *((char *)&n2 + q);
						fprintf(f_end, "%c", sym_rez);
					}
				}
				else {
					int itt = file_size - i;
					for (int q = 0; q < 4; q++)
					{
						if (itt == 0) {
							break;
						}
						itt--;
						sym_rez = *((char *)&n1 + q);
						fprintf(f_end, "%c", sym_rez);
						
					}
					for (int q = 0; q < 4; q++)
					{
						if (itt == 0) {
							break;
						}
						itt--;
						sym_rez = *((char *)&n2 + q);
						fprintf(f_end, "%c", sym_rez);
					}
				}
			}
			fread(buff, 4, 1, f_begin);
			test_imit = *((unsigned long *)&buff[0]);
			fclose(f_begin);
			fclose(f_end);
			f_begin = fopen(file_output, "rb");
			for (int i = 0; i < file_size; i += 8) {
				for (int k = 0; k < 8; k++) {
					buff[k] = false;
				}
				if (file_size - i >= 8) {
					fread(buff, 8, 1, f_begin);
				}
				else {
					fread(buff, file_size - i, 1, f_begin);
				}
				n1 = *((unsigned long *)&buff[0]);
				n2 = *((unsigned long *)&buff[4]);

				imit1 ^= n1;
				imit2 ^= n2;
				imitation(imit1, imit2);
			}
			cout << "---------------------------------------------------------------------" << endl;
			if (imit1 == test_imit) {
				cout << "Декодирование прошло успешно" << endl;
			}
			else {
				cout << "Декодирование прошло не успешно" << endl;
			}
			cout << "---------------------------------------------------------------------" << endl;

			break;

		default:
			cout << "---------------------------------------------------------------------" << endl;
			cout << "Неверное введенные значения" << endl;
			cout << "---------------------------------------------------------------------" << endl;
			break;
		};
	}
	return 0;
}


long filesize(FILE *stream)
{
	long curpos, length;
	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

int basic_step(int n1, int key) {
	n1 = key + n1;
	char first_byte = 0, second_byte = 0, zam_symbol = 0;
	int n = 7;
	for (int q = 3; q >= 0; q--) {
		zam_symbol = *((char *)&n1 + q);
		first_byte = (zam_symbol & 0xF0) >> 4;
		second_byte = (zam_symbol & 0x0F);
		first_byte = Tab_Z[n][first_byte];
		n--;
		second_byte = Tab_Z[n][second_byte];
		n--;
		zam_symbol = (first_byte << 4) | second_byte;
		*((char *)&n1 + q) = zam_symbol;
	}

	n1 = (n1 << 11) | (n1 >> 21);
	return n1;
}

void imitation(unsigned long &n1, unsigned long &n2) {
	unsigned int SUM232;
	for (int j = 0; j < 8; j++) {
		SUM232 = n2 ^ basic_step(n1, key[j]);
		n2 = n1;
		n1 = SUM232;
	}
	for (int j = 0; j < 7; j++) {
		SUM232 = n2 ^ basic_step(n1, key[j]);
		n2 = n1;
		n1 = SUM232;
	}
	SUM232 = n2 ^ basic_step(n1, key[0]);
	n2 = SUM232;
}

void coding(unsigned long &n1, unsigned long &n2) {
	unsigned int SUM232;
	for (int k = 0; k < 3; k++) {
		for (int j = 0; j < 8; j++) {
			SUM232 = n2 ^ basic_step(n1, key[j]);
			n2 = n1;
			n1 = SUM232;
		}
	}
	for (int j = 7; j > 0; j--) {
		SUM232 = n2 ^ basic_step(n1, key[j]);
		n2 = n1;
		n1 = SUM232;
	}
	SUM232 = n2 ^ basic_step(n1, key[0]);
	n2 = SUM232;
}