/*
//  matrix.h
//  math
//

	this uses a slightly different than standard way of implementing binary operators.  the reason for this is to avoid move constructor,
	since std::array is O(n) in moves.
*/

#ifndef __math__matrix__
#define __math__matrix__

#include <iostream>
#include <iomanip>

#include <cstddef>
#include <cmath>
#include <cassert>
#include <array>
#include <numeric>

#include "core.h"

#include "iterator_range.h"
#include "step_iterator.h"
#include "functors.h"

using pat::iterator_range;
using pat::make_range;

namespace math {
	template <typename Type = reals_t, std::size_t N = 3, std::size_t M = 3>
	class matrix : public std::array<Type, N * M> {
	public:
		static_assert(check::ring<Type>::value,
					  "Assertion failed, matrix type not a Ring.");
		
		static constexpr std::size_t rows = N;
		static constexpr std::size_t cols = M;
		
		typedef Type value_t;
		
		typedef pat::step_iterator<value_t*>		col_iterator;
		typedef pat::step_iterator<value_t const*>	const_col_iterator;
		
		typedef value_t*		row_iterator;
		typedef value_t const*	const_row_iterator;
		
		template <typename NewType>
		using convert_type = matrix<NewType, rows, cols>;
		
		matrix()					= default;
		matrix(matrix const & v)	= default;
		matrix(matrix && v)			= default;
		
		matrix& operator=(matrix const & v)	= default;
		matrix& operator=(matrix && v)		= default;
		
		matrix(math::additive_identity_tag) : matrix(value_t{}) { }
		
		template <std::size_t _N = N, std::size_t _M = M, typename = typename std::enable_if<_N == _M>::type>
		matrix(math::multiplicative_identity_tag) {
			for (std::size_t i = 0; i < cols; ++i) {
				(*this)[i * (cols + 1)] = value_t(math::multiplicative_identity_tag{});
			}
		}
		matrix(std::initializer_list<value_t> L) {
			std::copy(L.begin(), L.end(), this->begin());
		}
		
		explicit matrix(value_t const & f) {
			this->fill(f);
		}
		
		/*
		template <typename U, typename >
		matrix(matrix<U, scalar_type, rows, cols> const & other) {
			std::transform(other.begin(), other.end(), this->begin(), pat::static_caster<storage_type>());
		}
		*/
		
		iterator_range<row_iterator> row(std::size_t index) {
			assert(index < rows);
			
			row_iterator	r(&(this->data()[index * cols]));
			
			return make_range(r, r + cols);
		}
		
		iterator_range<const_row_iterator> row(std::size_t index) const {
			assert(index < rows);
			
			const_row_iterator	r(&(this->data()[index * cols]));
			
			return make_range(r, r + cols);
		}
		
		iterator_range<col_iterator> col(std::size_t index) {
			assert(index < cols);
			
			col_iterator	c(&(this->data()[index]), cols);
			
			return make_range(c, c + rows);
		}
		
		iterator_range<const_col_iterator> const col(std::size_t index) const {
			assert(index < cols);
			
			const_col_iterator	c(&(this->data()[index]), cols);
			
			return make_range(c, c + rows);
		}
	};
	// ------------------------------------------------------
	// matrix == matrix
	template <typename T, std::size_t N, std::size_t M>
	bool operator==(matrix<T,N,M> const & a, matrix<T,N,M> const & b) {
		for (auto i = 0; i < a.size(); ++i)
			if (a[i] != b[i])
				return false;

		return true;
	}
	template <typename T, std::size_t N, std::size_t M>
	bool operator!=(matrix<T,N,M> const & a, matrix<T,N,M> const & b) {
		return !(a == b);
	}
	

	// ------------------------------------------------------
	// matrix += matrix
	template <typename T, std::size_t N, std::size_t M>
	matrix<T,N,M>& operator+=(matrix<T,N,M> & a, matrix<T,N,M> const & b) {
		a = a + b;
		
		return a;
	}
	template <typename T, std::size_t N, std::size_t M>
	matrix<T,N,M>&& operator+=(matrix<T,N,M> && a, matrix<T,N,M> const & b) {
		a = a + b;
		
		return std::move(a);
	}
	
	// ------------------------------------------------------
	// matrix -= matrix
	
	template <typename T, std::size_t N, std::size_t M>
	matrix<T,N,M>& operator-=(matrix<T,N,M>& a, matrix<T,N,M> const & b) {
		a = a - b;
		
		return a;
	}
	template <typename T, std::size_t N, std::size_t M>
	matrix<T,N,M>&& operator-=(matrix<T,N,M> && a, matrix<T,N,M> const & b) {
		a = a - b;
		
		return std::move(a);
	}
	


	// ------------------------------------------------------
	// matrix * matrix
	
	template <typename T, std::size_t N, std::size_t M, std::size_t P>
	matrix<T,N,P> operator*(matrix<T,N,M> const & a, matrix<T,M,P> const & b) {
		matrix<T,N,P>	m;
		
		for (std::size_t i = 0; i < N; ++i) {
			for (std::size_t j = 0; j < P; ++j) {
				auto row = a.row(i);
				
				m[i * P + j] = std::inner_product(row.begin(), row.end(), b.col(j).begin(), T{});
			}
		}
		
		return m;
	}
	
	
	// ------------------------------------------------------
	// matrix + matrix
	
	template <typename T, std::size_t N, std::size_t M>
	matrix<T,N,M> operator+(matrix<T,N,M> const & a, matrix<T,N,M> const & b) {
		matrix<T,N,M> t;
		
		for (auto i = 0; i < a.size(); ++i)
			t[i] = a[i] + b[i];

		return t;
	}
	
