#include <iostream>
#include <vector>
#include <ctime>
#include "myVector.hpp"

template<typename Vec>
void test()
{
	static const int eleCnt = 10000;

	Vec myVec(eleCnt, eleCnt);
	for (int i = 0; i < eleCnt; ++i)
		myVec.push_back(i);

	for (int i = 0; i < eleCnt / 2; ++i)
		myVec.pop_back();

	myVec.resize(eleCnt / 3 * 2);
	myVec.resize(eleCnt / 3);

	Vec myVed(eleCnt, eleCnt * 2);
	myVed = myVec;

	myVec.insert(myVec.end(), myVed.begin(), myVed.end());

	myVec.erase(myVec.begin()+myVed.size(), myVec.end());

	myVec.shrink_to_fit();
}

int main()
{
	std::clock_t t;

	/* std::vector */
	t = std::clock();
	test<std::vector<int> >();
	t = std::clock() - t;
	std::cout << "std::vector cost time: " << (double)t / CLOCKS_PER_SEC << std::endl;

	/* myVector */
	t = std::clock();
	test<myVector<int> >();
	t = std::clock() - t;
	std::cout << "   myVector cost time: " << (double)t / CLOCKS_PER_SEC << std::endl;

	return 0;
}