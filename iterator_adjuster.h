//
//  iterator_adjuster.h
//  math
//
//  Created by Patrick Sauter on 3/15/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_iterator_adjuster_h
#define math_iterator_adjuster_h

// I started writing this only to discover boost::iterator_adaptor and boost::transform_iterator take care of most of this for me.
// I wrote a step_iterator that uses iterator_adaptor to do the step portion of this guy.
// not going to delete it simply for future reference

#include <type_traits>
#include <iterator>
#include "iterator.h"
#include "functors.h"

template <typename Iterator, std::size_t Step = 1, typename Functor = identity>
class iterator_adjuster : public std::iterator_traits<Iterator> {
public:
	typedef iterator_adjuster	iterator_adjuster_;
	typedef Functor				functor;
	typedef Iterator			iterator;
protected:
	iterator						_value;
	static constexpr std::size_t	_step = Step;
	functor							_functor;
public:
	template <typename Iter = iterator, typename std::enable_if<std::is_constructible<Iter>::value>::type ...>
	iterator_adjuster() : _value{}, _functor{} { }
	
	iterator_adjuster(iterator_adjuster const & p) = default;
	iterator_adjuster& operator=(iterator_adjuster const & p) = default;
	
	virtual ~iterator_adjuster() = default;
	
	iterator_adjuster(iterator const & value) : _value(value) {
		
	}
	
	iterator base() const { return _value; }

	void swap(iterator_adjuster & other) {
		using std::swap;
		
		swap(_value, other._value);
	}
	
	iterator_adjuster & operator++() {
		std::advance(_value, _step);
		
		return *this;
	}
	iterator_adjuster operator++(int) {
		iterator_adjuster tmp(*this);
		
		++(*this);
		
		return tmp;
	}
	
	// random access iterator
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, bool>
	operator>(iterator_adjuster const & other) const {
		return _value > other._value;
	}
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, bool>
	operator>=(iterator_adjuster const & other) const {
		return _value >= other._value;
	}
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, bool>
	operator<(iterator_adjuster const & other) const {
		return _value < other._value;
	}
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, bool>
	operator<=(iterator_adjuster const & other) const {
		return _value <= other._value;
	}

	
	
	
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, typename iterator_adjuster::reference>
	operator[](typename iterator_adjuster::difference_type n) const {
		return _value[n * _step];
	}
	
	
	
	
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, iterator_adjuster>
	operator+(typename iterator_adjuster::difference_type n) const {
		return iterator_adjuster(_value + n * _step);
	}
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, iterator_adjuster>
	operator-(typename iterator_adjuster::difference_type n) const {
		return iterator_adjuster(_value - n * _step);
	}
	
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, typename iterator_adjuster::difference_type>
	operator-(iterator_adjuster const & b) const {
		return _value - b._value;
	}
	
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, iterator_adjuster&>
	operator+=(typename iterator_adjuster::difference_type n) {
		_value += n * _step;
		
		return *this;
	}
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::random_access_iterator_tag, iterator_adjuster&>
	operator-=(typename iterator_adjuster::difference_type n) {
		_value -= n * _step;
		
		return *this;
	}
	
	// bidirectional iterator
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::bidirectional_iterator_tag, iterator_adjuster&>
	operator--() {
		std::advance(_value, -_step);
		
		return *this;
	}
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::bidirectional_iterator_tag, iterator_adjuster>
	operator--(int) {
		iterator_adjuster tmp(*this);
		
		--(*this);
		
		return tmp;
	}

	// input iterator
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::input_iterator_tag, bool>
	operator==(iterator_adjuster const & other) const {
		return _value == other._value;
	}
	template <typename Iter = iterator>
	if_iterator_satisfies<Iter, std::input_iterator_tag, bool>
	operator!=(iterator_adjuster const & other) const {
		return _value != other._value;
	}
	
	// input / output iterator
	template <typename Iter = iterator>
	typename std::enable_if<
		iterator_satisfies<Iter, std::input_iterator_tag>::value ||
		iterator_satisfies<Iter, std::output_iterator_tag>::value,
		iterator
	>::type
	operator->() const {
		return _value;
	}
	
	template <typename Iter = iterator>
	typename std::enable_if<
		iterator_satisfies<Iter, std::input_iterator_tag>::value ||
		iterator_satisfies<Iter, std::output_iterator_tag>::value,
		typename std::result_of<functor(typename std::iterator_traits<iterator>::reference)>::type
	>::type
	operator*() const {
		return _functor(*_value);
	}
};


template <typename Iterator, std::size_t Steps = 1>
iterator_adjuster<typename std::decay<Iterator>::type>
make_iterator_adjuster(Iterator const & a) {
	return iterator_adjuster<typename std::decay<Iterator>::type, Steps>(a);
}

namespace std {
	template <typename T>
	void swap(iterator_adjuster<T> & a, iterator_adjuster<T> & b) {
		a.swap(b);
	}
}

#endif
