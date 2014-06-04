//
//  trig.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_trig_h
#define math_trig_h

#include <cmath>

namespace math {
	namespace analytic {
		namespace detail {
			template <typename X>
			struct __cos {
			private:
				X _x;
			public:
				X get() const { return _x; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::cos(_x(a ...))) {
					return std::cos(_x(a ...));
				}
			};
			
			template <typename X>
			struct __acos {
			private:
				X _x;
			public:
				X get() const { return _x; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::acos(_x(a ...))) {
					return std::acos(_x(a ...));
				}
			};
			
			template <typename X>
			struct _cos {
				typedef __cos<X> type;
			};
			template <typename X>
			struct _cos<__acos<X>> {
				typedef X type;
			};
			
			template <typename X>
			struct _acos {
				typedef __acos<X> type;
			};
			template <typename X>
			struct _acos<__cos<X>> {
				typedef X type;
			};
		}
		
		template <typename X>
		using cos = typename detail::_cos<X>::type;
		
		template <typename X>
		using acos = typename detail::_acos<X>::type;
		
		
		
		namespace detail {
			template <typename X>
			struct __sin {
			private:
				X _x;
			public:
				X get() const { return _x; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::sin(_x(a ...))) {
					return std::sin(_x(a ...));
				}
			};
			
			template <typename X>
			struct __asin {
			private:
				X _x;
			public:
				X get() const { return _x; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::asin(_x(a ...))) {
					return std::asin(_x(a ...));
				}
			};
			
			template <typename X>
			struct _sin {
				typedef __sin<X> type;
			};
			template <typename X>
			struct _sin<__asin<X>> {
				typedef X type;
			};
			
			template <typename X>
			struct _asin {
				typedef __asin<X> type;
			};
			template <typename X>
			struct _asin<__sin<X>> {
				typedef X type;
			};
		}
		
		template <typename X>
		using sin = typename detail::_sin<X>::type;
		
		template <typename X>
		using asin = typename detail::_asin<X>::type;
		
		
		
		
		namespace detail {
			template <typename X>
			struct __tan {
			private:
				X _x;
			public:
				X get() const { return _x; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::tan(_x(a ...))) {
					return std::tan(_x(a ...));
				}
			};
			
			template <typename X>
			struct __atan {
			private:
				X _x;
			public:
				X get() const { return _x; }
				
				template <typename ... Args>
				auto operator()(Args ... a) const -> decltype(std::atan(_x(a ...))) {
					return std::atan(_x(a ...));
				}
			};
			
			template <typename X>
			struct _tan {
				typedef __tan<X> type;
			};
			template <typename X>
			struct _tan<__atan<X>> {
				typedef X type;
			};
			
			template <typename X>
			struct _atan {
				typedef __atan<X> type;
			};
			template <typename X>
			struct _atan<__tan<X>> {
				typedef X type;
			};
		}
		
		template <typename X>
		using tan = typename detail::_tan<X>::type;
		
		template <typename X>
		using atan = typename detail::_atan<X>::type;
		
		
		
		
		namespace detail {
			template <typename F>
			std::ostream & operator << (std::ostream & o, __cos<F> const & m) {
				return o << "cos(" << m.get() << ")";
			}
			template <typename F>
			std::ostream & operator << (std::ostream & o, __sin<F> const & m) {
				return o << "sin(" << m.get() << ")";
			}
			template <typename F>
			std::ostream & operator << (std::ostream & o, __tan<F> const & m) {
				return o << "tan(" << m.get() << ")";
			}
			
			template <typename F>
			std::ostream & operator << (std::ostream & o, __acos<F> const & m) {
				return o << "acos(" << m.get() << ")";
			}
			template <typename F>
			std::ostream & operator << (std::ostream & o, __asin<F> const & m) {
				return o << "asin(" << m.get() << ")";
			}
			template <typename F>
			std::ostream & operator << (std::ostream & o, __atan<F> const & m) {
				return o << "atan(" << m.get() << ")";
			}
		}
	}
}

#endif
