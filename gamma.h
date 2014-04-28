//
//  gamma.h
//  math
//
//  Created by Patrick Sauter on 4/25/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_gamma_h
#define math_gamma_h

#include "constant.h"

namespace math {
	namespace analytic {
		namespace detail {
			template <typename X>
			struct __gamma {
			private:
				X _x;
			public:
				X get() const { return _x; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::tgamma(_x(a ...))) {
					return std::tgamma(_x(a ...));
				}
			};
			
			template <typename X>
			struct _gamma {
				typedef __gamma<X> type;
			};
			
			constexpr std::intmax_t Factorial(std::uintmax_t N) {
				return (N > 0 ? N * Factorial(N - 1) : 1);
			}
			
			template <std::uintmax_t N>
			struct _gamma<rational<N>> {
				typedef rational<Factorial(N - 1)> type;
			};
			template <>
			struct _gamma<rational<0>> {
				typedef __gamma<rational<0>> type;
			};
		}
		
		template <typename X>
		using gamma = typename detail::_gamma<X>::type;
		
		namespace detail {
			template <typename X>
			std::ostream & operator << (std::ostream & o, __gamma<X> const & m) {
				return o << "gamma(" << m.get() << ")";
			}
		}
	}
}

#endif