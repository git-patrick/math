//
//  derivative.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_derivative_h
#define math_derivative_h

#include "analytic.h"
#include "numeric_derivative.h"

namespace math {
	namespace analytic {
		namespace detail {
			constexpr int derivative_max_stage       = 2;
			constexpr int derivative_analytic_stage  = derivative_max_stage;
			constexpr int derivative_numeric_stage   = derivative_analytic_stage - 1;

			template <typename T, typename dx, int priority>
			struct __D;
			
			template <typename T, std::size_t N>
			struct __D<T, pat::select<N>, derivative_numeric_stage> {
				template <typename __T=T> using type = numeric_derivative<__T, 1, N>;
			};
			
			template <typename T, typename U, int X>
			struct _D;
			template <typename T, typename U>
			struct _D<T, U, 0>;
			
			template <typename T, typename U, int X>
			struct _D {
				template <typename G, typename H, int Y> static typename __D<G,H,Y>::template type<> test(typename __D<G,H,Y>::template type<>* a);
				template <typename G, typename H, int Y> static typename _D<G,H,Y-1>::template type<> test(...);

				template <typename G=T>
				using type = decltype(test<G,U,X>(0));
			};
		}
		
		template <typename T, typename dx>
		using D = typename detail::_D<T, dx, detail::derivative_max_stage>::template type<>;
		
		namespace detail {
			template <std::intmax_t N, std::intmax_t D, std::intmax_t iN, std::intmax_t iD, std::size_t A>
			struct __D<complex<N,D,iN,iD>, pat::select<A>, derivative_analytic_stage> {
				template <std::intmax_t __T=N> using type = rational<0>;
			};
			
			template <std::size_t A>
			struct __D<pat::select<A>, pat::select<A>, derivative_analytic_stage> {
				template <std::size_t __T=A> using type = rational<1>;
			};
			
			template <std::size_t A, std::size_t B>
			struct __D<pat::select<A>, pat::select<B>, derivative_analytic_stage> {
				template <std::size_t __T=A> using type = rational<0>;
			};
			
			// product rule
			template <typename F, typename G, typename dx>
			struct __D<___multiply<F,G>, dx, derivative_analytic_stage> {
				template <typename __T=F> using type = add<multiply<D<__T,dx>,G>, multiply<__T, D<G,dx>>>;
			};
			
			// derivative linear
			template <typename F, typename G, typename dx>
			struct __D<___add<F,G>, dx, derivative_analytic_stage> {
				template <typename __T=F> using type = add<D<__T,dx>, D<G,dx>>;
			};
			
			template <typename F, typename G, typename dx>
			struct __D<___pow<F,G>, dx, derivative_analytic_stage> {
				template <typename __T=F> using type = add<
					multiply<
						D<G, dx>,
						ln<__T>,
						pow<__T,G>
					>,
					multiply<
						D<__T, dx>,
						G,
						pow<
							__T,
							add<
								G,
								rational<-1>
							>
						>
					>
				>;
			};
			
			template <typename T, typename dx>
			struct __D<__sin<T>, dx, derivative_analytic_stage> {
				template <typename __T=T> using type = multiply<cos<__T>, D<__T, dx>>;
			};
			
			template <typename T, std::size_t N, std::size_t X>
			struct __D<numeric_derivative<T, N, X>, pat::select<X>, derivative_analytic_stage> {
				template <typename __T=T> using type = numeric_derivative<T, N + 1, X>;
			};
			
			template <typename T, typename dx>
			struct __D<__cos<T>, dx, derivative_analytic_stage> {
				template <typename __T=T> using type = multiply<rational<-1>, sin<__T>, D<__T, dx>>;
			};
			
			template <typename T, typename dx>
			struct __D<__tan<T>, dx, derivative_analytic_stage> {
				template <typename __T=T> using type = multiply<cos<T>, D<__T, dx>>;
			};
		
			template <typename T, typename dx>
			struct __D<__asin<T>, dx, derivative_analytic_stage> {
				template <typename __T=T> using type = multiply<pow<sub<rational<1>, pow<__T,rational<2>>>, rational<-1,2>>, D<__T,dx>>;
			};
			
			template <typename T, typename dx>
			struct __D<__acos<T>, dx, derivative_analytic_stage> {
				template <typename __T=T> using type = multiply<rational<-1>, pow<sub<rational<1>, pow<__T,rational<2>>>, rational<-1,2>>, D<__T,dx>>;
			};
			
			template <typename T, typename dx>
			struct __D<__atan<T>, dx, derivative_analytic_stage> {
				template <typename __T=T> using type = multiply<pow<add<rational<1>, pow<__T,rational<2>>>, rational<-1>>, D<__T,dx>>;
			};
			
			
			template <typename T, typename dx>
			struct __D<__exp<T>, dx, derivative_analytic_stage> {
				template <typename __T=T> using type = multiply<__exp<__T>, D<__T, dx>>;
			};
			
			template <typename T, typename dx>
			struct __D<__ln<T>, dx, derivative_analytic_stage> {
				template <typename __T=T> using type = multiply<___pow<__T, rational<-1>>, D<__T, dx>>;
			};
			

			template <typename F, typename ... G, typename dx>
			struct __D<pat::compose<F, G ...>, dx, derivative_analytic_stage> {
			private:
				template <typename _F, typename S>
				struct _detail;
				
				template <typename _F, int ... S>
				struct _detail<_F, pat::integer_sequence<S ...>> {
					template <typename __F = _F> using type = add<multiply<pat::compose<D<__F, pat::select<S>>, G...>, D<G, dx>>...>;
				};
			public:
				template <typename __T=F> using type = typename _detail<F, pat::index_sequence_for<G ...>>::template type<>;
			};
		}
	}
}


#endif
