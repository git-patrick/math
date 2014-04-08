//
//  dihedral.h
//  math
//
//  Created by Patrick Sauter on 3/31/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_dihedral_h
#define math_dihedral_h

#include <iostream>
#include "core.h"

// elements of dihedral group D(2N) are math::groups::D2<N>

namespace math {
	namespace groups {
		template <int N>
		struct D2 {
			D2(bool s, int r) : _s(s), _r((r + N) % N) { };
			
			explicit D2(math::additive_identity_tag) : _s(false), _r(0) { }
			
			bool	get_s() const { return _s; }
			int		get_r() const { return _r; }
		private:
			bool	_s;
			int		_r;
		};
		
		template <int N>
		bool operator==(D2<N> const & a, D2<N> const & b) {
			return (a.get_s() == b.get_s() && a.get_r() == b.get_r());
		}
		template <int N>
		bool operator!=(D2<N> const & a, D2<N> const & b) {
			return !(a == b);
		}
		
		template <int N>
		D2<N>& operator+=(D2<N> & a, D2<N> const & b) {
			a = a + b;
			
			return a;
		}
		template <int N>
		D2<N>& operator-=(D2<N> & a, D2<N> const & b) {
			a = a - b;
			
			return a;
		}
		
		template <int N>
		D2<N> operator+(D2<N> const & a, D2<N> const & b) {
			auto s = b.get_s();
			
			return D2<N>(a.get_s() != s, b.get_r() + (s ? -a.get_r() : a.get_r()));
		}
		template <int N>
		D2<N> operator-(D2<N> const & a, D2<N> const & b) {
			return a + (-b);
		}
		template <int N>
		D2<N> operator-(D2<N> const & a) {
			auto s = a.get_s();
			
			return D2<N>(s, (s ? a.get_r() : N - a.get_r()));
		}
	}
}

namespace std {
	template <int N>
	ostream & operator<<(ostream& o, math::groups::D2<N> const & a) {
		return o << "D(" << 2 * N << ")(" << a.get_s() << ", " << a.get_r() << ")";
	}
}
#endif
