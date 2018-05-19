#ifndef PQHUFFMAN_H
#define PQHUFFMAN_H
#include <vector>
#include <iostream>
#include <iomanip>
#include "bitStreams.h"


template <typename Data, typename Priority>
class item {
public:
	item(Data data, Priority priority) : data_(data), priority_(priority), left_(nullptr), right_(nullptr) {}
	item() : data_(), priority_(), left_(nullptr), right_(nullptr), visited_(false) {}
	Data & data() { return data_; }
	Priority& priority() { return priority_; }
	bool & visited() { return visited_; }
	item *& left() { return left_; }
	item *& right() { return right_; }
private:
	item * left_;
	item* right_;
	bool visited_;
	Data data_;
	Priority priority_;
};


template <typename Data, typename Priority>
class Heap {
public:
	Heap() : bitsBytes("output.txt.huff") {}
	void buildCStable(item<unsigned char, int>* temp);
	void encode(unsigned char temp);
	void sortDown(int a);
	void sortUp(int a);
	void push(Data data, Priority priority);
	bool is_empty();
	void dump(item<unsigned char, int>* temp, int i);
	item<Data, Priority>* pop();
	std::pair<int, unsigned long long> symbolz[256];
	std::vector<item<Data, Priority>*> heap;
	bitStreams& bitsBytes_() { return bitsBytes; }
private:
	bitStreams bitsBytes;
	std::fstream fin;
	std::fstream fout;
};

template<typename Data, typename Priority>
inline void Heap<Data, Priority>::buildCStable(item<unsigned char, int>* temp)
{
	if (!temp->right() && !temp->left()) {
		temp->visited() = true;
		symbolz[temp->data()] = make_pair(bitsBytes.numBitsIn(), bitsBytes.bufferIn());
		bitsBytes.bufferIn() = '\0';
		bitsBytes.numBitsIn() = 0;
		return;
	}
	if (!temp->data() && temp->visited() == false) {
		if (temp->left() && temp->left()->visited() == false) {
			bitsBytes.bufferIn() = bitsBytes.bufferIn() << 1;
			bitsBytes.bufferIn() = bitsBytes.bufferIn() | 0;
			bitsBytes.numBitsIn() = bitsBytes.numBitsIn() + 1;
			buildCStable(temp->left());
			if (temp->right()->visited() && temp->left()->visited() && !temp->visited()) {
				temp->visited() = true;
				return;
			}
			if (temp->left()->visited() && !temp->right()->visited())
				buildCStable(heap[0]->data());
			else
				return;
		}
		if (temp->right() && temp->right()->visited() == false) {
			bitsBytes.bufferIn() = bitsBytes.bufferIn() << 1;
			bitsBytes.bufferIn() = bitsBytes.bufferIn() | 1;
			bitsBytes.numBitsIn() = bitsBytes.numBitsIn() + 1;
			buildCStable(temp->right());
			if (temp == heap[0]->data())
				return;
			if (temp->right()->visited() && temp->left()->visited() && !temp->visited()) {
				temp->visited() = true;
				return;
			}
		}
	}
}

template<typename Data, typename Priority>
inline void Heap<Data, Priority>::encode(unsigned char temp)
{
	bitsBytes.numBitsOut() = symbolz[temp].first;
	bitsBytes.bufferOut() = symbolz[temp].second;
	bitsBytes.location() = bitsBytes.numBitsOut() - 1;
	while (bitsBytes.location() >= 0) {
		bitsBytes.putBit(bitsBytes.getByte());
		--bitsBytes.location();
	}
}

template<typename Data, typename Priority>
inline void Heap<Data, Priority>::sortDown(int a)
{
	// a*2 is right child, a*2-1 is left child
	if (heap.size() > a * 2 && heap.size() > (a * 2 - 1))
	{
		if (heap[a * 2]->priority() > heap[a * 2 - 1]->priority())
		{
			if (heap[(a - 1)]->priority() > heap[(a * 2) - 1]->priority())
			{
				swap(heap[a - 1], heap[(a * 2) - 1]);
				sortDown(a * 2);
				return;
			}
		}
		else if (heap[a - 1]->priority() > heap[a * 2]->priority())
		{
			swap(heap[a - 1], heap[a * 2]);
			sortDown(a * 2 + 1);
			return;
		}
	}
	if (heap.size() > (a * 2 - 1) && heap[a * 2 - 1]->priority() < heap[a - 1]->priority()) {
		swap(heap[a - 1], heap[(a * 2) - 1]);
		sortDown(a * 2);
		return;
	}
	return;
}

template<typename Data, typename Priority>
inline void Heap<Data, Priority>::sortUp(int a)
{
	if (0 > ((a - 1) / 2))
		return;
	if (heap.size() > 1 && heap[(a - 1) / 2]->priority() > heap[a]->priority()) {
		swap(heap[(a - 1) / 2], heap[a]);
		sortUp((a - 1) / 2);
	}
	return;
}

template<typename Data, typename Priority>
inline void Heap<Data, Priority>::push(Data data, Priority priority)
{
	item<Data, Priority> *itm = new item<Data, Priority>(data, priority);
	heap.push_back(itm);
	sortUp(heap.size() - 1);
}

template<typename Data, typename Priority>
inline item<Data, Priority>* Heap<Data, Priority>::pop()
{
	item<Data, Priority>* temp = heap.front();
	if (!heap.empty()) {
		heap.front() = heap.back();
		heap.pop_back();
		sortDown(1);
		return temp;
	}
	std::cerr << "No items in heap" << std::endl;
	return temp;
}

template<typename Data, typename Priority>
inline bool Heap<Data, Priority>::is_empty()
{
	if (heap.empty())
		return true;
	return false;
}

template<typename Data, typename Priority>
inline void Heap<Data, Priority>::dump(item<unsigned char, int>* temp, int i)
{
	// i == depth
	std::cerr << "parent: " << std::setw(i) << temp->priority() << std::endl;
	//if (temp == heap[0]->data())
	//	std::cerr << "parent: " << std::setw(i) << temp->priority() << std::endl;
	if (temp->left_)
		std::cerr << "left: " << std::setw(i) << temp->left_->data() << std::endl;
	if (temp->right_)
		std::cerr << "right: " << std::setw(i) << temp->right_->data() << std::endl;
	if (temp->left_)
		dump(temp->left_, (i));
	if (temp->right_)
		dump(temp->right_, i);
	return;
}
#endif