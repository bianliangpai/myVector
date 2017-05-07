#ifdef MY_VECTOR_HPP

template<typename T, typename Alloc = std::allocator<T> >
myVector<T, Alloc>::myVector(size_type count, const T& value, Alloc& alloc = Alloc())
{
	T t = T();
	alloc.construct(&t);
}

#endif