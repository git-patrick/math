/*
//  matrix.h
//  math
*/

#ifndef __math_matrix_h
#define __math_matrix_h

#include <iostream>
#include <iomanip>

#include <type_traits>
#include <cstddef>
#include <cmath>
#include <cassert>
#include <array>
#include <numeric>

#include "core.h"

#include <pat/array.h>
#include <pat/iterator_range.h>
#include <pat/step_iterator.h>
#include <pat/functors.h>

using pat::iterator_range;
using pat::make_range;

namespace math {
	template <typename Type = reals_t, std::size_t N = 3, std::size_t M = 3>
	class matrix : public std::array<Type, N * M> {
	private:
		typedef std::array<Type, N * M>					super_t;
	public:
		static_assert(check::ring<Type>::value,
					  "Assertion failed, matrix type not a Ring.");
		
		static constexpr std::size_t rows() { return N; }
		static constexpr std::size_t cols() { return M; }
		
		typedef Type value_type;
		
		typedef pat::step_iterator<value_type*>			col_iterator;
		typedef pat::step_iterator<value_type const*>	const_col_iterator;
		
		typedef value_type*			row_iterator;
		typedef value_type const*	const_row_iterator;
		
		template <typename NewType>
		using convert_type = matrix<NewType, rows(), cols()>;
		
		matrix()					= default;
		matrix(matrix const & v)	= default;
		matrix(matrix && v)			= default;
		
		matrix& operator=(matrix const & v)	= default;
		matrix& operator=(matrix && v)		= default;
		
		template <typename T, typename = std::enable_if_t<!std::is_same<T, Type>::value>>
		explicit matrix(matrix<T, N, M> const & m) : super_t{ pat::array_cast<Type>(m) } { }
		
		matrix(math::additive_identity_tag) : matrix(value_type{}) { }
		
		template <std::size_t _N = N, std::size_t _M = M, typename = typename std::enable_if<_N == _M>::type>
		matrix(math::multiplicative_identity_tag) {
			for (std::size_t i = 0; i < std::array<Type, N * M>::size(); ++i) {
				(*this)[i] = (!(i % (cols() + 1)) ? value_type(math::multiplicative_identity) : value_type(math::additive_identity));
			}
		}
		matrix(std::initializer_list<value_type> L) {
			std::copy(L.begin(), L.end(), this->begin());
		}
		
		explicit matrix(value_type const & f) {
			this->fill(f);
		}
		
	/*
		template <typename U, typename >
		matrix(matrix<U, scalar_type, rows, cols> const & other) {
			std::transform(other.begin(), other.end(), this->begin(), pat::static_caster<storage_type>());
		}
	*/
		
		typedef iterator_range<row_iterator> row_t;
		typedef iterator_range<const_row_iterator> const_row_t;
		
		typedef iterator_range<col_iterator> col_t;
		typedef iterator_range<const_col_iterator> const_col_t;
		
		row_t row(std::size_t index) {
			assert(index < rows());
			
			row_iterator	r(&(this->data()[index * cols()]));
			
			return make_range(r, r + cols());
		}
		
		const_row_t row(std::size_t index) const {
			assert(index < rows());
			
			const_row_iterator	r(&(this->data()[index * cols()]));
			
			return make_range(r, r + cols());
		}
		
		col_t col(std::size_t index) {
			assert(index < cols());
			
			col_iterator	c(&(this->data()[index]), cols());
			
			return make_range(c, c + rows());
		}
		
		const_col_t const col(std::size_t index) const {
			assert(index < cols());
			
			const_col_iterator	c(&(this->data()[index]), cols());
			
			return make_range(c, c + rows());
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
	
	template <typename T, typename Y, std::size_t N, std::size_t M, std::size_t P>
	matrix<std::common_type_t<T,Y>,N,P> operator*(matrix<T,N,M> const & a, matrix<Y,M,P> const & b) {
		matrix<std::common_type_t<T,Y>,N,P>	m;
		
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
	
	template <typename T, typename Y, std::size_t N, std::size_t M>
	matrix<std::common_type_t<T,Y>,N,M> operator+(matrix<T,N,M> const & a, matrix<Y,N,M> const & b) {
		matrix<std::common_type_t<T,Y>,N,M> t;
		
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
	
	template <typename T, typename Y, std::size_t N, std::size_t M>
	matrix<std::common_type_t<T,Y>,N,M> operator-(matrix<T,N,M> const & a, matrix<Y,N,M> const & b) {
		matrix<std::common_type_t<T,Y>,N,M> t;
		
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
	// CONCATENATE
	template <typename T, std::size_t N, std::size_t M, std::size_t P>
	matrix<T,N,M+P> hconcat(matrix<T,N,M> const & a, matrix<T,N,P> const & b) {
		matrix<T,N,M+P> m;
		
		for (auto i = 0; i < a.rows(); ++i) {
			auto A = a.row(i);
			auto B = b.row(i);
			
			std::copy(B.begin(), B.end(), std::copy(A.begin(), A.end(), m.row(i).begin()));
		}
		
		return m;
	}
	
	template <typename T, std::size_t N, std::size_t M, std::size_t P>
	matrix<T,N+M,P> vconcat(matrix<T,N,P> const & a, matrix<T,M,P> const & b) {
		matrix<T,N+M,P> m;
		
		std::copy(b.begin(), b.end(), std::copy(a.begin(), a.end(), m.begin()));
		
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
				pat::traits::has_multiply<T,Scalar,T>::value
			  >::type>
	matrix<T,N,M>& operator*=(matrix<T,N,M>& a, Scalar const & s) {
		for (int i = 0; i < a.size(); ++i)
			a[i] = a[i] * s;
		
		return a;
	}
	template <std::size_t N, std::size_t M, typename T, typename Scalar,
			  typename = typename std::enable_if<
				pat::traits::has_multiply<T,Scalar,T>::value
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
			pat::traits::has_divide<T, Scalar, T>::value
		>::type>
	matrix<T,N,M>&
	operator/=(matrix<T,N,M>& a, Scalar const & s) {
		for (int i = 0; i < N; ++i)
			a[i] = a[i] / s;
		
		return a;
	}
	
	template <std::size_t N, std::size_t M, typename T, typename Scalar,
		typename = typename std::enable_if<
			pat::traits::has_divide<T, Scalar,T>::value
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
			pat::traits::has_multiply<T, Scalar, T>::value
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
			pat::traits::has_multiply<Scalar, T, T>::value
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
			pat::traits::has_divide<T, Scalar, T>::value
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
