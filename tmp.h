//
//  tmp.h
//  math
//
//  Created by Patrick Sauter on 4/14/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

/*  TEMPLATE METAPROGRAMMING LIBRARY    ---------- */
//	Just adding things I find useful as I need them.

#ifndef _pat_tmp_h
#define _pat_tmp_h

namespace pat {
	namespace tmp {
		namespace detail {
			template <template <typename, typename> class Operator, typename ... Args>
			struct _binary_split;
			template <template <typename, typename> class Operator, typename T>
			struct _binary_split<Operator, T> {
				typedef T result;
			};
			template <template <typename, typename> class Operator, typename T, typename U, typename ... More>
			struct _binary_split<Operator, T, U, More ...> {
				typedef Operator<T, typename _binary_split<Operator, U, More ...>::result> result;
			};
		}

		// SPLITS VARARGS INTO A SEQUENCE OF BINARY OP CALLS
		// ie, binary_split<multiply, a,b,c,d> == multiply<a, multiply<b, multiply<c,d>>>
		
		template <template <typename, typename> class Operator, typename ... Args>
		using binary_split = typename detail::_binary_split<Operator, Args ...>::result;
		
		
		
		namespace detail {
			template <typename ... Args>
			struct __append {
				typedef __append type;
			};
			template <typename Arg1, typename ... Args2>
			struct __append<Arg1, __append<Args2 ...>> {
				typedef __append<Arg1, Args2 ...> type;
			};
			template <typename ... Args>
			using _append = typename __append<Args ...>::type;
			
			
			template <template <typename ...> class Output, typename T>
			struct _devour_append;
			
			template <template <typename ...> class Output, typename ... Args>
			struct _devour_append<Output, __append<Args ...>> {
				typedef Output<Args ...> result;
			};
			
			
			template <template <typename, typename> class Operator, typename From>
			struct _devour_operator_chain {
				typedef From result;
			};
			template <template <typename, typename> class Operator, typename T1, typename T2>
			struct _devour_operator_chain<Operator, Operator<T1, T2>> {
				typedef _append<T1, typename _devour_operator_chain<Operator, T2>::result> result;
			};
		}
		
		// COMPRESSES A SEQUENCE BINARY OP CALLS, AND PASSES THE RESULTING ARGS TO Output TEMPLATE
		// ie, binary_compact<multiply, Output, multiply<a, multiply<b, multiply<c,d>>>> == Output<a,b,c,d>;
		
		template <template <typename, typename> class Operator, template <typename ...> class Output, typename T>
		using binary_compact = typename detail::_devour_append<Output, typename detail::_devour_operator_chain<Operator, T>::result>::result;
		




		
		
		
		
		namespace detail {
			template <int X, template <int> class Until, template <int> class Adjust>
			struct _next { };
			
			template <typename T>
			struct _for_return {
				typedef T type;
			};
			
			template <int X, template <int> class Until, template <int> class Adjust>
			struct _for {
				template <int Y> static Until<Y>
				test(Until<Y>* a);
				
				template <int Y> static _next<Y, Until, Adjust>
				test(...);

				typedef typename _for_return<decltype(test<X>(0))>::type type;
			};
			
			template <int X, template <int> class Until, template <int> class Adjust>
			struct _for_return<_next<X,Until,Adjust>> {
				typedef typename _for<Adjust<X>::value, Until, Adjust>::type type;
			};
		}
		
		template <int X>
		struct inc {
			static constexpr int value = X + 1;
		};
		template <int X>
		struct dec {
			static constexpr int value = X - 1;
		};
		
		
		// loops until Until<X> is valid (uses SFINAE), then returns it.  uses X = Adjust<X>::value each passthrough.
		template <int X, template <int> class Until, template <int> class Adjust>
		using for_loop = typename detail::_for<X, Until, Adjust>::type;
		
		
		
		
		
		
		namespace detail {
			// contains our binary operation to sort T1, T2 by the passed ordering, definition below.
			template <template <typename> class ordering>
			struct _sort;
			
			// result type, indicates T1 and T2 are properly sorted.
			template <typename T1, typename T2>
			struct _sorted { };
	
	
			// actually swaps the elements.  S is the result of _compare, 0 means they need to be swapped, anything else
			// and they are sorted.
			template <typename T1, typename T2, int S>
			struct _sorter {
				template <template <typename> class Order> using result = _sorted<T1, T2>;
			};
			template <typename T1, typename T2>
			struct _sorter<T1, T2, 0> {
				template <template <typename> class Order> using result = _sorted<T2, T1>;
			};
			template <typename T1, typename T2, typename T3, int S>
			struct _sorter<T1, _sorted<T2, T3>, S> {
				template <template <typename> class Order> using result = _sorted<T1, _sorted<T2, T3>>;
			};
			template <typename T1, typename T2, typename T3>
			struct _sorter<T1, _sorted<T2, T3>, 0> {
				// mainly templated so I don't have to keep passing ordering down.
				template <template <typename> class Order> using result =
					_sorted<T2, typename _sort<Order>::template op<T1, T3>>;
			};
			
			template <template <typename> class values, typename T1, typename T2>
			constexpr int __compare() { return values<T1>::value <= values<T2>::value; }
			
			template <template <typename> class values, typename T1, typename T2>
			struct _compare {
				static constexpr int value = __compare<values, T1, T2>();
			};
			template <template <typename> class values, typename T1, typename T2, typename T3>
			struct _compare<values, T1, _sorted<T2, T3>> {
				static constexpr int value = __compare<values, T1, T2>();
			};
			
			
			template <template <typename> class ordering>
			struct _sort {
				template <typename T1, typename T2>
				using op = typename _sorter<T1, T2, _compare<ordering, T1, T2>::value>::template result<ordering>;
			};
		}
		
		// split up the args into detail::_sort binary chain, then recompact them into output_type.
		template <template <typename> class ordering, template <typename ...> class output_type, typename ... Args>
		using sort = binary_compact<
			detail::_sorted,
			output_type,
			binary_split<
				detail::_sort<ordering>::template op,
				Args ...
			>
		>;
		
		
		
		
		
		
		namespace detail {
			template <std::intmax_t T, std::intmax_t ... N>
			struct _sum;
			
			template <std::intmax_t T, std::intmax_t C, std::intmax_t ... N>
			struct _sum<T,C,N...> : _sum<T+C, N...> { };
			
			template <std::intmax_t T>
			struct _sum<T> {
				static constexpr std::intmax_t value = T;
			};
		}
		
		template <std::intmax_t ... N>
		using sum = detail::_sum<N...>;
	}
}

#endif
