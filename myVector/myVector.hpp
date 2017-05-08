#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

#include <cstddef>
#include <memory>
#include <limits>
#include <algorithm>

#include "checkIt.hpp"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef HALF
#define HALF(x) (x/2)
#endif

struct move_category {};
struct copy_and_move_tag : move_category {};
struct move_only_tag : move_category {};
 
template<typename T,
		 typename Alloc = std::allocator<T> >
class myVector
{
public:
	typedef T                                                          value_type;
	typedef Alloc                                                      allocator_type;
	typedef std::size_t                                                size_type;
	typedef value_type&                                                reference;
	typedef const value_type&                                          const_reference;
	typedef typename std::allocator_traits<Alloc>::pointer             pointer;
	typedef typename std::allocator_traits<Alloc>::const_pointer       const_pointer;
	typedef pointer                                                    iterator;
	typedef const iterator                                             const_iterator;
	typedef std::reverse_iterator<iterator>                            reverse_iterator;
	typedef std::reverse_iterator<const_iterator>                      const_reverse_iterator;
	typedef typename std::iterator_traits<iterator>::difference_type   difference_type;


	/* construct functions */
	/* 1 */
	explicit myVector(const Alloc& alloc = Alloc()) 
		: start(nullptr), finish(nullptr), end_of_storage(nullptr),
		  eleCnt(0), capCnt(0)
		{
		}


	/* 2 http://www.cnblogs.com/youxin/archive/2012/06/15/2550546.html */
	myVector(size_type count, const T& value, Alloc& alloc = Alloc())
	{	
		uninitialized_fill_n(count, alloc);

		for (size_type i = 0; i < count; ++i)
			*(start + i) = value;
	}


	/* 3 */
	explicit myVector(size_type count, Alloc& alloc = Alloc())
	{
		uninitialized_fill_n(count, alloc);
	}


	/* 4 */
	template<typename InputIt>
	myVector(InputIt first, InputIt last, Alloc& alloc = Alloc())
	{
		if (!InputItValid<InputIt>())
			return;

		if (first >= last)
			return;

		/* To do : if (InputIt derived from std::iterator) && (InputIt is empty class(example: no operator-)) */

		size_type cnt = size_type(last - first);

		if (cnt > this->max_size())
			return;

		uninitialized_fill_n(cnt, alloc);

		for (iterator i = this->start; first != last; ++first, ++i)
			*i = *first;
	}


	/* 5: copy constructor */
	myVector(const myVector& other)
	{
		uninitialized_fill_n(other.size());
		memcpy(this->start, other.begin(), other.size()*sizeof(size_type));
	}


	myVector(const myVector& other, Alloc& alloc)
	{
		uninitialized_fill_n(other.size(), alloc);
		memcpy(this->start, other.begin(), other.size()*sizeof(size_type));
	}


	/* 6: move constructor */
	myVector(myVector&& other)
	{
		this->swap(other);
	}


	/* 7: allocator-extended move constructor */
	myVector(myVector&& other, Alloc& alloc)
	{	
		uninitialized_fill_n(other.size(), alloc);
		memcpy(this->start, other.begin(), other.size()*sizeof(size_type));
	}


	/* 8: constructs the container with the contents of the initializer list init */
	myVector(std::initializer_list<T> init, Alloc& alloc = Alloc())
	{
		size_type cnt = size_type(init.end() - init.begin());

		uninitialized_fill_n(cnt, alloc);

		memcpy(this->start, init.begin(), sizeof(size_type)*cnt);
	}


	/* destruct functions */
	~myVector()
	{
		
		alloc.deallocate(start, capCnt);
	}


	/* operator= */
	myVector& operator=(const myVector& other)
	{
		/* whether element is convertible
		 * but no use
		 * if element type is not same ==> two different classes

		if (!std::is_convertible_v<typename std::decay_t<decltype(other)>::value_type, value_type>)
			return *this;
		
		*/

		int diff = other.size() - this->capCnt;
		if (diff > 0)
			this->move<move_only_tag>(diff);

		/* copy elements */
		memcpy(this->start, other.begin(), sizeof(size_type)*other.size());

		/* update */
		this->eleCnt = other.size();
		this->finish = this->begin() + this->eleCnt;

		return *this;
	}


	myVector& operator=(myVector&& other)
	{
		/* To do */
	}


