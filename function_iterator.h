//
//  function_iterator.h
//  math
//
//  Created by Patrick Sauter on 4/1/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_function_iterator_h
#define math_function_iterator_h

namespace math {
	// this already exists in the form of boost::transform_iterator
	
	template <typename Iterator, std::size_t Steps = 1,
			  typename Functor = identity,
			  typename std::enable_if<
				iterator_satisfies<Iterator, std::input_iterator_tag>::value ||
				iterator_satisfies<Iterator, std::output_iterator_tag>::value
			  >::type ...>
	class function_iterator : public iterator_adjuster<Iterator, Steps> {
	protected:
		Functor _functor;
	public:
		function_iterator() : function_iterator::iterator_adjuster_() { }
		
		function_iterator(function_iterator const & p) = default;
		function_iterator& operator=(function_iterator const & p) = default;
		
		function_iterator(typename function_iterator::iterator const & value) : function_iterator::iterator_adjuster_(value) { }
		
		virtual ~function_iterator() = default;
		
		typename std::result_of<Functor>::type operator*() const {
			return _functor(*this->_value);
		}
	};
}
#endif
