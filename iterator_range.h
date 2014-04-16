//
//  iterator_range.h
//  math
//
//  Created by Patrick Sauter on 3/15/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef _pat_iterator_range_h
#define _pat_iterator_range_h

// boost has this concept already.  I will be switching shortly.

#include <iterator>
#include "iterator.h"

namespace pat {
	template <typename Iterator>
	class iterator_range {
	public:
		typedef Iterator									iterator;
		typedef std::reverse_iterator<iterator>				reverse_iterator;
		
		typedef std::iterator_traits<iterator>				traits;
		
		typedef typename traits::difference_type			size_type;
		
		template <typename Iter = iterator, typename = typename std::enable_if<std::is_constructible<Iter>::value>::type>
		iterator_range() : _begin{}, _end{} { }
		
		iterator_range(iterator_range && r) = default;
		iterator_range(iterator_range const & r) = default;
		
		iterator_range& operator=(iterator_range const & r) {
			_begin = r._begin;
			_end = r._end;
			
			return *this;
		}
		
		iterator_range(Iterator const & beginning, Iterator const & ending) : _begin(beginning), _end(ending) {
			
		}
		
		void swap(iterator_range & r) {
			using std::swap;
			
			swap(_begin, r._begin);
			swap(_end, r._end);
		}
		
		virtual ~iterator_range() = default;
		
		iterator			begin() const	{ return _begin; }
		iterator			end() const		{ return _end; }
		
		template <typename Iter = iterator, typename = typename if_iterator_satisfies<Iter, std::random_access_iterator_tag>::type>
		typename traits::reference
		operator[](typename traits::difference_type d) const {
			return _begin[d];
		}
		template <typename Iter = iterator, typename = typename if_iterator_satisfies<Iter, std::random_access_iterator_tag>::type>
		size_type
		size() const {
			return _end - _begin;
		}
		
		template <typename Iter = iterator, typename = typename if_iterator_satisfies<Iter, std::bidirectional_iterator_tag>::type>
		reverse_iterator
		rbegin() const	{ return reverse_iterator(_end); }
		
		template <typename Iter = iterator, typename = typename if_iterator_satisfies<Iter, std::bidirectional_iterator_tag>::type>
		reverse_iterator
		rend() const	{ return reverse_iterator(_begin); }
	private:
		iterator _begin, _end;
	};
	
	template <typename Iterator>
	inline iterator_range<Iterator> make_range(Iterator const & b, Iterator const & e) {
		return iterator_range<Iterator>(b,e);
	}
	
	template <typename T>
	void swap(iterator_range<T>& a, iterator_range<T>& b) {
		a.swap(b);
	}
}


#endif