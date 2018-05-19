#include <iostream>
#include <map>
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
void makeHuff(Heap<item<unsigned char, int>*, int>& Q) {
	item<item<unsigned char, int>*, int>* Qu = new item<item<unsigned char, int>*, int>(nullptr, 0);
	item<unsigned char, int>* Qu2;
	item<unsigned char, int>* Qu3;

	while (Q.heap.size() > 1) {
		item<unsigned char, int>* Qu4 = new item<unsigned char, int>();
		Qu2 = Q.pop()->data();
		Qu3 = Q.pop()->data();
		Qu4->right() = Qu3;
		Qu4->left() = Qu2;
		Qu4->priority() = Qu2->priority() + Qu3->priority();
		Q.push(Qu4, Qu4->priority());
	}
}

int main() {
	bitStreams decypher("words.txt.puff");
	map<unsigned char, int> count;
	Heap<item<unsigned char, int>*, int> Q;
	unsigned char words = '\0';
	int ct = 0;
	int cnt = 0;
	ofstream fout;
	fstream fin;
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
		Q.push(Que, itm.second);
	}
//	dump(count);
	makeHuff(Q);
//	Q.dump(Q.heap[0]->data(), 10);
	Q.buildCStable(Q.heap[0]->data());

	fin.open("words.txt", fstream::in);
	if (fin.fail())
		cerr << "fail" << endl;
	for (auto& itm : Q.symbolz) {
		unsigned char bitcount = itm.first;
		unsigned char pattern = cnt;
		Q.bitsBytes_().putByte(bitcount);
		if (itm.first != 0) {
			Q.encode(pattern);
			Q.bitsBytes_().flush();
		}
		cnt++;
	}
	cnt = 0;
	while(cnt < ct) {
		unsigned char temp = fin.get();
		Q.encode(temp);
		cnt++;
	}
	fin.close();
	Q.bitsBytes_().closeFile();
	///////////////////////////////////////
	//PUFF
	///////////////////////////////////////
	fin.open("output.txt.huff", fstream::in | fstream::binary);
	if (fin.fail())
		cerr << "fail" << endl;
	cnt = 0;
	unsigned char bitcount;
	unsigned char bytes;
	int repeat = 0;
	while (!fin.eof()) {
		if (cnt < 256) {
			bitcount = fin.get();
			if (bitcount != 0) {
				repeat = ceil(bitcount/8.);
				while (repeat > 0) {
					decypher.rebuild(fin.get());
					--repeat;
				}
			}
			++cnt;
		}
		else {
			++cnt;
			fin.get();
		}
	}
	return 0;
}