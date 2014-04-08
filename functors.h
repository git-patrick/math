//
//  functors.h
//  math
//
//  Created by Patrick Sauter on 3/14/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_functors_h
#define math_functors_h

namespace pat {
	template <typename To>
	struct static_caster {
		template <typename From>
		To operator()(From && f) const {
			return static_cast<To>(std::forward<From>(f));
		}
	};

	struct identity {
		template <typename Type>
		constexpr auto operator()(Type && t) const -> decltype(std::forward<Type>(t)) {
			return std::forward<Type>(t);
		}
	};
}

#endif
