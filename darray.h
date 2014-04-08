//
//  darray.h
//
//  Created by Patrick Sauter on 3/4/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef __darray__
#define __darray__

#include <iterator>
#include <algorithm>
#include <utility>
#include <memory>
#include <cstddef>
#include <limits>
#include <initializer_list>

// stands for dynamically allocated array
// provides us O(1) moves as opposed to O(N) in std::array, but there is a performance hit on creation as we dynamically allocate memory from the heap
// ultimately, it is most useful with large arrays that are required to move around, as opposed to small arrays where little moves are required.

namespace pat {
	template <typename T, std::size_t N>
	class darray {
	public:
		typedef T					value_type;
		typedef std::size_t			size_type;
		typedef std::ptrdiff_t		difference_type;
		typedef value_type&			reference;
		typedef value_type const&	const_reference;
		typedef value_type*			pointer;
		typedef value_type const*	const_pointer;
		typedef value_type*			iterator;
		typedef value_type const*	const_iterator;
		typedef std::reverse_iterator<iterator>			reverse_iterator;
		typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
		
		darray() : V(new value_type[N]{}) {
			trace();
		}
		
		darray(std::initializer_list<value_type> list) : V(new value_type[N]) {
			trace();
			std::copy(list.begin(), list.end(), begin());
		}
		
		darray(darray const & other) : V(new value_type[N]) {
			trace();
			
			std::copy(other.begin(), other.end(), begin());
		}
		darray(darray && other) {
			trace();
			
			swap(*this, other);
		}
		
		darray & operator=(darray const & other) {
			std::copy(other.begin(), other.end(), begin());
			
			return *this;
		}
		darray & operator=(darray && other) {
			swap(*this, other);
			
			return *this;
		}
		
		virtual ~darray() = default;
		
		reference		front() { return V[0]; }
		const_reference front() const { return V[0]; }
		
		reference		back() { return V[N - 1]; }
		const_reference	back() const { return V[N - 1]; }
		
		pointer			data() { return V.get(); }
		const_pointer	data() const { return V.get(); }
		
		constexpr bool empty() const { return N == 0; }
		
		constexpr size_type size() const { return N; }
		constexpr size_type max_size() const { return std::numeric_limits<size_type>::max(); }
		
		void fill(const_reference value) {
			std::fill(begin(), end(), value);
		}
		
		friend void swap(darray& a, darray& b) {
			using std::swap;
			
			swap(a.V, b.V);
		}
		
		reference at(size_type index) {
			if (index >= size())
				throw std::out_of_range("Index out of range.");
			
			return V[index];
		}
		
		const_reference at(size_type index) const {
			if (index >= size())
				throw std::out_of_range("Index out of range.");
			
			return V[index];
		}
		
		const_reference operator[](size_type index) const {
			return V[index];
		}
		
		reference operator[](size_type index) {
			return V[index];
		}
		
		iterator begin() { return V.get(); }
		const_iterator begin() const { return V.get(); }
		const_iterator cbegin() const { return V.get(); }
		
		iterator end() { return V.get() + N; }
		const_iterator end() const { return V.get() + N; }
		const_iterator cend() const { return V.get() + N; }
		
		reverse_iterator rbegin() { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
		const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
		
		reverse_iterator rend() { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
		const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }
		
	private:
		std::unique_ptr<value_type[]>	V;
	};


	template <typename T, std::size_t N>
	bool operator==(darray<T,N> const & a, darray<T,N> const & b) {
		for (int i = 0; i < N; i++) {
			if (a[i] != b[i])
				return false;
		}
		
		return true;
	}
	template <typename T, std::size_t N>
	bool operator<(darray<T,N> const & a, darray<T,N> const & b) {
		for (int i = 0; i < N; i++) {
			if (a[i] < b[i]) return true;
			if (a[i] < b[i]) return false;
		}
		return false;
	}
	template <typename T, std::size_t N>
	bool operator<=(darray<T,N> const & a, darray<T,N> const & b) {
		for (int i = 0; i < N; i++) {
			if (a[i] < b[i]) return true;
			if (a[i] < b[i]) return false;
		}
		return true;
	}

	template <typename T, std::size_t N>
	bool operator>(darray<T,N> const & a, darray<T,N> const & b) {
		return !(a <= b);
	}
	template <typename T, std::size_t N>
	bool operator>=(darray<T,N> const & a, darray<T,N> const & b) {
		return !(a < b);
	}
	template <typename T, std::size_t N>
	bool operator!=(darray<T,N> const & a, darray<T,N> const & b) {
		return !(a == b);
	}
}

#endif /* defined(__darray__) */
