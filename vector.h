//
//  vector_v3.h
//  math
//
//  Created by Patrick Sauter on 3/15/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_vector_h
#define math_vector_h

#include "matrix.h"

namespace math {
	template <typename Scalar = reals_t, std::size_t N = 3>
	using vector = matrix<Scalar, N, 1>;
	
	// ------------------------------------------------------
	// dot and cross products.
	
	template <std::size_t N>
	reals_t inner_product(vector<reals_t,N> const & a, vector<reals_t,N> const & b) {
		reals_t sum;
		
		for (int i = 0; i < N; i++)
			sum += reals_t(a[i] * b[i]);
		
		return sum;
	}
	
	template <std::size_t N>
	reals_t inner_product(vector<complex_t,N> const & a, vector<complex_t,N> const & b) {
		reals_t sum;
		
		for (int i = 0; i < N; i++)
			sum += reals_t(a[i] * conj(b[i]));
		
		return sum;
	}
	
	template <typename T, std::size_t N>
	reals_t norm(vector<T,N> const & a) {
		return sqrt(inner_product(a,a));
	}
	
	template <typename T, std::size_t N>
	reals_t distance(vector<T,N> const & a, vector<T,N> const & b) {
		return norm(b - a);
	}
	
	// support a dot product of Nx1 matrices with other Nx1 matrices without having to transpose and do standard matrix multiplication.
	
	template <typename T, std::size_t N>
	auto operator*(vector<T,N> const & a, vector<T,N> const & b) -> decltype(inner_product(a,b)) {
		return inner_product(a,b);
	}
	
	// written this way so std::array and my darray implementation both work
	// through aggregrate initialization and initializer_list constructor respectively
	// the extra copy should be elided, and so this should be as fast as the c++14 version
	// of just returning { ... };
	template <typename T>
	vector<T,3> cross(vector<T,3> const & a, vector<T,3> const & b) {
		vector<T,3> ret = { a[1] * b[2] - a[2] * b[1],
							a[2] * b[0] - a[0] * b[2],
							a[0] * b[1] - a[1] * b[0] };
		
		return ret;
	}
	
	template <typename T>
	vector<T,3>&& cross(vector<T,3> && a, vector<T,3> const & b) {
		a = { a[1] * b[2] - a[2] * b[1],
			  a[2] * b[0] - a[0] * b[2],
			  a[0] * b[1] - a[1] * b[0] };
		
		return std::move(a);
	}
	template <typename T>
	vector<T,3>&& cross(vector<T,3> const & a, vector<T,3> && b) {
		b = { a[1] * b[2] - a[2] * b[1],
			  a[2] * b[0] - a[0] * b[2],
			  a[0] * b[1] - a[1] * b[0] };
		
		return std::move(b);
	}
	template <typename T>
	vector<T,3>&& cross(vector<T,3> && a, vector<T,3> && b) {
		a = { a[1] * b[2] - a[2] * b[1],
			  a[2] * b[0] - a[0] * b[2],
			  a[0] * b[1] - a[1] * b[0] };
		
		return std::move(a);
	}
}

namespace std {
	template <typename T, std::size_t N>
	ostream & operator<<(ostream& out, math::matrix<T,N,1> const & a) {
		out << "{ ";
		
		for (auto i : a.col(0)) {
			out << i << ' ';
		}
		
		out << "}";
		
		return out;
	}
}

#endif
