#ifndef BITSTREAMS_H
#define BITSTREAMS_H
#include <bitset>
#include <iostream>
#include <fstream>

struct Node {
	unsigned char data = '\0';
	Node* left = nullptr;
	Node* right = nullptr;
};

class bitStreams {
public:
	bitStreams(std::string filename);
	~bitStreams();
	void rebuild(unsigned char temp);
	int putBit(unsigned int bit);
	int putByte(unsigned char byte);
	int getBit();
	int getByte();
	void flush();
	int & numBitsIn() { return numBitsIn_; }
	int & numBitsOut() { return numBitsOut_; }
	unsigned char & bufferIn() { return bufferIn_; }
	unsigned long long & bufferOut() { return bufferOut_; }
	int& location() { return location_; }
	Node* root() { return root_; }
	void closeFile() {	fout.close(); };
private:
	Node * root_;
	int numBitsIn_;
	int numBitsOut_;
	unsigned char bufferIn_;
	unsigned long long bufferOut_;
	int location_;
	std::fstream fout;
};


inline bitStreams::bitStreams(std::string filename)
{
	fout.open(filename, std::fstream::out);
	numBitsIn_ = 0;
	numBitsOut_ = 0;
	bufferIn_ = '\0';
	bufferOut_ = '\0';
}

inline void bitStreams::rebuild(unsigned char temp)
{
	Node* tempPtr = new Node;
	bufferOut() = temp;
	location() = 7;
	while (location() >= 0) {
		if (getBit()) {
			Node* temp2;
			tempPtr->right = temp2;
			tempPtr = tempPtr->right;
		}
		else {

		}
		--location();
	}
	std::cout << " ";
}

inline bitStreams::~bitStreams()
{
	flush();
}

inline int bitStreams::putBit(unsigned int bit) {
	bufferIn() = bufferIn() << 1;
	bufferIn() = bufferIn() | bit;
	++numBitsIn();
	if (numBitsIn() % 8 == 0) {
		putByte(bufferIn());
		numBitsIn() = 0;
		bufferIn() = '\0';
		return bufferIn();
	}
	return bufferIn();
}

inline int bitStreams::putByte(unsigned char byte)
{
	fout.put(byte);
	return 0;
}

inline int bitStreams::getBit()
{
	int temp = 0;
	if ((bufferOut() & (1 << location())) >> location())
		return temp | 1;
	else
		return temp;
}

inline int bitStreams::getByte()
{
	return getBit();
}

inline void bitStreams::flush()
{
	while (numBitsIn() != 0)
		putBit(0);
}
#endif