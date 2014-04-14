//
//  select.h
//  math
//
//  Created by Patrick Sauter on 4/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_select_h
#define math_select_h

namespace math {
	template <std::size_t N>
	struct select {
	private:
		template <int index, typename T, typename ... R>
		struct _select : _select<index - 1, R ...> {
			template <typename U, typename ... P>
			_select(U && t, P && ... r) : _select<index - 1, R ...>(r ...) { }
		};
		
		template <typename T, typename ... R>
		struct _select<0, T, R ...> {
			template <typename U, typename ... P>
			_select(U && u, P && ... r) : value(u) { }
			
			typename std::decay<T>::type const & value;
		};
	public:
		static constexpr int multiply_sort_priority = 3;
		
		template <typename ... Args>
		auto operator()(Args && ... a) const -> decltype(_select<N, Args ...>(a ...).value) {
			return _select<N, Args ...>(a ...).value;
		}
	};
	
	using x = select<0>;
	using y = select<1>;
	using z = select<2>;
	
	std::ostream & operator << (std::ostream & o, select<0> const & m) {
		return o << "x";
	}
	std::ostream & operator << (std::ostream & o, select<1> const & m) {
		return o << "y";
	}
	std::ostream & operator << (std::ostream & o, select<2> const & m) {
		return o << "z";
	}
	template <std::size_t N>
	std::ostream & operator << (std::ostream & o, select<N> const & m) {
		return o << "#" << N;
	}
}

#endif