	// ------------------------------------------------------
	// -matrix
	
	template <typename T, std::size_t N, std::size_t M>
	matrix<T,N,M> operator-(matrix<T,N,M> const & a) {
		matrix<T,N,M> t;
		
		for (auto i = 0; i < a.size(); ++i)
			t[i] = -a[i];

		return t;
	}
	
	// ------------------------------------------------------
	// matrix - matrix
	
	template <typename T, std::size_t N, std::size_t M>
	matrix<T,N,M> operator-(matrix<T,N,M> const & a, matrix<T,N,M> const & b) {
		matrix<T,N,M> t;
		
		for (auto i = 0; i < a.size(); ++i)
			t[i] = a[i] - b[i];

		return t;
	}

	// --------------------------------------------------
	// TRANSPOSE
	// I could consider adding a flag to say whether the matrix is transposed or not
	// then adjust the returned iterator based on that flag.
	// depends on how many checks against that flag would be required vs just transposing like this.
	
	template <typename T, std::size_t N, std::size_t M>
	matrix<T,M,N> transpose(matrix<T,N,M> const & a) {
		matrix<T,M,N>	m;
		
		for (std::size_t i = 0; i < N; ++i) {
			for (std::size_t j = 0; j < M; ++j) {
				m.col(i)[j] = a.row(i)[j];
			}
		}
		
		return m;
	}
	
	// --------------------------------------------------
	// DETERMINANT
	
/*	template <typename T, std::size_t N> {
	T determinant(matrix<T,N,N> const & a) {
	
	I want to add permutations before I add this.
*/
	
	// --------------------------------------------------
	// inverse
	// this needs to be enabled only if the matrix is over a field, not a ring.
	// also depends on the determinant.
	
	// ------------------------------------------------------
	// matrix *= scalar
	// I am deeming A *= s to be the same thing as A = A * s, or right side multiplication
	
	template <std::size_t N, std::size_t M, typename T, typename Scalar,
			  typename = typename std::enable_if<
				math::check::has_multiply<T,Scalar,T>::value
			  >::type>
	matrix<T,N,M>& operator*=(matrix<T,N,M>& a, Scalar const & s) {
		for (int i = 0; i < a.size(); ++i)
			a[i] = a[i] * s;
		
		return a;
	}
	template <std::size_t N, std::size_t M, typename T, typename Scalar,
			  typename = typename std::enable_if<
				math::check::has_multiply<T,Scalar,T>::value
			  >::type>
	matrix<T,N,M>&& operator*=(matrix<T,N,M>&& a, Scalar const & s) {
		for (int i = 0; i < a.size(); ++i)
			a[i] = a[i] * s;
		
		return std::move(a);
	}
	
	// ------------------------------------------------------
	// matrix /= scalar
	// when scalars form a field field
	
	template <std::size_t N, std::size_t M, typename T, typename Scalar,
		typename = typename std::enable_if<
			math::check::has_divide<T, Scalar, T>::value
		>::type>
	matrix<T,N,M>&
	operator/=(matrix<T,N,M>& a, Scalar const & s) {
		for (int i = 0; i < N; ++i)
			a[i] = a[i] / s;
		
		return a;
	}
	
	template <std::size_t N, std::size_t M, typename T, typename Scalar,
		typename = typename std::enable_if<
			math::check::has_divide<T, Scalar,T>::value
		>::type>
	matrix<T,N,M>&&
	operator/=(matrix<T,N,M>&& a, Scalar const & s) {
		for (int i = 0; i < N; ++i)
			a[i] = a[i] / s;
		
		return std::move(a);
	}
	
	
	
	
	// ------------------------------------------------------
	// matrix * Scalar // Scalar * matrix
		
	template <std::size_t N, std::size_t M, typename T, typename Scalar,
		typename = typename std::enable_if<
			math::check::has_multiply<T, Scalar, T>::value
		>::type>
	matrix<T,N,M>
	operator*(matrix<T,N,M> const & a, Scalar const & s) {
		matrix<T,N,M> t;
		
		for (auto i = 0; i < a.size(); ++i)
			t[i] = a[i] * s;

		return t;
	}
	template <std::size_t N, std::size_t M, typename T, typename Scalar,
		typename = typename std::enable_if<
			math::check::has_multiply<Scalar, T, T>::value
		>::type>
	matrix<T,N,M>
	operator*(Scalar const & s, matrix<T,N,M> const & a) {
		matrix<T,N,M> t;
		
		for (int i = 0; i < t.size(); ++i)
			t[i] = s * a[i];
		
		return t;
	}
	
	// ------------------------------------------------------
	// matrix / Scalar
	// when scalar's form a field.
	
	template <std::size_t N, std::size_t M, typename T, typename Scalar,
		typename = typename std::enable_if<
			math::check::has_divide<T, Scalar, T>::value
		>::type>
	matrix<T,N,M>
	operator/(matrix<T,N,M> const & a, Scalar const & s) {
		matrix<T,N,M> t;
		
		for (int i = 0; i < t.size(); ++i)
			t[i] = a[i] / s;
		
		return t;
	}
}

namespace std {
	template <typename T, std::size_t N, std::size_t M>
	ostream & operator<<(ostream& out, math::matrix<T,N,M> const & a) {
		out << "{" << std::endl;
		
		for (int i = 0; i < N; ++i) {
			for (auto j : a.row(i)) {
				out << std::setw(3) << j;
			}
			out << std::endl;
		}
		
		out << "}";
		
		return out;
	}
}

#endif /* defined(__math__matrix__) */