	myVector& operator=(std::initializer_list<T> ilist)
	{
		int diff = ilist.size() - this->capCnt;
		if (diff > 0)
			this->move<move_only_tag>(diff);

		/* copy elements */
		memcpy(this->start, ilist.begin(), sizeof(size_type)*ilist.size());

		/* update */
		this->eleCnt = ilist.size();
		this->finish = this->begin() + this->eleCnt;

		return *this;
	}


	/* assign value */
	void assign(size_type count, const T& value)
	{
		if (count > this->max_size())
			return;
		
		int diff = count - this->capCnt;
		if (diff > 0)
			this->move<move_only_tag>(diff);

		for (iterator i = this->start; i != last; ++i)
			*i = value;

		/* update */
		this->eleCnt = count;
		this->finish = this->begin() + this->eleCnt;
	}


	template<typename InputIt>
	void assign(InputIt first, InputIt last)
	{
		if (!InputItValid<InputIt>())
			return;

		size_type iptSize = size_type(last - first);
		if (iptSize > this->max_size())
			return;

		if (first == last)
			return;

		int diff = iptSize - this->capCnt;
		if (diff > 0)
			this->move<move_only_tag>(diff);
		else if (checkShrink(iptSize))
			this->shrink(HALF(this->capCnt));

		this->clear();
		
		for (iterator i = this->start; first != last; ++first, ++i)
			*i = *first;

		/* update */
		this->eleCnt = iptSize;
		this->finish = this->start + this->eleCnt;
	}


	/* return the allocator associated with the container */
	allocator_type get_allocator() const { return Alloc(); }

	/* element access */
	reference       at(size_type pos) { return *(this->start + pos); }
	const_reference at(size_type pos) const { return *(this->start + pos); }

	reference       operator[](size_type pos) { return *(this->start + pos); }
	const_reference operator[](size_type pos) const { return *(this->start + pos); }

	reference       front() { return *(this->start); }
	const_reference front() const { return *(this->start); }

	reference       back() { return *(this->finish - 1); }
	const_reference back() const { return *(this->finish - 1); }

	T*       data() { return &(*(this->start)); }
	const T* data() const { return &(*(this->start)); }

	/* iterators */
	iterator       begin() { return this->start; }
	const_iterator begin() const { return this->start; }
	const_iterator cbegin() const { return this->start; }

	iterator       end() { return this->finish; }
	const_iterator end() const { return this->finish; }
	const_iterator cend() const { return this->finish; }

	reverse_iterator       rbegin() { return reverse_iterator(this->finish); }
	const_reverse_iterator rbegin() const { return reverse_iterator(this->finish); }
	const_reverse_iterator crbegin() const { return reverse_iterator(this->finish); }

	reverse_iterator       rend() { return reverse_iterator(this->start); }
	const_reverse_iterator rend() const { return reverse_iterator(this->start); }
	const_reverse_iterator crend() const { return reverse_iterator(this->start); }

	/* capacity */
	bool empty() const { return (this->start == this->finish); }


	size_type size() const { return this->eleCnt; }


	size_type max_size() const { return (size_type(-1) / sizeof(size_type)); }


	void reserve(size_type new_cap)
	{
		if (new_cap > this->max_size()
			|| new_cap < this->eleCnt)
			return;

		if (new_cap > this->capCnt)
		{
			size_type diff = new_cap - this->capCnt;
			this->move<copy_and_move_tag>(diff);
		}
		else
		{
			this->shrink(new_cap);
		}
	}


	size_type capacity() const { return this->capCnt; }


	void shrink_to_fit()
	{
		this->shrink(this->eleCnt);
	}
	

	/* modifiers */
	void clear()
	{
		memset(this->start, value_type(), sizeof(size_type)*this->eleCnt);

		/* update */
		this->eleCnt = 0;
		this->finish = this->start;
	}


	iterator insert(const_iterator pos, const T& value)
	{
		/* To do */
	}


	iterator insert(const_iterator pos, size_type count, const T& value)
	{
		if (pos < this->start || pos > this->finish)
			return this->finish;

		return this->_insert(pos, count, value);
	}


	iterator insert(const_iterator pos, T&& value)
	{
		return insert(pos, 1, value);
	}


