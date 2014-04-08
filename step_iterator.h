//
//  step_iterator.h
//  math
//
//  Created by Patrick Sauter on 3/25/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_step_iterator_h
#define math_step_iterator_h

#include <boost/iterator/iterator_adaptor.hpp>

// this iterator DOES NOT make sure that you do not go past the end.
// either the underlying iterator must handle such situations gracefully,
// or you need to set things up so that it can't happen.

namespace pat {
	using namespace boost;
	
	template <typename Iterator>
	class step_iterator : public iterator_adaptor<step_iterator<Iterator>, Iterator>
	{
		typedef iterator_adaptor<step_iterator<Iterator>, Iterator> super_t;
		
		friend class boost::iterator_core_access;
	protected:
		typename super_t::difference_type _step = 1;
	public:
		step_iterator(Iterator const & i, typename super_t::difference_type step = 1) : super_t(i), _step(step) { }
	private:
		void increment() {
			std::advance(this->base_reference(), _step);
		}
		void decrement() {
			std::advance(this->base_reference(), -_step);
		}
		void advance(typename super_t::difference_type n) {
			std::advance(this->base_reference(), n * _step);
		}
	};
}
#endif
