//
//  numeric_derivative.h
//  math
//
//  Created by Patrick Sauter on 4/1/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_numeric_derivative_h
#define math_numeric_derivative_h

/*	"dx" cannot currently be changed after construction.
	this is a conscious decision to avoid thread issues of stateful functors.
	I may change this eventually, or provide a thread safe version where it can be changed.
*/


#include <utility>
#include <typeinfo>

#include "core.h"
#include "function_traits.h"
#include "tuple_help.h"

namespace math {
	// need to enable_if this for only callable types F
	// X is the index of the parameter we are differentiating with respect to.
	// N is the order of the derivative
	
	template <typename F, std::size_t X>
	class numeric_derivative : public function_traits<F> {
	private:
		typedef function_traits<F> _traits;
		
		static_assert(X < _traits::arity,
					  "Assertion failed, numeric_derivative parameter index too large.");
		
		typedef typename _traits::template arg<X>	field_t;
		typedef typename _traits::result_type		result_t;
		typedef typename std::decay<F>::type		functor_t;
		
		static_assert(check::field<field_t>::value,
					  "Assertion failed, domain not a field");
		static_assert(check::vector_space<result_t, field_t>::value,
					  "Assertion failed, range not a vector space over the domain field");
		
		functor_t			_f;
		field_t const		_dx = infinitesimal_tag{};
	public:
		numeric_derivative() = default;

		numeric_derivative(F const & f, field_t dx) : _f(f), _dx(dx) { };
		numeric_derivative(F && f, field_t dx) : _f(std::move(f)), _dx(dx) { };
		
		field_t		dx() const { return _dx; }
		functor_t	f() const  { return _f; }
	private:
		template <int A, int B>
		struct if_same {
			// just passes through the first parameter if A != B
			template <typename T, typename U>
			static constexpr T _add(T const & t, U const & u) {
				return t;
			}
		};
		template <int A>
		struct if_same<A, A> {
		
			// adds the first and second parameter with return type == decltype(second parameter)
			// the reason for returning the second parameter type is that is where we are passing _dx, and the return type
			// needs to be the same as the original functor's parameter type to avoid rounding errors if you did something like the following...
			/*	
				double f(double, double, double)
				
				D(f)(1,2,3)
			*/
			// the parameters to D(f)::operator() are deduced as integers, so when you add field_t dx (double)
			// when calculating the numeric_derivative value, we get rounding errors.
			// so we have to make sure the X-th parameter is, infact, field_t to avoid any rounding concerns.
			
			template <typename T, typename U>
			static constexpr U _add(T const & t, U const & u) {
				return t + u;
			}
		};
		
		/*
			want to add support for higher order numerical derivatives using the same central difference technique
		
		template <typename ... S>
		struct _sequence { };
		
		template <typename ... Rationals, typename ... Args, int ... S, int ... SR, int ... dx>
		auto _call(
			Args const & ... args,
			pat::integer_sequence<S ...> s,		// integer sequence from 0 - #args - 1
			_sequence<Rationals ...>,			// coefficients for the central difference sum
			pat::integer_sequence<SR ...>,		// integer sequence from 0 - #Rationals - 1
			pat::integer_sequence<dx ...>		// integer sequence for the dx multipler (-2 ... 2) for example, sizeof...(dx) == sizeof...(coefficients)
		) const -> decltype(_f(args ...) / _dx) {
			add<
				multiply<
					Rational,
					compose<functor_t, add_if_same<X, S> ...>> multiply<rational<dx>,
			
			// accuracy 4 central finite difference
			return (  rational<1,12>()() * _f(if_same<X, S>::_add(args, rational<-2>()() * _dx) ...)
					- rational<2,3>()()  * _f(if_same<X, S>::_add(args, -_dx ) ...)
					+ rational<2,3>()()  * _f(if_same<X, S>::_add(args, _dx) ...)
					- rational<1,12>()() * _f(if_same<X, S>::_add(args, rational<2>()() * _dx) ...)) / _dx;
		}
		
		*/
		
		
		
		template <int ... S, typename ... Args>
		auto _call(
			pat::integer_sequence<S ...> s,
			Args const & ... a
		) const -> decltype(_f(a ...) / _dx) {
			// accuracy 4 central finite difference first order derivative
		return (  reals_t(1) / 12 * _f(if_same<X, S>::_add(a, reals_t(-2) * _dx) ...)
				- reals_t(2) / 3  * _f(if_same<X, S>::_add(a, -_dx ) ...)
				+ reals_t(2) / 3  * _f(if_same<X, S>::_add(a, _dx) ...)
				- reals_t(1) / 12 * _f(if_same<X, S>::_add(a, reals_t(2) * _dx) ...)) / _dx;
		}
	public:
		template <typename ... Args>
		auto operator()(Args const & ... a) const -> decltype(_call(pat::index_sequence_for<Args ...>{}, a ...)) {
			return _call(pat::index_sequence_for<Args ...>{}, a ...);
		}
	};
	
	template <std::size_t N, typename F>
	numeric_derivative<F, N> ND(F const & f, typename function_traits<F>::template arg<N> dx = infinitesimal_tag{}) {
		return numeric_derivative<F, N>(f, dx);
	}
	
	template <typename Functor, std::size_t X>
	std::ostream & operator << (std::ostream & o, numeric_derivative<Functor,X> const & m) {
		return o << "ND<" << X << ">(" << typeid(Functor).name() << ", " << m.dx() << ")";
	}
}

#endif