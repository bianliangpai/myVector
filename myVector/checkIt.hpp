#ifndef IS_ITERATOR_HPP
#define IS_ITERATOR_HPP

#include <type_traits>
#include <iterator>

template<typename T,
		 typename = void >
struct checkIt : std::false_type
{
};

/* iterator derived from std::iterator */
template<typename T >
struct checkIt<T, std::void_t<typename T::iterator_category > >
	: std::true_type
{
	using eleType = typename T::value_type;
};

/* iterator is built-in std::container's iterator */
template<typename T >
struct checkIt<typename T,
			   std::enable_if_t< std::is_base_of< std::_Iterator_base12, T >::value > >
	: std::true_type
{
	using eleType = typename T::value_type;
};

/* iterator is a pointer */
template<typename T >
struct checkIt<typename T,
			   std::enable_if_t< std::is_pointer<T>::value > >
	: std::true_type
{
	using eleType = typename std::remove_pointer<T>::type;
};

template<typename T >
struct identity
{
	using type = T;
};

#endif /* IS_ITERATOR_HPP */

