//
//  setup.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef _math_setup_h
#define _math_setup_h

#include "tmp.h"

#include "constant.h"
#include "select.h"
#include "trig.h"
#include "compose.h"
#include "gamma.h"

namespace math {
	namespace analytic {
		namespace detail {
			// Multiply Sort Priority
			template <typename T>
			struct __msp;
			
			// ultimately the functor that will be getting called.
			template <typename F, typename G>
			struct ___multiply;
			
			// this object ultimately contains the ___multiply type we want after the mathematical simplification / order adjustments to allow simplification
			// the purpose of the int X is to allow partial specializations that would otherwise be ambiguous to NOT be ambiguous if X1 != X2
			// if the specialization would match in terms of T and U, it will choose the template with LARGER X first.
			// the lowest you can set X and still have the template considered is 1.
			template <typename T1, typename T2, int Priority>
			struct __multiply;
			
			enum __multiply_priority {
				mp_maximum  = 6,
				mp_complex  = mp_maximum,
				mp_repair   = mp_complex - 1,
				mp_simplify = mp_repair - 1,
				mp_pow      = mp_simplify - 1,
				mp_sort     = mp_pow - 1,
				mp_lowest   = mp_sort - 1
			};
			
			template <typename T1, typename T2>
			struct find__multiply {
				template <int Y> using action = typename __multiply<T1, T2, Y>::type;
			};

			template <typename T1, typename T2>
			using _multiply = pat::tmp::for_loop<mp_maximum, find__multiply<T1,T2>::template action, pat::tmp::dec>;
		}
		
		template <typename ... Args>
		using multiply =
		//	pat::tmp::binary_compact<
		//		detail::___multiply,
		//		detail::multiply_t,
				pat::tmp::binary_split<
					detail::_multiply,
					Args ...
				>;
		//	>;
				
		namespace detail {
			// Add Sort Priority
			template <typename T>
			struct __asp;
			
			template <typename F, typename G>
			struct ___add;
			
			template <typename T, typename U, int X>
			struct __add;
			
			enum __add_priority {
				ap_maximum  = 6,
				ap_zeroes   = ap_maximum,
				ap_repair   = ap_zeroes - 1,
				ap_simplify = ap_repair - 1,
				ap_sort     = ap_simplify - 1,
				ap_lowest   = ap_sort - 1
			};
			
			template <typename T1, typename T2>
			struct find__add {
				template <int Y> using action = typename __add<T1, T2, Y>::type;
			};

			template <typename T1, typename T2>
			using _add = pat::tmp::for_loop<ap_maximum, find__add<T1,T2>::template action, pat::tmp::dec>;
		}
		
		template <typename ... Args>
		using add =
		//	pat::tmp::binary_compact<
		//		detail::___add,
		//		detail::add_t,
				pat::tmp::binary_split<
					detail::_add,
					Args ...
				>;
		//	>;
				
		namespace detail {
			template <typename X>
			struct __exp;
			
			template <typename X>
			struct _exp;
			
			template <typename X>
			struct __ln;
			
			template <typename X>
			struct _ln;
		}
		
		template <typename X>
		using ln = typename detail::_ln<X>::type;
		
		template <typename X>
		using exp = typename detail::_exp<X>::type;
		
		namespace detail {
			template <typename X, typename N>
			struct ___pow;
			
			template <typename T1, typename T2, int Priority>
			struct __pow;
			
			enum __pow_priority {
				pp_maximum  = 1,
				pp_zeroone  = pp_maximum,
				pp_lowest   = pp_zeroone - 1
			};
			
			template <typename T1, typename T2>
			struct find__pow {
				template <int Y> using action = typename __pow<T1, T2, Y>::type;
			};

			template <typename T1, typename T2>
			using _pow = pat::tmp::for_loop<pp_maximum, find__pow<T1,T2>::template action, pat::tmp::dec>;
		}
		
		template <typename X, typename N>
		using pow = detail::_pow<X,N>;
	
		template <typename X>
		using sqrt = detail::_pow<X,rational<1,2>>;
		
		namespace detail {
			template <typename F, typename G>
			struct __sub;
			
			template <typename F, typename G>
			struct _sub;
		}
		
		template <typename A, typename B>
		using sub = add<A, multiply<rational<-1>, B>>;
		
		
		
		template <typename F>
		using negate = multiply<rational<-1>, F>;
		
		template <typename X>
		using inverse = pow<X, rational<-1>>;
		
		template <typename X, typename Y>
		using divide = multiply<X, inverse<Y>>;
		
		template <typename X>
		using factorial = gamma<add<X,rational<1>>>;
		
		template <typename N, typename R>
		using binomial_coef = divide<factorial<N>, multiply<factorial<R>, factorial<sub<N,R>>>>;
	}
}

#endif

