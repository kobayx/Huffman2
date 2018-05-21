#ifndef BITSTREAMS_H
#define BITSTREAMS_H
#include <bitset>
#include <iostream>
#include <fstream>

struct Node {
	unsigned char data;
	Node* left;
	Node* right;
};

class bitStreams {
public:
	bitStreams(std::string filename);
	bitStreams(std::string filename, bool bin);
	~bitStreams();
	void rebuild(unsigned long long pattern, unsigned char data_, unsigned char bitcount);
	void decode(unsigned char byte);
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
	Node*& placeHolder() { return placeHolder_; }
	void closeFile() {	fout.close(); };
private:
	Node * root_ = new Node();
	Node * placeHolder_ = new Node();
	int numBitsIn_;
	int numBitsOut_;
	unsigned char bufferIn_;
	unsigned long long bufferOut_;
	int location_;
	std::fstream fout;
};


inline bitStreams::bitStreams(std::string filename)
{
	fout.open(filename, std::fstream::out| std::fstream::in | std::fstream::binary);
	numBitsIn_ = 0;
	numBitsOut_ = 0;
	bufferIn_ = '\0';
	bufferOut_ = '\0';
	root_->data = '\0';
	root_->left = nullptr;
	root_->right = nullptr;
	placeHolder_ = root_;
}
inline bitStreams::bitStreams(std::string filename, bool bin)
{
	fout.open(filename, std::fstream::out | std::fstream::in);
	numBitsIn_ = 0;
	numBitsOut_ = 0;
	bufferIn_ = '\0';
	bufferOut_ = '\0';
	root_->data = '\0';
	root_->left = nullptr;
	root_->right = nullptr;
	placeHolder_ = root_;
}
inline void bitStreams::rebuild(unsigned long long pattern, unsigned char data_, unsigned char bitcount)
{
	Node* tpPtr = root();
	bufferOut() = pattern;
	location() = bitcount - 1;
	while (location() >= 0) {
		if (getBit()) {
			if (!tpPtr->right) {
				Node *tempR = new Node();
				tpPtr->right = tempR;
				tpPtr = tpPtr->right;
			}
			else if (tpPtr->right) {
				tpPtr = tpPtr->right;
			}
			if (location() == 0)
				tpPtr->data = data_;
		}
		else if (!getBit()) {
			if (!tpPtr->left) {
				Node *tempL = new Node();
				tpPtr->left = tempL;
				tpPtr = tpPtr->left;
			}
			else if (tpPtr->left) {
				tpPtr = tpPtr->left;
			}
			if (location() == 0)
				tpPtr->data = data_;
		}
	--location();
	}
}

inline void bitStreams::decode(unsigned char byte)
{

	int temp = 0;
	location() = 7;
	bufferIn() = byte;
	while (location() >= 0) {
		if ((bufferIn() & (1 << location())) >> location()) {
			if (placeHolder()->right) {
				placeHolder() = placeHolder()->right;
				if (placeHolder()->data != '\0') {
					putByte(placeHolder()->data);
					placeHolder() = root();
				}
			}
		}
		else {
			//if (!placeHolder()->left)
			//	return;
			if (placeHolder()->left) {
				placeHolder() = placeHolder()->left;
				if (placeHolder()->data != '\0') {
					putByte(placeHolder()->data);
					placeHolder() = root();
				}
			}
		}
		--location();
	}
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
	//bufferOut() = fout.get();
	//++numBitsOut();
	//if (numBitsOut() == 0) {
	//	numBitsOut() = 8;
	//	bufferIn() = getByte();
	//	return bufferOut();
	//}
	//return bufferIn();
	int temp = 0;
	if ((bufferOut() & (1 << location())) >> location())
		return temp | 1;
	else
		return temp;
}

inline int bitStreams::getByte()
{
	return fout.get();
}

inline void bitStreams::flush()
{
	while (numBitsIn() != 0)
		putBit(0);
}
#endif
