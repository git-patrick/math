#ifndef _iterator_h
#define _iterator_h

/*
	this is poorly done (should just use is_convertible), and can be completely replaced by boost stuff.  I will switch to boost shortly.
*/

template <typename it, typename it_category>
struct iterator_satisfies : std::false_type {

};

template <typename it>
struct iterator_satisfies<it, std::random_access_iterator_tag> :
public std::is_same<typename it::iterator_category, std::random_access_iterator_tag> {
	
};

template <typename it>
struct iterator_satisfies<it, std::bidirectional_iterator_tag> :
public std::conditional<
	std::is_same<typename it::iterator_category, std::bidirectional_iterator_tag>::value,
	std::true_type,
	iterator_satisfies<it, std::random_access_iterator_tag>
>::type {

};

template <typename it>
struct iterator_satisfies<it, std::forward_iterator_tag> :
public std::conditional<
	std::is_same<typename it::iterator_category, std::forward_iterator_tag>::value,
	std::true_type,
	iterator_satisfies<it, std::bidirectional_iterator_tag>
>::type {

};

template <typename it>
struct iterator_satisfies<it, std::input_iterator_tag> :
public std::conditional<
	std::is_same<typename it::iterator_category, std::input_iterator_tag>::value,
	std::true_type,
	iterator_satisfies<it, std::forward_iterator_tag>
>::type {

};

template <typename it>
struct iterator_satisfies<it, std::output_iterator_tag> :
public std::conditional<
	std::is_const<typename it::value_type>::value,
	std::false_type,
	typename std::conditional<
		std::is_same<typename it::iterator_category, std::output_iterator_tag>::value,
		std::true_type,
		iterator_satisfies<it, std::forward_iterator_tag>
	>::type
>::type {

};

template <typename T>
struct iterator_satisfies<T*, std::random_access_iterator_tag> : public std::true_type { };
template <typename T>
struct iterator_satisfies<T*, std::bidirectional_iterator_tag> : public std::true_type { };
template <typename T>
struct iterator_satisfies<T*, std::forward_iterator_tag> : public std::true_type { };
template <typename T>
struct iterator_satisfies<T*, std::input_iterator_tag> : public std::true_type { };
template <typename T>
struct iterator_satisfies<T*, std::output_iterator_tag> :
public std::conditional<
	std::is_const<T>::value,
	std::false_type,
	std::true_type
>::type {

};

template <typename Iter, typename it_type, typename enable_type = void>
using if_iterator_satisfies = typename std::enable_if<iterator_satisfies<Iter, it_type>::value, enable_type>::type;

#endif