#include <iostream>
#include <map>
#include <cmath>
#include "PQHuffman.h"
#include "bitStreams.h"
using namespace std;



void dump(map<unsigned char, int> count) {
	int i = 0;
	cerr << "Char|Hex|Frequency" << endl;
	for (auto& itm : count) {
		i = itm.first;
		if (isprint(itm.first))
			cerr << setw(4) << right << itm.first;
		else
			cerr << setw(4) << right << " ";
		cerr << "|" << hex << setw(3);
		cerr << i << "|" << setw(9) << right << dec << itm.second << endl;
	}
}
void makeHuff(Heap<item<unsigned char, int>*, int>& PQheap) {
	item<unsigned char, int>* Qu2;
	item<unsigned char, int>* Qu3;

	while (PQheap.heap.size() > 1) {
		item<unsigned char, int>* Qu4 = new item<unsigned char, int>();
		Qu2 = PQheap.pop()->data();
		Qu3 = PQheap.pop()->data();
		Qu4->right() = Qu3;
		Qu4->left() = Qu2;
		Qu4->priority() = Qu2->priority() + Qu3->priority();
		PQheap.push(Qu4, Qu4->priority());
	}
}

int main() {
	bitStreams decypher("words.txt.puff", true);
	map<unsigned char, int> count;
	pair<unsigned char, unsigned long long> CStable[256];
	Heap<item<unsigned char, int>*, int> PQheap;
	unsigned char words = '\0';
	int ct = 0;
	int cnt = 0;
	ofstream fout;
	fstream fin;
	unsigned long long pattern;
	unsigned char bitcount;
	unsigned char getter;
	unsigned char byte = 0;

	///////////////////////////////////////
	// HUFF
	//////////////////////////////////////

	fin.open("words.txt");
	if (fin.fail())
		cerr << "fail" << endl;
	while (!fin.eof()) {
		++count[fin.get()];
		ct++;
	}
	fin.close();
	for (auto& itm : count) {
		item<unsigned char, int>* Que = new item<unsigned char, int>(itm.first, itm.second);
		PQheap.push(Que, itm.second);
	}
	makeHuff(PQheap);
	PQheap.buildCStable(PQheap.heap[0]->data());

	fin.open("words.txt", fstream::in);
	if (fin.fail())
		cerr << "fail" << endl;
	for (auto itm : PQheap.symbols) {
		bitcount = itm.first;
		getter = cnt;
		PQheap.bitsBytes_().putByte(bitcount);
		if (bitcount != 0) {
			PQheap.encode(getter);
			PQheap.bitsBytes_().flush();
		}
		cnt++;
	}
	
	cnt = 0;
	while(cnt < ct) {
		unsigned char temp = fin.get();
		PQheap.encode(temp);
		cnt++;
	}
	fin.close();
	PQheap.bitsBytes_().closeFile();
	///////////////////////////////////////
	//PUFF
	///////////////////////////////////////
 	fin.open("output.txt.huff", fstream::in | fstream::binary);
	if (fin.fail())
		cerr << "fail" << endl;
	cnt = 0;
	while (cnt < 256) {
		bitcount = fin.get();
		unsigned char bytes = ceil(bitcount / 8.);
		byte = bytes;
		for (unsigned char j = 0; j < bytes; ++j) {
			if (j != 0)
				decypher.bufferOut() <<= 8;
			getter = fin.get();
			decypher.bufferOut() |= getter;
		}
		decypher.bufferOut() >>= ((byte * 8) - bitcount);
		CStable[cnt] = make_pair(bitcount, decypher.bufferOut());
		decypher.bufferOut() = '\0';
		cnt++;
	}
	cnt = 0;
	for (auto T : CStable) {
		unsigned char data_;
		data_ = cnt;
		bitcount = T.first;
		pattern = T.second;
		if(bitcount != 0)
			decypher.rebuild(pattern, data_, bitcount);
		++cnt;
	}

	decypher.root();
	while (!fin.eof()) {
		getter = fin.get();
		decypher.decode(getter);
	}
	return 0;
}