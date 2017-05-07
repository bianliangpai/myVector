#include <iostream>
#include <vector>
#include "myVector.hpp"

int main()
{
	std::size_t vecL = 5;
	int initVal = 10;

	myVector<int> myVec(vecL, initVal);
	myVector<int> myVed(vecL*2, initVal*2);
	myVector<int> myVee(myVec);
	myVee = myVed;

	for (std::size_t i = 0; i < vecL; ++i)
	{
		std::cout << myVed[i] << std::endl;
	}

	return 0;
}