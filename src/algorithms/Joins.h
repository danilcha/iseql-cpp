#pragma once

#include <limits>
#include "BasicJoins.h"


constexpr Timestamp UNBOUND = std::numeric_limits<Timestamp>::max();


// This class allow writing
// ReversingConsumer<Consumer>(consumer)
// instead of
// [&consumer] (const Tuple& s, const Tuple& r) { consumer(r, s); }
template<typename Consumer>
class ReversingConsumer
{
private:
	const Consumer& consumer;

public:
	ReversingConsumer(const Consumer& consumer) noexcept : consumer(consumer) { }

	void operator()(const Tuple& s, const Tuple& r) const noexcept
	{
		consumer(r, s);
	}
};



template <typename Consumer>
void startPrecedingJoin(const Relation& R, const Relation& S, Timestamp delta, const Consumer& consumer)
{
	startPrecedingJoin(R, S, [delta, &consumer] (const Tuple& r, const Tuple& s)
	{
		if (s.start - r.start <= delta)
			consumer(r, s);
	});
}



template <typename Consumer>
void reverseStartPrecedingJoin(const Relation& R, const Relation& S, const Consumer& consumer)
{
	startPrecedingJoin(S, R, ReversingConsumer<Consumer>(consumer));
}



template <typename Consumer>
void endFollowingJoin(const Relation& R, const Relation& S, Timestamp epsilon, const Consumer& consumer)
{
	endFollowingJoin(R, S, [epsilon, &consumer] (const Tuple& r, const Tuple& s)
	{
		if (r.end - s.end <= epsilon)
			consumer(r, s);
	});
}



template <typename Consumer>
void reverseEndFollowingJoin(const Relation& R, const Relation& S, const Consumer& consumer)
{
	endFollowingJoin(S, R, [&consumer] (const Tuple& s, const Tuple& r) { consumer(r, s); });
}



template <typename Consumer>
void leftOverlapJoin(const Relation& R, const Relation& S, const Consumer& consumer)
{
	startPrecedingJoin(R, S, [&consumer] (const Tuple& r, const Tuple& s)
	{
		if (r.end <= s.end)
			consumer(r, s);
	});
}




template <typename Consumer>
void leftOverlapJoin(const Relation& R, const Relation& S, Timestamp delta, Timestamp epsilon, const Consumer& consumer)
{
	leftOverlapJoin(R, S, [delta, epsilon, &consumer] (const Tuple& r, const Tuple& s)
	{
		if (s.start - r.start <= delta && s.end - r.end <= epsilon)
			consumer(r, s);
	});
}




template <typename Consumer>
void rightOverlapJoin(const Relation& R, const Relation& S, const Consumer& consumer)
{
	leftOverlapJoin(S, R, ReversingConsumer<Consumer>(consumer));
}




template <typename Consumer>
void rightOverlapJoin(const Relation& R, const Relation& S, Timestamp delta, Timestamp epsilon, const Consumer& consumer)
{
	leftOverlapJoin(S, R, delta, epsilon, ReversingConsumer<Consumer>(consumer));
}



template <typename Consumer>
void duringJoin(const Relation& R, const Relation& S, const Consumer& consumer)
{
	reverseStartPrecedingJoin(R, S, [&consumer] (const Tuple& r, const Tuple& s)
	{
		if (r.end <= s.end)
			consumer(r, s);
	});
}



template <typename Consumer>
void duringJoin(const Relation& R, const Relation& S, Timestamp delta, Timestamp epsilon, const Consumer& consumer)
{
	duringJoin(R, S, [delta, epsilon, &consumer] (const Tuple& r, const Tuple& s)
	{
		if (r.start - s.start <= delta && s.end - r.end <= epsilon)
			consumer(r, s);
	});
}



template <typename Consumer>
void reverseDuringJoin(const Relation& R, const Relation& S, const Consumer& consumer)
{
	duringJoin(S, R, ReversingConsumer<Consumer>(consumer));
}



template <typename Consumer>
void reverseDuringJoin(const Relation& R, const Relation& S, Timestamp delta, Timestamp epsilon, const Consumer& consumer)
{
	duringJoin(S, R, delta, epsilon, ReversingConsumer<Consumer>(consumer));
}




