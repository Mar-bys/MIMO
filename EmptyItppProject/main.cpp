#include <iostream>
#include <itpp/base/vec.h>
#include <itpp/base/mat.h>
#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <itpp\fixed\cfixed.h>
using namespace std;
using namespace itpp;
//creating matrix after turbocoder
void SubBlockBvec(bvec coded_blocks, int subblock_array[175][32])
{

	int dummy_bits = 28;
	int index = 0;
	for (int i = 0; i < 32; i++)
	{
		if (i < dummy_bits)
		{
			subblock_array[0][i] = 2;
		}
		else
		{
			subblock_array[0][i] = coded_blocks[index];
			index++;
		}
	}
	for (int i = 1; i < 175; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			subblock_array[i][j] = coded_blocks[index];
			index++;
		}
	}
}
void SubBlockBmat(bmat coded_blocks, int subblock_array[175][32])
{
	int dummy_bits = 28;
	int index = 0;
	for (int i = 0; i < 32; i++)
	{
		if (i < dummy_bits)
		{
			subblock_array[0][i] = 2;
		}
		else
		{
			subblock_array[0][i] = coded_blocks(index);
			index++;
		}
	}
	for (int i = 1; i < 175; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			subblock_array[i][j] = coded_blocks(index);
			index++;
		}
	}
}
//interleaver in1, parity1
void Interleaver(int subblock_array[175][32], int interleaver[175][32])
{
	int tab_seq[32] = { 0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30, 1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31 };
	for (int i = 0; i < 175; i++) 
	{
		for (int j = 0; j < 32; j++) 
		{
			interleaver[i][j] = subblock_array[i][tab_seq[j]];
		}
	}
	/*for (int i = 0; i < 175; i++) 
	{
		for (int j = 0; j < 32; j++)
			cout << interleaver[i][j] << " ";

	}*/
}
//interleaver parity2 pattern
void Interleaver_parity2(int subblock_array[175][32], int interleaver[175][32])
{
	int K = 5600;
	int RTC = 175;
	int CTC = 32;
	int p_k;
	int index = 0;
	int tab[5600];
	int tab_seq[32] = { 0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30, 1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31 };
	for (int i = 0; i < 175; i++) 
	{
		for (int j = 0; j < 32; j++) 
		{
			tab[i * 32 + j] = subblock_array[i][j];

		}
	}
	for (int i = 0; i < 175; i++) 
	{
		for (int j = 0; j < 32; j++) 
		{
			p_k = (tab_seq[index/ RTC] + CTC * (index % RTC) + 1) % K;
			interleaver[i][j] = tab[p_k];
			index++;
		}
	}

	
}
//bit collection-> in1, parity1 and parity2 to one vector
void Bit_collection(ivec & collection_vector, int tab1[175][32], int tab2[175][32], int tab3[175][32])
{
	int index = 0;
	for (int i = 0; i < 175; i++) 
	{
		for (int j = 0; j < 32; j++) 
		{
			collection_vector(index) = tab1[i][j];
			index++;
		}
	}
	for (int i = 0; i < 175; i++) 
	{
		for (int j = 0; j < 32; j++) {
			collection_vector(index) = tab2[i][j];
			index++;
			collection_vector(index) = tab3[i][j];
			index++;
		}
	}
}
void layer_mapping(int nrOfTrans, cvec modulated_signal, cvec & odd, cvec & even, cvec & v1, cvec & v2, cvec & v3, cvec & v4)
{
	if (nrOfTrans == 2)
	{
		int j = 0;
		for (int i = 0; i < 16800; i++)
		{
			if (i % 2 == 0)
			{
				even(j) = modulated_signal(i);
				j++;
			}
		}
		j = 0;
		for (int i = 0; i < 16800; i++)
		{
			if (i % 2 == 1)
			{
				odd(j) = modulated_signal(i);
				j++;
			}
		}
	}
	if (nrOfTrans == 4)
	{
		for (int i = 0; i < 16800; i++)
		{
			int j = 0;
			for (int i = 0; i < 16800; i++)
			{
				if (i % 2 == 0)
				{
					v1(j) = modulated_signal(i);
					j++;
				}
			}
			j = 0;
			for (int i = 0; i < 16800; i++)
			{
				if (i % 4 == 1)
				{
					v2(j) = modulated_signal(i);
					j++;
				}
			}
			j = 0;
			for (int i = 0; i < 16800; i++)
			{
				if (i % 4 == 2)
				{
					v3(j) = modulated_signal(i);
					j++;
				}
			}
			j = 0;
			for (int i = 0; i < 16800; i++)
			{
				if (i % 4 == 3)
				{
					v4(j) = modulated_signal(i);
					j++;
				}
			}

		}
	}
}
int main()
{
	QPSK qpsk;
	int Number_of_bits = 5568-24; // 5544 bits before crc
	bvec transmitted_bits;
	bvec crc_transmitted_bits;
	transmitted_bits = randb(Number_of_bits); //adding zeros and ones to transmitted data
	CRC_Code crc(string("CRC-24"));
	crc_transmitted_bits = crc.encode(transmitted_bits); // 24 bits added by crc
	Turbo_Codec turbo;
	ivec gen(2);
	gen(0) = 013;
	gen(1) = 015;
	int tail = 5;
	ivec interleaver_sequence = lte_turbo_interleaver_sequence(5568);
	turbo.set_parameters(gen, gen, tail, interleaver_sequence);
	bvec in1;
	bvec in2;
	bmat parity1;
	bmat parity2;
	turbo.encode_block(crc_transmitted_bits, in1, in2, parity1, parity2); //tail+(in1+in2)= crc_transmitter_bits
	cout << length(in1) << endl; //5571=5568+4 tail
	cout << parity1.size() << endl; //5571
	int subblock_in1[175][32];
	int subblock_parity1[175][32];
	int subblock_parity2[175][32];

	SubBlockBvec(in1,subblock_in1);
	SubBlockBmat(parity1, subblock_parity1);
	SubBlockBmat(parity2, subblock_parity2);
	int interleaver_in1[175][32];
	int interleaver_par1[175][32];
	int interleaver_par2[175][32];
	Interleaver(subblock_in1, interleaver_in1);
	Interleaver(subblock_parity1, interleaver_par1);
	Interleaver_parity2(subblock_parity2, interleaver_par2);
	ivec collection;
	collection.set_size(16800);
	Bit_collection(collection, interleaver_in1, interleaver_par1, interleaver_par2);
	cvec modulated_signal;
	modulated_signal = qpsk.modulate(collection);
	//cout << modulated_signal;
	cout << endl << "2 or 4 transmitter: ";
	int nrOfTrans;
	cin >> nrOfTrans;
	cvec even;
	even.set_size(8400);
	cvec odd;
	odd.set_size(8400);
	cvec v1;
	v1.set_size(4200);
	cvec v2;
	v2.set_size(4200);
	cvec v3;
	v3.set_size(4200);
	cvec v4;
	v4.set_size(4200);
	layer_mapping(nrOfTrans, modulated_signal, odd, even, v1, v2, v3, v4);
	cout << even << endl;
	//int x;
	//cin >> x;
	cvec y0, y1;
	y0.set_size(16800);
	y1.set_size(16800);
	//for (int i = 0; i < 8400; i++)
	cmat temp1;
	temp1 = even(0);
	cout <<"tutaj temp1:"<< temp1 << endl;
	cmat temp;
	temp = "1, 0, 0+1i, 0; 0, -1, 0, 0+1i; 0, 1, 0, 0+1i; 1, 0, 0-1i, 0";
	cout << even(0) << endl;
	cout << temp(3, 2) << endl;
	cout << temp(3, 2)*even(0);
	//cout << imag(temp(3,2));
	int c;
	cin >> c;
	

	//modulo indexu- layer mapping

	/*OFDM _ofdm;
	cvec output;
	int usedSubc = 0;
	cvec zeros_c;
	cvec symbvec = resGrid[txAntIdx]->getResGridSymbol(symbIdx);// pobranie pojedynczego symbolu z siatki zasobów
	symbol_buff = concat(zeros_c(1),
		resGrid[txAntIdx]->getResGridSymbol(symbIdx).get(usedSubc / 2, usedSubc - 1)
		zeros_c(fftSize - usedSubc - 1),
		resGrid[txAntIdx]->getResGridSymbol(symbIdx).get(0, usedSubc / 2 - 1) 
	system("Pause");
	int ncpu = cyclicPrefix;
	int fftSize = 1024;
	_ofdm.set_parameters(fftSize, ncpu);

	_ofdm.modulate(symbol_buff, output);
	*/
}