	template<typename InputIt>
	iterator insert(const_iterator pos, InputIt first, InputIt last)
	{
		if (!InputItValid<InputIt>())
			return this->finish;

		if (first >= last)
			return this->finish;

		return this->_insert<InputIt>(pos, first, last);
	}


	/* ------------------------------------------------> to do */
	/*
	template<typename Args>
	iterator emplace(const_iterator pos, Args&& args);
	template<typename Args>
	void emplace_back(Args&& args);
	template<typename Args>
	reference emplace_back(Args&& args);
	*/
	/* ------------------------------------------------> end */

	iterator erase(const_iterator pos) { return erase(pos, pos+1); }


	iterator erase(const_iterator first, const_iterator last)
	{
		if (first >= last)
			return this->finish;

		if (first < this->start || last > this->finish)
			return this->finish;

		/* do erase */
		size_type count = std::distance(first, last);
		memset(first, value_type(), sizeof(value_type)*count);
		memcpy(first, last, sizeof(value_type)*count);

		/* update */
		this->eleCnt -= count;
		this->finish = this->start + this->eleCnt;

		if (checkShrink(this->eleCnt))
			this->shrink(HALF(this->capCnt));
	}


	void push_back(const T& value)
	{
		if (this->full())
			this->move<copy_and_move_tag>();

		if (this->empty())
			this->uninitialized_fill_n(0);

		*(this->finish) = value;

		/* update */
		++(this->finish);
		++(this->eleCnt);
	}


	void push_back(T&& value)
	{
		if (this->full())
			this->move<copy_and_move_tag>();

		if (this->empty())
			this->uninitialized_fill_n(0);

		*(this->finish) = value;

		/* update */
		++(this->finish);
		++(this->eleCnt);
	}


	void pop_back()
	{
		if (this->empty())
			return;

		*(this->finish) = value_type();

		/* update */
		--(this->eleCnt);
		--(this->finish);

		if (checkShrink(this->eleCnt))
			shrink(HALF(this->capCnt));
	}


	void resize(size_type count)
	{
		resize(count, 0);
	}


	void resize(size_type count, const value_type& value)
	{
		if (count < 0 || count > this->max_size())
			return;

		int diff = count - this->eleCnt;
		if (diff == 0)
			return;
		else if (diff > 0)
		{
			if (diff > (this->capCnt - this->eleCnt))
				this->move<copy_and_move_tag>(diff);

			for (size_type i = 0; i < diff; ++i)
				*(this->finish + i) = value;

			/* update */
			this->eleCnt += diff;
			this->finish += diff;
		}
		else
		{
			diff *= -1;
			memset(this->finish - diff, value_type(), sizeof(value_type)*diff);

			/* update */
			this->eleCnt -= diff;
			this->finish -= diff;

			if (checkShrink(this->eleCnt))
				this->shrink(HALF(this->capCnt));
		}
	}


	void swap(myVector& other)
	{
		std::swap(this->start, other.start);
		std::swap(this->finish, other.finish);
		std::swap(this->end_of_storage, other.end_of_storage);
		std::swap(this->eleCnt, other.eleCnt);
		std::swap(this->capCnt, other.capCnt);
	}


private:
	Alloc alloc;

	iterator start;
	iterator finish;
	iterator end_of_storage;

	size_type eleCnt;
	size_type capCnt;
	
	static const 
	size_type initSize = 100;


	bool full() { return (this->capCnt == this->eleCnt); }


	template<typename InputIt>
	bool InputItValid()
	{
		/* whether is iterator */
		if (!checkIt<InputIt>::type())
			return false;

		/* whether element is convertible */
		if (!std::is_convertible_v<typename checkIt<InputIt>::eleType, value_type>)
			return false;

		return true;
	}


	bool checkShrink(size_type eleNum)
	{
		return (eleNum < HALF(this->capCnt)) && (this->capCnt > initSize);
	}


	void uninitialized_fill_n(size_type count, Alloc& alloc = Alloc())
	{
		size_type allocCnt = MIN(MAX(count * 2, initSize), this->max_size());
		this->start = alloc.allocate(allocCnt);
		this->finish = this->start + count;
		this->end_of_storage = this->start + allocCnt;

		this->eleCnt = count;
		this->capCnt = allocCnt;
	}


	template<typename>
	void move(size_type diff = size_type(), Alloc& alloc = Alloc())
	{  // two ways to move
	}
	

