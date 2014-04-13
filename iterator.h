#ifndef _iterator_h
#define _iterator_h

#include <iterator>

namespace pat {
	template <typename Iter, typename it_type>
	struct if_iterator_satisfies {
		typedef typename std::enable_if<std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, it_type>::value>::type type;
	};
}

#endif