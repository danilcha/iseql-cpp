#pragma once

#include <functional>
#include "algorithms/internal/Joins0.h"



template <typename IteratorR, typename Consumer>
void joinBySStart(const Relation& R, const Relation& S, IteratorR itR, const Consumer& consumer) noexcept
{
	joinByS(R, S, itR, makeFilteringIterator(S.getIndex(), Endpoint::Type::START), std::less<Endpoint>(), consumer);
}



template <typename IteratorR, typename Consumer>
void joinBySEnd(const Relation& R, const Relation& S, IteratorR itR, const Consumer& consumer) noexcept
{
	joinByS(R, S, itR, makeFilteringIterator(S.getIndex(), Endpoint::Type::END), std::less_equal<Endpoint>(), consumer);
}
