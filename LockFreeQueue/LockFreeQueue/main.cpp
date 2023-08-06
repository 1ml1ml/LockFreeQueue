#include <iostream>

#include "LockFreeQueue.h"

struct A { }; 

int main(void)
{
	LockFreeQueue <int> lfq(10);
	lfq.enqueue(10);
	auto c = lfq.dequeue();
	return 0;
}