//
//  numeric_derivative.h
//  math
//
//  Created by Patrick Sauter on 4/1/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_numeric_derivative_h
#define math_numeric_derivative_h

// so my goal is to take a template parameter that is any callable type
// and wrap that up in a functor who forwards parameters through operator() to the callable.
// I need to be able to change arbitrarily indexed parameters to add "dx" during derivative calculation
// solution: ended up using variadic template parameters and tuple stuff to accomplish the forwarding

// for functors that can store state, the question is: should I copy that functors state at the time of dF creation,
// or should I store a reference to the functor and always act on the most up to date version of it ?
// I think it makes more sense to make a copy.

// trying to avoid derivative having state so I don't have to worry about threading issues.
// need to find a way to pass dx to the derivative without it storing it then...

// update: I've decided to store dx in derivative.  the question is, should I allow it to be modified, or keep it constant
// and if I allow modification, should derivative be thread safe?
// for the time being, I am going to make it so dx cannot be changed after creation, and see how I like it.
// if it proves cumbersome, I will likely make derivative thread safe, or perhaps provide a thread safe version.

#include <utility>
#include <typeinfo>

#include "core.h"
#include "function_traits.h"
#include "tuple_help.h"

namespace math {
	// need to enable_if this for only callable types F
	// X is the index of the parameter we are differentiating with respect to.
	
	template <typename F, std::size_t X>
	class numeric_derivative : public function_traits<F> {
	private:
		typedef function_traits<F> _traits;
		
		static_assert(X < _traits::arity,
					  "Assertion failed, numeric_derivative parameter index too large.");
		
		typedef typename _traits::template arg<X>	field_t;
		typedef typename _traits::result_type		result_t;
		
		static_assert(check::field<field_t>::value,
					  "Assertion failed, domain not a field");
		static_assert(check::vector_space<result_t, field_t>::value,
					  "Assertion failed, range not a vector space over the domain field");
		
		F				_f;
		field_t const	_dx = infinitesimal_tag{};
	public:
		numeric_derivative() = default;

		numeric_derivative(F const & f, field_t dx) : _f(f), _dx(dx) { };
		numeric_derivative(F && f, field_t dx) : _f(std::move(f)), _dx(dx) { };
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
		
		
		
		
		/* fix up these Args ..., I think they should be Args && ... */
		template <typename ... Args, int ... S>
		auto _call(
			pat::integer_sequence<S ...> s,
			Args ... a
		) const -> decltype((_f(a ...) - _f(a ...)) / _dx) {
			// look up more accurate numeric numeric_derivatives
			
			return (_f(if_same<X, S>::_add(a, _dx) ...) - _f(a ...)) / _dx;
		}
	public:
		template <typename ... Args>
		auto operator()(Args ... a) const -> decltype(_call(pat::index_sequence_for<Args ...>{}, a ...)) {
			return _call(pat::index_sequence_for<Args ...>{}, a ...);
		}
	};
	
	template <typename Functor, std::size_t X>
	std::ostream & operator << (std::ostream & o, numeric_derivative<Functor,X> const & m) {
		return o << "ND(" << typeid(Functor).name() << ", " << X << ")";
	}
}

#endif