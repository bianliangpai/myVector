#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

#include <cstddef>
#include <memory>
#include <limits>

#include "checkIt.hpp"
 
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
	typedef std::random_access_iterator_tag                            temp;
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
		/* whether 'InputIt' is iterator */
		if (!checkIt<InputIt>::type())
			return;
		
		// whether the element is convertible 
		if (!std::is_convertible_v< typename checkIt<InputIt>::eleType, value_type >)
			return;

		/* To do : if (InputIt derived from std::iterator) && (InputIt is empty class(example: no operator-)) */

		size_type cnt = size_type(last - first);

		uninitialized_fill_n(cnt, alloc);

		for (iterator i = start; first != last; ++first, ++i)
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

		iterator i = start;
		for (size_type st = 0; st < cnt; ++st, ++i)
			*i = *(init.begin()+st);
	}


	/* destruct functions */
	~myVector()
	{
		Alloc alloc;
		alloc.deallocate(start, capCnt);
	}


	/* operator= */
	myVector& operator=(const myVector& other)
	{
		/* whether element is convertible */
		return *this;
	}


	myVector& operator=(myVector&& other);
	myVector& operator=(std::initializer_list<T> ilist);

	/* assign value */
	void assign(size_type count, const T& value);
	template<typename InputIt>
	void assign(InputIt first, InputIt last);
	void assign(std::initializer_list<T> ilist);

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
	void reserve(size_type new_cap);
	size_type capacity() const { return this->capCnt; }
	void shrink_to_fit();

	/* modifiers */
	void clear();

	iterator insert(const_iterator pos, const T& value);
	iterator insert(const_iterator pos, T&& value);
	iterator insert(const_iterator pos, size_type count, const T& value);
	template<typename InputIt>
	void insert(iterator pos, InputIt first, InputIt last);
	template<typename InputIt>
	iterator insert(const_iterator pos, InputIt first, InputIt last);
	iterator insert(const_iterator pos, std::initializer_list<T> ilist);

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

	iterator erase(iterator pos);
	//iterator erase(const_iterator pos);
	iterator erase(iterator first, iterator last);
	//iterator erase(const_iterator first, const_iterator last);

	void push_back(const T& value);
	void push_back(T&& value);

	void pop_back();

	void resize(size_type count, T value = T());
	void resize(size_type count);
	void resize(size_type count, const value_type& value);

	void swap(myVector& other)
	{
		std::swap(this->start, other.start);
		std::swap(this->finish, other.finish);
		std::swap(this->end_of_storage, other.end_of_storage);
		std::swap(this->eleCnt, other.eleCnt);
		std::swap(this->capCnt, other.capCnt);
	}

private:
	iterator start;
	iterator finish;
	iterator end_of_storage;

	size_type eleCnt;
	size_type capCnt;
	static const size_type initSize = 100;

	void uninitialized_fill_n(size_type count, Alloc& alloc = Alloc())
	{
		size_type allocCnt = count * 2;
		this->start = alloc.allocate(
			allocCnt > this->initSize ? allocCnt : this->initSize
			);
		this->finish = this->start + count;
		this->end_of_storage = this->start + allocCnt;

		this->eleCnt = count;
		this->capCnt = allocCnt;
	}
};


template<typename T, typename Alloc>
bool operator == (const myVector<T, Alloc>& lhs,
				  const myVector<T, Alloc>& rhs);

template<typename T, typename Alloc>
bool operator != (const myVector<T, Alloc>& lhs,
				  const myVector<T, Alloc>& rhs);

template<typename T, typename Alloc>
bool operator < (const myVector<T, Alloc>& lhs,
				 const myVector<T, Alloc>& rhs);

template<typename T, typename Alloc>
bool operator <= (const myVector<T, Alloc>& lhs,
				  const myVector<T, Alloc>& rhs);

template<typename T, typename Alloc>
bool operator > (const myVector<T, Alloc>& lhs,
				 const myVector<T, Alloc>& rhs);

template<typename T, typename Alloc>
bool operator >= (const myVector<T, Alloc>& lhs,
				  const myVector<T, Alloc>& rhs);

template<typename T, typename Alloc>
void swap (const myVector<T, Alloc>& lhs,
		   const myVector<T, Alloc>& rhs);

//#include "myVector.tpp"

#endif /* MY_VECTOR_HPP */