#pragma once

#include <limits>
#include "Joins2.h"



constexpr Timestamp UNBOUND = std::numeric_limits<Timestamp>::max();



template<typename Consumer>
inline auto makeReversingConsumer(const Consumer& consumer) noexcept
{
	return [&consumer] (const Tuple& s, const Tuple& r) noexcept
	{
		consumer(r, s);
	};
}



template <typename Consumer>
void startPrecedingJoin(const Relation& R, const Relation& S, Timestamp delta, const Consumer& consumer) noexcept
{
	startPrecedingJoin(R, S, [delta, &consumer] (const Tuple& r, const Tuple& s)
	{
		if (s.start - r.start <= delta)
			consumer(r, s);
	});
}



template <typename Consumer>
void reverseStartPrecedingJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	startPrecedingJoin(S, R, makeReversingConsumer(consumer));
}



template <typename Consumer>
void reverseStartPrecedingStrictJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	startPrecedingStrictJoin(S, R, makeReversingConsumer(consumer));
}



template <typename Consumer>
void endFollowingJoin(const Relation& R, const Relation& S, Timestamp epsilon, const Consumer& consumer) noexcept
{
	endFollowingJoin(R, S, [epsilon, &consumer] (const Tuple& r, const Tuple& s)
	{
		if (r.end - s.end <= epsilon)
			consumer(r, s);
	});
}



template <typename Consumer>
void reverseEndFollowingJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	endFollowingJoin(S, R, makeReversingConsumer(consumer));
}



template <typename Consumer>
void reverseEndFollowingStrictJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	endFollowingStrictJoin(S, R, makeReversingConsumer(consumer));
}



template <typename Consumer>
void leftOverlapJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	startPrecedingJoin(R, S, [&consumer] (const Tuple& r, const Tuple& s)
	{
		if (r.end <= s.end)
			consumer(r, s);
	});
}




template <typename Consumer>
void leftOverlapStrictJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	startPrecedingStrictJoin(R, S, [&consumer] (const Tuple& r, const Tuple& s)
	{
		#ifdef COUNTERS
		myCounterBeforeSelection++;
		#endif

		if (r.end < s.end)
		{
			#ifdef COUNTERS
			myCounterAfterSelection++;
			#endif

			consumer(r, s);
		}
	});
}



template <typename Consumer>
void leftOverlapJoin(const Relation& R, const Relation& S, Timestamp delta, Timestamp epsilon, const Consumer& consumer) noexcept
{
	leftOverlapJoin(R, S, [delta, epsilon, &consumer] (const Tuple& r, const Tuple& s)
	{
		if (s.start - r.start <= delta && s.end - r.end <= epsilon)
			consumer(r, s);
	});
}



template <typename Consumer>
void rightOverlapJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	leftOverlapJoin(S, R, makeReversingConsumer(consumer));
}



template <typename Consumer>
void rightOverlapJoin(const Relation& R, const Relation& S, Timestamp delta, Timestamp epsilon, const Consumer& consumer) noexcept
{
	leftOverlapJoin(S, R, delta, epsilon, makeReversingConsumer(consumer));
}



template <typename Consumer>
void duringJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	reverseStartPrecedingJoin(R, S, [&consumer] (const Tuple& r, const Tuple& s)
	{
		if (r.end <= s.end)
			consumer(r, s);
	});
}



template <typename Consumer>
void reverseDuringStrictJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	startPrecedingStrictJoin(R, S, [&consumer] (const Tuple& r, const Tuple& s)
	{
		#ifdef COUNTERS
		myCounterBeforeSelection++;
		#endif

		if (s.end < r.end)
		{
			#ifdef COUNTERS
			myCounterAfterSelection++;
			#endif

			consumer(r, s);
		}
	});
}



template <typename Consumer>
void duringJoin(const Relation& R, const Relation& S, Timestamp delta, Timestamp epsilon, const Consumer& consumer) noexcept
{
	duringJoin(R, S, [delta, epsilon, &consumer] (const Tuple& r, const Tuple& s)
	{
		if (r.start - s.start <= delta && s.end - r.end <= epsilon)
			consumer(r, s);
	});
}



template <typename Consumer>
void reverseDuringJoin(const Relation& R, const Relation& S, const Consumer& consumer) noexcept
{
	duringJoin(S, R, makeReversingConsumer(consumer));
}



template <typename Consumer>
void reverseDuringJoin(const Relation& R, const Relation& S, Timestamp delta, Timestamp epsilon, const Consumer& consumer) noexcept
{
	duringJoin(S, R, delta, epsilon, makeReversingConsumer(consumer));
}



template <typename Consumer>
void afterJoin(const Relation& R, const Relation& S, Timestamp delta, const Consumer& consumer) noexcept
{
	beforeJoin(S, R, delta, makeReversingConsumer(consumer));
}
