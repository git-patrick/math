//
//  algorithm.h
//  math
//
//  Created by Patrick Sauter on 6/5/14.
//  Copyright (c) 2014 Patrick Sauter. All rights reserved.
//

#ifndef math_algorithm_h
#define math_algorithm_h

namespace math {
	// --------------------------------------------------
	// ROW REDUCE
	/*
	class soe_status {
	public:
		static constexpr int determinate = 0;
		static constexpr int indeterminate = 1;
		static constexpr int inconsistent = 2;
		
		operator bool() { return _value == determinate; }
		
		soe_status & operator=(int value) { _value = value; return *this; }
		
		bool operator==(int value) { return _value == value; }
		bool operator!=(int value) { return _value != value; }
		
		int value() const { return _value; }
	private:
		int _value;
	};
	

	
	
	class row_compare {
	public:
		bool operator()(row_type const & a, row_type const & b) {
			return std::lexicographical_compare(
				a.begin(), a.end(),
				b.begin(), b.end(),
				std::greater<typename row_type::value_type>()
			);
		}
	};
	
	template <typename T, std::size_t N>
	template <typename Y>
	void soe<T,N>::row_status::operator()(Y const & y) {
		_nonzero = -1;
		_status = soe_status::determinate;
		
		int i;
		
		for (i = static_cast<int>(_search_begin); i < N + 1; ++i) {
			if (y[i] != 0) {
				if (_nonzero < 0)
					_nonzero = i;
				else
					break;
			}
		}
		
		if (_nonzero == -1 || i < N)
			_status = soe_status::indeterminate;
		else if (_nonzero == N)
			_status = soe_status::inconsistent;
	}
	

	std::ostream & operator<<(std::ostream& o, soe_status a) {
		switch (a.value()) {
		case soe_status::indeterminate:
			return o << "indeterminate";
		case soe_status::determinate:
			return o << "determinate";
		case soe_status::inconsistent:
			return o << "inconsistent";
		default:
			return o << "invalid soe::status";
		}
	}
	*/
	
	
	

	template <typename RowType>
	class row_reducer {
	private:
		typedef RowType row_type;
		typedef typename row_type::iterator iterator;
		typedef typename row_type::reference reference;
	
		iterator _nonzero;
	public:
		void operator()(row_type const & t) {
			_nonzero = std::find_if(t.begin(), t.end(), [](reference t) {
				return t != 0;
			});
		}
	};

	
	// Iterator is a forward output iterator with a random access container for value_type
	template <typename Iterator>
	void row_reduce(Iterator begin, Iterator end) {
		for (auto r = begin; r != end; ++r) {
			typedef typename std::iterator_traits<decltype(r->begin())>::reference reference;
			
			// it would be nice to store this in something (row_reducer object) for the row, if available.
			auto first_nonzero = std::find_if(r->begin(), r->end(), [](reference t) {
				return t != 0;
			});
			
			if (first_nonzero != r->end()) {
				auto scale = *first_nonzero;
				auto index = first_nonzero - r->begin();
				
				if (scale != 1) {
					std::transform(
						r->begin(), r->end(),
						r->begin(), [scale](reference t) {
							return t / scale;
						}
					);
				}
				
				for (auto i = begin; i != end; ++i) {
					if (i == r) continue;
					
					auto adjust	= (*i)[index];
					
					if (adjust) {
						std::transform(
							r->begin(), r->end(),
							i->begin(), i->begin(),
							[adjust](reference a, reference b) {
								return b - adjust * a;
							}
						);
					}
				}
			}
		}
	}
}

#endif
