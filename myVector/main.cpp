#include <iostream>
#include <vector>
#include "myVector.hpp"

struct Struct 
{
}S;

int main()
{
	myVector<int> myVec;
	for (int i = 0; i < 10; ++i)
		myVec.push_back(i);

	for (int i = 0; i < 5; ++i)
		myVec.pop_back();

	myVec.resize(3);
	myVec.resize(10);

	for (std::size_t i = 0; i < myVec.size(); ++i)
	{
		std::cout << myVec[i] << std::endl;
	}

	return 0;
}