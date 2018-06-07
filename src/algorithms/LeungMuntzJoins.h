#pragma once

#include <algorithm>
#include <containers/GaplessList.h>
#include "model/Relation.h"


#ifdef COUNTERS
static unsigned long long lmCounterBeforeSelection   = 0;
static unsigned long long lmCounterAfterSelection    = 0;
static unsigned long long lmCounterActiveXCount      = 0;
static unsigned long long lmCounterActiveYCount      = 0;
static unsigned long long lmCounterActiveXCountTimes = 0;
static unsigned long long lmCounterActiveYCountTimes = 0;
static size_t lmCounterActiveXMax    = 0;
static size_t lmCounterActiveYMax    = 0;
#endif


#define LM_VERBOSE


template <typename ShouldReadX,
          typename ShouldReadY,
          typename CleanupTestForX,
          typename CleanupTestForY,
          typename CheckingConsumer>
void leungMuntzJoin(const Relation& X,
                    const Relation& Y,
                    const ShouldReadX& shouldReadX,
                    const ShouldReadY& shouldReadY,
                    const CleanupTestForX& cleanupTestForX,
                    const CleanupTestForY& cleanupTestForY,
                    const CheckingConsumer& checkingConsumer) noexcept
{
//	auto tauX = 1 + static_cast<Timestamp>(( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
//	auto tauY = 1 + static_cast<Timestamp>(( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;
	auto tauX = 1 + static_cast<Timestamp>(round( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
	auto tauY = 1 + static_cast<Timestamp>(round( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;

	GaplessList<Tuple> workspaceY;
	GaplessList<Tuple> workspaceX;

//	static int a = 0;
//	static int b = 0;


	auto bufferX = X.begin();
	auto bufferY = Y.begin();

	bool shouldReadYNextIteration = false;

	#ifdef LM_VERBOSE
	std::cout << std::endl;
	#endif

	for (;;)
	{
		unsigned potentialPrune = 0;

		if (shouldReadYNextIteration)
		{
			#ifdef LM_VERBOSE
			std::cout << "  Reading Y " << *bufferY << std::endl;
			#endif
			auto x = workspaceX.begin();
			while (x != workspaceX.end())
			{
				checkingConsumer(*x, *bufferY);

				if (cleanupTestForX(*x, bufferY->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase X " << *x << std::endl;
					#endif
					workspaceX.erase(x);
				}
				else
				{
					if (cleanupTestForX(*x, bufferY->start + tauY))
						potentialPrune++;

					++x;
				}
			}

			workspaceY.insert(*bufferY);
			++bufferY;
		}
		else
		{
			#ifdef LM_VERBOSE
			std::cout << "  Reading X " << *bufferX << std::endl;
			#endif

			auto y = workspaceY.begin();
			while (y != workspaceY.end())
			{
				checkingConsumer(*bufferX, *y);

				if (cleanupTestForY(*y, bufferX->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase Y " << *y << std::endl;
					#endif
					workspaceY.erase(y);
				}
				else
				{
					if (cleanupTestForY(*y, bufferX->start + tauX))
						potentialPrune++;

					++y;
				}
			}

			workspaceX.insert(*bufferX);
			++bufferX;
		}


		if (bufferX == X.end())
		{
			if (workspaceX.empty() || bufferY == Y.end())
			{
//				std::cout << a << ' ' << b << std::endl;
				return;
			}
			shouldReadYNextIteration = true;
		}
		else
		if (bufferY == Y.end())
		{
			if (workspaceY.empty())
			{
//				std::cout << a << ' ' << b << std::endl;
				return;
			}
			shouldReadYNextIteration = false;
		}
		else
		if (shouldReadX(*bufferX, *bufferY))
		{
			shouldReadYNextIteration = false;
		}
		else
		if (shouldReadY(*bufferX, *bufferY))
		{
			shouldReadYNextIteration = true;
		}
		else
		{
			unsigned potentialPruneX = 0;
			unsigned potentialPruneY = 0;

			if (shouldReadYNextIteration)
			{
//				a ++;
//				std::cout << 1 << std::endl;
				potentialPruneX = potentialPrune;
				for (const auto& y : workspaceY)
					if (cleanupTestForY(y, bufferX->start + tauX))
						potentialPruneY++;
			}
			else
			{
//				std::cout << 2 << std::endl;
//				b++;

				potentialPruneY = potentialPrune;
				for (const auto& x : workspaceX)
					if (cleanupTestForX(x, bufferY->start + tauY))
						potentialPruneX++;
			}

			#ifdef LM_VERBOSE
			std::cout << "POTENTIAL PRUNE X " << potentialPruneX << " and Y " << potentialPruneY << std::endl;
			#endif

//			if (potentialPruneX == potentialPruneY)
//				shouldReadYNextIteration = !shouldReadYNextIteration;
//			else
				shouldReadYNextIteration = potentialPruneX > potentialPruneY;
		}

		#ifdef COUNTERS
		lmCounterActiveXCount += workspaceX.size();
		lmCounterActiveYCount += workspaceY.size();
		lmCounterActiveXCountTimes++;
		lmCounterActiveYCountTimes++;
		lmCounterActiveXMax = std::max(lmCounterActiveXMax, workspaceX.size());
		lmCounterActiveYMax = std::max(lmCounterActiveYMax, workspaceY.size());
		#endif
	}
}





template <typename Consumer>
void leungMuntzReverseDuringStrictJoin2(const Relation& X, const Relation& Y, const Consumer& consumer) noexcept
{
	leungMuntzJoin(X, Y,
		[] (const Tuple&        , const Tuple&        ) { return false;                         },
		[] (const Tuple& bufferX, const Tuple& bufferY) { return bufferY.start < bufferX.start; },
		[] (const Tuple& x, Timestamp startY) { return x.end   < startY; },
		[] (const Tuple& y, Timestamp startX) { return y.start < startX; },
		[&consumer] (const Tuple& x, const Tuple& y)
		{
			#ifdef COUNTERS
			lmCounterBeforeSelection++;
			#endif

			if (x.start < y.start && y.end < x.end)
			{
				#ifdef COUNTERS
				lmCounterAfterSelection++;
				#endif

				consumer(x, y);
			}
		}
	);
}


template <typename Consumer>
void leungMuntzStartPrecedingStrictJoin2(const Relation& X, const Relation& Y, const Consumer& consumer) noexcept
{
	leungMuntzJoin(X, Y,
		[] (const Tuple&        , const Tuple&        ) { return false;                         },
		[] (const Tuple& bufferX, const Tuple& bufferY) { return bufferY.start < bufferX.start; },
		[] (const Tuple& x, Timestamp startY) { return x.end   < startY; },
		[] (const Tuple& y, Timestamp startX) { return y.start < startX; },
		[&consumer] (const Tuple& x, const Tuple& y)
		{
			#ifdef COUNTERS
			lmCounterBeforeSelection++;
			#endif

			if (x.start < y.start && x.end > y.start)
			{
				#ifdef COUNTERS
				lmCounterAfterSelection++;
				#endif

				consumer(x, y);
			}
		}
	);
}


template <typename Consumer>
void leungMuntzReverseDuringStrictJoin(const Relation& X, const Relation& Y, const Consumer& consumer) noexcept
{
	auto consumeIfOK = [&consumer] (const Tuple& x, const Tuple& y)
	{
		if (x.start < y.start && y.end < x.end)
			consumer(x, y);
	};


//	static int a = 0;
//	static int b = 0;

	double tauX = (X[X.size() - 1].start - X[0].start) / (double) (X.size() - 1);
	double tauY = (Y[Y.size() - 1].start - Y[0].start) / (double) (Y.size() - 1);

	GaplessList<Tuple> workspaceY;
	GaplessList<Tuple> workspaceX;

	auto bufferX = X.begin();
	auto bufferY = Y.begin();

	bool shouldReadY = false;

	for (;;)
	{
		unsigned potentialPrune = 0;

		if (shouldReadY)
		{
			auto x = workspaceX.begin();
			while (x != workspaceX.end())
			{
				consumeIfOK(*x, *bufferY);

				if (x->end < bufferY->start)           //   XXXXXX
					workspaceX.erase(x);               //           yyyy
				else
				{
					if (x->end < bufferY->start + static_cast<Timestamp>(tauY))
						potentialPrune++;
					++x;
				}
			}

			workspaceY.insert(*bufferY);
			++bufferY;
		}
		else
		{
			auto y = workspaceY.begin();
			while (y != workspaceY.end())
			{
				consumeIfOK(*bufferX, *y);

				if (y->start < bufferX->start)         //      xxxxx
					workspaceY.erase(y);               //   YYYYYY....
				else
				{
					if (y->start < bufferX->start + static_cast<Timestamp>(tauX))
						potentialPrune++;
					++y;
				}
			}

			workspaceX.insert(*bufferX);
			++bufferX;
		}


		if (bufferX == X.end())
		{
			if (workspaceX.empty() || bufferY == Y.end())
			{
//				std::cout << a << ' ' << b << std::endl;
				return;
			}
			shouldReadY = true;
		}
		else
		if (bufferY == Y.end())
		{
			if (workspaceY.empty())
			{
//				std::cout << a << ' ' << b << std::endl;
				return;
			}
			shouldReadY = false;
		}
		else
		if (bufferY->start < bufferX->start)
		{
			shouldReadY = true;
		}
		else
		{
			unsigned potentialPruneX = 0;
			unsigned potentialPruneY = 0;

			if (shouldReadY)
			{
//				a++;
				potentialPruneX = potentialPrune;
				for (const auto& y : workspaceY)
					if (y.start < bufferX->start + static_cast<Timestamp>(tauX))
						potentialPruneY++;
			}
			else
			{
//				b++;
				potentialPruneY = potentialPrune;
				for (const auto& x : workspaceX)
					if (x.end < bufferY->start + static_cast<Timestamp>(tauY))
						potentialPruneX++;
			}

			shouldReadY = potentialPruneX > potentialPruneY;
		}
	}



}


template <typename Consumer>
void leungMuntzStartPrecedingStrictJoin(const Relation& X, const Relation& Y, const Consumer& consumer) noexcept
{
	auto consumeIfOK = [&consumer] (const Tuple& x, const Tuple& y)
	{
		if (x.start < y.start && x.end > y.start)
			consumer(x, y);
	};

	double tauX = (X[X.size() - 1].start - X[0].start) / ((double) X.size() - 1.0);
	double tauY = (Y[Y.size() - 1].start - Y[0].start) / ((double) Y.size() - 1.0);

	GaplessList<Tuple> workspaceY;
	GaplessList<Tuple> workspaceX;

	auto bufferX = X.begin();
	auto bufferY = Y.begin();

	bool shouldReadY = false;

	for (;;)
	{
		unsigned potentialPrune = 0;

		if (shouldReadY)
		{
			auto x = workspaceX.begin();
			while (x != workspaceX.end())
			{
				consumeIfOK(*x, *bufferY);

				if (x->end < bufferY->start)           //   XXXXXX
					workspaceX.erase(x);               //           yyyy
				else
				{
					if (x->end < bufferY->start + static_cast<Timestamp>(tauY))
						potentialPrune++;
					++x;
				}
			}

			workspaceY.insert(*bufferY);
			++bufferY;
		}
		else
		{
			auto y = workspaceY.begin();
			while (y != workspaceY.end())
			{
				consumeIfOK(*bufferX, *y);

				if (y->start < bufferX->start)         //      xxxxx
					workspaceY.erase(y);               //   YYYYYY....
				else
				{
					if (y->start < bufferX->start + static_cast<Timestamp>(tauX))
						potentialPrune++;
					++y;
				}
			}

			workspaceX.insert(*bufferX);
			++bufferX;
		}


		if (bufferX == X.end())
		{
			if (workspaceX.empty() || bufferY == Y.end())
				return;
			shouldReadY = true;
		}
		else
		if (bufferY == Y.end())
		{
			if (workspaceY.empty())
				return;
			shouldReadY = false;
		}
		else
		if (bufferY->start < bufferX->start)
		{
			shouldReadY = true;
		}
		else
		{
			unsigned potentialPruneX = 0;
			unsigned potentialPruneY = 0;

			if (shouldReadY)
			{
				potentialPruneX = potentialPrune;
				for (const auto& y : workspaceY)
					if (y.start < bufferX->start + static_cast<Timestamp>(tauX))
						potentialPruneY++;
			}
			else
			{
				potentialPruneY = potentialPrune;
				for (const auto& x : workspaceX)
					if (x.end < bufferY->start + static_cast<Timestamp>(tauY))
						potentialPruneX++;
			}

			shouldReadY = potentialPruneX > potentialPruneY;
		}
	}



}

template <typename Consumer>
void leungMuntzReverseDuringStrictJoinOld(const Relation& X, const Relation& Y, const Consumer& consumer) noexcept
{
	auto consumeIfCovers = [&consumer] (const Tuple& x, const Tuple& y)
	{
		if (x.start < y.start && y.end < x.end)
			consumer(x, y);
	};

	GaplessList<Tuple> workspaceY;
	GaplessList<Tuple> workspaceX;

	auto bufferX = X.begin();
	auto bufferY = Y.begin();

	while (bufferX != X.end() || bufferY != Y.end())
	{
		if (bufferX == X.end() || (bufferY != Y.end() && bufferY->start < bufferX->start))
		{
			auto x = workspaceX.begin();
			while (x != workspaceX.end())
			{
				consumeIfCovers(*x, *bufferY);

				if (x->end < bufferY->start)           //   XXXXXX
					workspaceX.erase(x);               //           yyyy
				else
					++x;
			}

			workspaceY.insert(*bufferY);
			++bufferY;
		}
		else
		{
			auto y = workspaceY.begin();
			while (y != workspaceY.end())
			{
				consumeIfCovers(*bufferX, *y);

				if (y->start < bufferX->start)         //      xxxxx
					workspaceY.erase(y);               //   YYYYYY....
				else
					++y;
			}

			workspaceX.insert(*bufferX);
			++bufferX;
		}



	}



}

