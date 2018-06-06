#pragma once

#include <functional>
#include "Joins0.h"



template <typename IteratorR, typename Consumer>
void joinBySStart(const Relation& R, const Relation& S, IteratorR itR, const Consumer& consumer) noexcept
{
	joinByS(R, S, itR, makeFilteringIterator(S.getIndex(), Endpoint::Type::START), std::less_equal<>(), consumer);
}


template <typename IteratorR, typename Consumer>
void joinBySStartStrict(const Relation& R, const Relation& S, IteratorR itR, const Consumer& consumer) noexcept
{
	joinByS(R, S, itR, makeFilteringIterator(S.getIndex(), Endpoint::Type::START), std::less<>(), consumer);
}


template <typename IteratorR, typename Consumer>
void joinBySEnd(const Relation& R, const Relation& S, IteratorR itR, const Consumer& consumer) noexcept
{
	joinByS(R, S, itR, makeFilteringIterator(S.getIndex(), Endpoint::Type::END), std::less<>(), consumer);
}


template <typename IteratorR, typename Consumer>
void joinBySEndStrict(const Relation& R, const Relation& S, IteratorR itR, const Consumer& consumer) noexcept
{
	joinByS(R, S, itR, makeFilteringIterator(S.getIndex(), Endpoint::Type::END), std::less_equal<>(), consumer);
}
