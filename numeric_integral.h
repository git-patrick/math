//
//  integral.h
//  math
//
//  Created by Patrick Sauter on 3/20/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_integral_h
#define math_integral_h

#include <type_traits>
#include <cmath>

#include "core.h"
#include "box.h"

namespace math {
	namespace measure {
		// these aren't ACTUALLY measures, but that is the general idea of them.
		
		// I can continue to add support for addition subsets if I find them useful, though boxes are probably going
		// to be used most often.
		
		// can't really generalize this to just use the determinant of a jacobian
		// since we aren't actually dealing with differential volumes.
		
		// coordinate vector is (r, theta, phi) where theta is polar and phi is azimuthal
		// the box is a box in (r, theta, phi) coordinate space.
		
		reals_t spherical(box<vector<reals_t,3>> const & r) {
			auto dr = r.diagonal();
			auto R = r.a();
			
			return (R[0] * R[0] * dr[0] + R[0] * dr[0] * dr[0] + dr[0] * dr[0] * dr[0] / 3) * (-std::cos(R[1] + dr[1]) + std::cos(R[1])) * dr[2];
		}
		
		template <typename Scalar, std::size_t N>
		reals_t cartesian(box<vector<Scalar, N>> const & r) {
			Scalar ret = 1;
			
			for (auto i : r.diagonal()) {
				ret *= i;
			}
			
			return ret;
		}
	}
	
	template <typename Function, typename Measure, typename Container>
	auto numeric_integral(Function f, Measure mu, Container set_container) -> decltype(f(*set_container.begin())) {
		static_assert(check::vector_space<decltype(f(*set_container.begin())), reals_t>::value,
					  "Assertion failed, return type not a vector space over the reals.");
		
		decltype(f(*set_container.begin())) sum{};
		
		for (auto i : set_container) {
			sum += f(i) * mu(i);
		}
		
		return sum;
	}
}
#endif