	template<>
	void move<copy_and_move_tag>(size_type diff, Alloc& alloc)
	{
		/* To do : bad alloc */
		size_type allocCnt = (diff + this->capCnt) * 2;
		iterator tStart = alloc.allocate(allocCnt);

		/* copy elements */
		memcpy(tStart, this->start, sizeof(value_type)*this->eleCnt);

		/* free memory */
		alloc.deallocate(this->start, this->eleCnt);

		/* update */
		this->start = tStart;
		this->finish = this->start + this->eleCnt;
		this->end_of_storage = this->start + allocCnt;

		this->capCnt = allocCnt;
	}


	template<>
	void move<move_only_tag>(size_type diff, Alloc& alloc)
	{
		/* To do : bad alloc */
		size_type allocCnt = (diff + this->capCnt) * 2;
		iterator tStart = alloc.allocate(allocCnt);

		/* free memory */
		alloc.deallocate(this->start, this->eleCnt);

		/* update */
		this->start = tStart;
		this->finish = this->start + this->eleCnt;
		this->end_of_storage = this->start + allocCnt;

		this->capCnt = allocCnt;
	}


	void shrink(size_type remainSize)
	{
		if (remainSize < this->eleCnt)
			return;

		alloc.deallocate(start+remainSize, this->capCnt-remainSize);

		/* update */
		this->end_of_storage = start + remainSize;
		this->capCnt = remainSize;
	}


	iterator _insert(const_iterator pos, size_type count, const T& value)
	{
		size_type posi = std::distance(this->start, pos);

		int diff = count - (this->capCnt - this->eleCnt);
		if (diff > 0)
			this->move<copy_and_move_tag>(diff);

		iterator new_pos = start + posi;
		memcpy(new_pos + count, new_pos, sizeof(size_type)*std::distance(new_pos, this->finish));

		for (size_type i = 0; i < posi; ++i)
			*(new_pos + i) = value;

		/* update */
		this->eleCnt += count;
		this->finish = this->start + this->eleCnt;

		return new_pos;
	}


	template<typename InputIt>
	iterator _insert(const_iterator pos, InputIt first, InputIt last)
	{
		size_type posi = std::distance(this->start, pos);
		size_type count = std::distance(first, last);
		int diff = count - (this->capCnt - this->eleCnt);
		if (diff > 0)
			this->move<copy_and_move_tag>(diff);

		iterator new_pos = this->start + posi;
		memcpy(new_pos + count, new_pos, sizeof(size_type)*std::distance(new_pos, this->finish));

		for (size_type i = 0; i < posi; ++i)
			*(new_pos + i) = *(first + i);

		/* update */
		this->eleCnt += count;
		this->finish = this->start + this->eleCnt;

		return new_pos;
	}

};


template<typename T, typename Alloc>
bool operator == (const myVector<T, Alloc>& lhs,
				  const myVector<T, Alloc>& rhs)
{
	if (lhs.size() != rhs.size())
		return false;

	uint32_t size = lhs.size();
	for (uint32_t i = 0; i < size; ++i)
		if (lhs[i] != rhs[i])
			return false;

	return true;
}

template<typename T, typename Alloc>
bool operator != (const myVector<T, Alloc>& lhs,
				  const myVector<T, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template<typename T, typename Alloc>
bool operator < (const myVector<T, Alloc>& lhs,
				 const myVector<T, Alloc>& rhs)
{
	return std::lexicographical_compare(lhs.begin(), lhs.end(),
										rhs.begin(), rhs.end());
}

template<typename T, typename Alloc>
bool operator <= (const myVector<T, Alloc>& lhs,
				  const myVector<T, Alloc>& rhs)
{
	return ((lhs < rhs) || (lhs == rhs));
}

template<typename T, typename Alloc>
bool operator > (const myVector<T, Alloc>& lhs,
				 const myVector<T, Alloc>& rhs)
{
	return !(lhs <= rhs);
}

template<typename T, typename Alloc>
bool operator >= (const myVector<T, Alloc>& lhs,
				  const myVector<T, Alloc>& rhs)
{
	return !(lhs < rhs);
}

template<typename T, typename Alloc>
void swap (const myVector<T, Alloc>& lhs,
		   const myVector<T, Alloc>& rhs);

//#include "myVector.tpp"

#endif /* MY_VECTOR_HPP */