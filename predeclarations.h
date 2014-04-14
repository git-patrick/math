//
//  predeclarations.h
//  math
//
//  Created by Patrick Sauter on 4/13/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_predeclarations_h
#define math_predeclarations_h

namespace math {
	namespace detail {
		template <typename F, typename G>
		struct ___add;
		
		template <typename F, typename G>
		struct ___multiply;
		
		template <typename ... Args>
		struct _split_mult;
		
		template <typename ... Args>
		struct _split_add;
	}
	
	template <typename ... Args>
	using multiply = typename detail::_split_mult<Args ...>::template type<>;

	template <typename ... Args>
	using add = typename detail::_split_add<Args ...>::template type<>;
}

#endif
