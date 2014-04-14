//
//  bessel.h
//  math
//
//  Created by Patrick Sauter on 4/13/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_bessel_h
#define math_bessel_h

#include "analytic.h"
#include "derivative.h"
#include "tuple_help.h"

namespace math {
	namespace bessel {
		namespace detail {
			template <typename S>
			struct __j;
			
			template <int X, int ... S>
			struct __j<pat::integer_sequence<X, S ...>> {
				typedef multiply<inverse<x>, D<typename __j<pat::integer_sequence<S ...>>::type,x>> type;
			};
			template <int X>
			struct __j<pat::integer_sequence<X>> {
				typedef multiply<inverse<x>, D<multiply<sin<x>, inverse<x>>,x>> type;
			};
			template <>
			struct __j<pat::integer_sequence<>> {
				typedef multiply<sin<x>, inverse<x>> type;
			};
			
			template <std::size_t N>
			struct _j {
				typedef typename detail::__j<pat::sequence_to<N>>::type type;
			};
			
			template <>
			struct _j<0> {
				typedef multiply<sin<x>, inverse<x>> type;
			};
		}
		
		template <std::size_t N>
		using j = multiply<pow<negate<x>, rational<N>>, typename detail::_j<N>::type>;
	}
}

#endif
