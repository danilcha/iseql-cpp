#pragma once

#include <algorithm>
#include <containers/GaplessList.h>
#include "model/Relation.h"


//#define LM_VERBOSE


#ifdef COUNTERS
static unsigned long long lmCounterBeforeSelection   = 0;
static unsigned long long lmCounterAfterSelection    = 0;
static unsigned long long lmCounterActiveCountX      = 0;
static unsigned long long lmCounterActiveCountY      = 0;
static unsigned long long lmCounterActiveCountTimes  = 0;
static size_t lmCounterActiveMaxX    = 0;
static size_t lmCounterActiveMaxY    = 0;
static size_t lmCounterActiveMax     = 0;
#endif



template <typename ShouldReadX,
          typename ShouldReadY,
          typename CanCleanupX,
          typename CanCleanupY,
          typename CheckingConsumer>
void leungMuntzJoin2(const Relation& X,
                    const Relation& Y,
                    const ShouldReadX& shouldReadX,
                    const ShouldReadY& shouldReadY,
                    const CanCleanupX& canCleanupX,
                    const CanCleanupY& canCleanupY,
                    const CheckingConsumer& checkingConsumer) noexcept
{
	auto tauX = 1 + static_cast<Timestamp>(( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
	auto tauY = 1 + static_cast<Timestamp>(( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;
//	auto tauX = 1 + static_cast<Timestamp>(round( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
//	auto tauY = 1 + static_cast<Timestamp>(round( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;

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
		/* READING AND JOINING */

		if (shouldReadYNextIteration)
		{
			#ifdef LM_VERBOSE
			std::cout << "  Reading Y " << *bufferY << std::endl;
			#endif

			for (const auto& x : workspaceX)
			{
				checkingConsumer(x, *bufferY);
			}

			workspaceY.insert(*bufferY);
//			++bufferY;
		}
		else
		{
			#ifdef LM_VERBOSE
			std::cout << "  Reading X " << *bufferX << std::endl;
			#endif

			for (const auto& y : workspaceY)
			{
				checkingConsumer(*bufferX, y);
			}

			workspaceX.insert(*bufferX);
//			++bufferX;
		}


		/* CLEANUP */

		unsigned potentialPruneX = 0;
		unsigned potentialPruneY = 0;

		{
			auto x = workspaceX.begin();
			while (x != workspaceX.end())
			{
				if (canCleanupX(*x, bufferY->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase X " << *x << std::endl;
					#endif
					workspaceX.erase(x);
				}
				else
				{
					if (canCleanupX(*x, bufferY->start + tauY))
						potentialPruneX++;

					++x;
				}
			}
		}

		{
			auto y = workspaceY.begin();
			while (y != workspaceY.end())
			{
				if (canCleanupY(*y, bufferX->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase Y " << *y << std::endl;
					#endif
					workspaceY.erase(y);
				}
				else
				{
					if (canCleanupY(*y, bufferX->start + tauX))
						potentialPruneY++;

					++y;
				}
			}
		}



		if (shouldReadYNextIteration)
			++bufferY;
		else
			++bufferX;


		/* DECIDING */

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
			#ifdef LM_VERBOSE
			std::cout << "POTENTIAL PRUNE X " << potentialPruneX << " and Y " << potentialPruneY << std::endl;
			#endif

//			if (potentialPruneX == potentialPruneY)
//				shouldReadYNextIteration = !shouldReadYNextIteration;
//			else
				shouldReadYNextIteration = potentialPruneX > potentialPruneY;
		}

		#ifdef COUNTERS
		lmCounterActiveCountX += workspaceX.size();
		lmCounterActiveCountY += workspaceY.size();
		lmCounterActiveCountTimes++;
		lmCounterActiveMaxX = std::max(lmCounterActiveMaxX, workspaceX.size());
		lmCounterActiveMaxY = std::max(lmCounterActiveMaxY, workspaceY.size());
		lmCounterActiveMax = std::max(lmCounterActiveMax, workspaceX.size() + workspaceY.size());
		#endif
	}
}



template <typename ShouldReadX,
          typename ShouldReadY,
          typename CanCleanupX,
          typename CanCleanupY,
          typename CheckingConsumer>
void leungMuntzJoin3(const Relation& X,
                    const Relation& Y,
                    const ShouldReadX& shouldReadX,
                    const ShouldReadY& shouldReadY,
                    const CanCleanupX& canCleanupX,
                    const CanCleanupY& canCleanupY,
                    const CheckingConsumer& checkingConsumer) noexcept
{
	auto tauX = 1 + static_cast<Timestamp>(( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
	auto tauY = 1 + static_cast<Timestamp>(( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;
//	auto tauX = 1 + static_cast<Timestamp>(round( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
//	auto tauY = 1 + static_cast<Timestamp>(round( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;

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
		unsigned potentialPruneX = 0;
		unsigned potentialPruneY = 0;

		if (shouldReadYNextIteration)
		{
			#ifdef LM_VERBOSE
			std::cout << "  Reading Y " << *bufferY << std::endl;
			#endif

			auto x = workspaceX.begin();
			while (x != workspaceX.end())
			{
				checkingConsumer(*x, *bufferY);

				if (canCleanupX(*x, bufferY->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase X " << *x << std::endl;
					#endif
					workspaceX.erase(x);
				}
				else
				{
					if (canCleanupX(*x, bufferY->start + tauY))
						potentialPruneX++;

					++x;
				}
			}

			workspaceY.insert(*bufferY);
			++bufferY;


			// We could just check for the new tuple, but we anyway have to estimate the potentialPruneY
			auto y = workspaceY.begin();
			while (y != workspaceY.end())
			{
				if (canCleanupY(*y, bufferX->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase Y " << *y << std::endl;
					#endif
					workspaceY.erase(y);
				}
				else
				{
					if (canCleanupY(*y, bufferX->start + tauX))
						potentialPruneY++;

					++y;
				}
			}
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

				if (canCleanupY(*y, bufferX->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase Y " << *y << std::endl;
					#endif
					workspaceY.erase(y);
				}
				else
				{
					if (canCleanupY(*y, bufferX->start + tauX))
						potentialPruneY++;

					++y;
				}
			}


			workspaceX.insert(*bufferX);
			++bufferX;


			auto x = workspaceX.begin();
			while (x != workspaceX.end())
			{
				if (canCleanupX(*x, bufferY->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase X " << *x << std::endl;
					#endif
					workspaceX.erase(x);
				}
				else
				{
					if (canCleanupX(*x, bufferY->start + tauY))
						potentialPruneX++;

					++x;
				}
			}
		}






		/* DECIDING */

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
			#ifdef LM_VERBOSE
			std::cout << "POTENTIAL PRUNE X " << potentialPruneX << " and Y " << potentialPruneY << std::endl;
			#endif

//			if (potentialPruneX == potentialPruneY)
//				shouldReadYNextIteration = !shouldReadYNextIteration;
//			else
				shouldReadYNextIteration = potentialPruneX > potentialPruneY;
		}

		#ifdef COUNTERS
		lmCounterActiveCountX += workspaceX.size();
		lmCounterActiveCountY += workspaceY.size();
		lmCounterActiveCountTimes++;
		lmCounterActiveMaxX = std::max(lmCounterActiveMaxX, workspaceX.size());
		lmCounterActiveMaxY = std::max(lmCounterActiveMaxY, workspaceY.size());
		lmCounterActiveMax = std::max(lmCounterActiveMax, workspaceX.size() + workspaceY.size());
		#endif
	}
}



template <typename ShouldReadX,
          typename ShouldReadY,
          typename CanCleanupX,
          typename CanCleanupY,
          typename CheckingConsumer>
void leungMuntzJoin4(const Relation& X,
                    const Relation& Y,
                    const ShouldReadX& shouldReadX,
                    const ShouldReadY& shouldReadY,
                    const CanCleanupX& canCleanupX,
                    const CanCleanupY& canCleanupY,
                    const CheckingConsumer& checkingConsumer) noexcept
{
	auto tauX = 1 + static_cast<Timestamp>(( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
	auto tauY = 1 + static_cast<Timestamp>(( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;
//	auto tauX = 1 + static_cast<Timestamp>(round( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
//	auto tauY = 1 + static_cast<Timestamp>(round( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;

	GaplessList<Tuple> workspaceY;
	GaplessList<Tuple> workspaceX;

//	static int a = 0;
//	static int b = 0;


	auto bufferX = X.begin();
	auto bufferY = Y.begin();

	bool shouldReadYNextIteration = false;

	unsigned potentialPruneX = 0;
	unsigned potentialPruneY = 0;


	#ifdef LM_VERBOSE
	std::cout << std::endl;
	#endif

	for (;;)
	{
		if (shouldReadYNextIteration)
		{
			#ifdef LM_VERBOSE
			std::cout << "  Reading Y " << *bufferY << std::endl;
			#endif

			potentialPruneX = 0;

			auto x = workspaceX.begin();
			while (x != workspaceX.end())
			{
				checkingConsumer(*x, *bufferY);

				if (canCleanupX(*x, bufferY->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase X " << *x << std::endl;
					#endif
					workspaceX.erase(x);
				}
				else
				{
					if (canCleanupX(*x, bufferY->start + tauY))
						potentialPruneX++;

					++x;
				}
			}

			if (!canCleanupY(*bufferY, bufferX->start))
			{
				workspaceY.insert(*bufferY);

				if (canCleanupY(*bufferY, bufferX->start + tauX))
					potentialPruneY++;
			}

			++bufferY;
		}
		else
		{
			#ifdef LM_VERBOSE
			std::cout << "  Reading X " << *bufferX << std::endl;
			#endif

			potentialPruneY = 0;

			auto y = workspaceY.begin();
			while (y != workspaceY.end())
			{
				checkingConsumer(*bufferX, *y);

				if (canCleanupY(*y, bufferX->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase Y " << *y << std::endl;
					#endif
					workspaceY.erase(y);
				}
				else
				{
					if (canCleanupY(*y, bufferX->start + tauX))
						potentialPruneY++;

					++y;
				}
			}


			if (!canCleanupX(*bufferX, bufferY->start))
			{
				workspaceX.insert(*bufferX);

				if (canCleanupX(*bufferX, bufferY->start + tauY))
					potentialPruneX++;
			}

			++bufferX;
		}






		/* DECIDING */

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
			#ifdef LM_VERBOSE
			std::cout << "POTENTIAL PRUNE X " << potentialPruneX << " and Y " << potentialPruneY << std::endl;
			#endif

//			if (potentialPruneX == potentialPruneY)
//				shouldReadYNextIteration = !shouldReadYNextIteration;
//			else
				shouldReadYNextIteration = potentialPruneX > potentialPruneY;
		}

		#ifdef COUNTERS
		lmCounterActiveCountX += workspaceX.size();
		lmCounterActiveCountY += workspaceY.size();
		lmCounterActiveCountTimes++;
		lmCounterActiveMaxX = std::max(lmCounterActiveMaxX, workspaceX.size());
		lmCounterActiveMaxY = std::max(lmCounterActiveMaxY, workspaceY.size());
		lmCounterActiveMax  = std::max(lmCounterActiveMax,  workspaceX.size() + workspaceY.size());
		#endif
	}
}




template <typename ShouldReadX,
          typename ShouldReadY,
          typename CanCleanupX,
          typename CanCleanupY,
          typename CheckingConsumer>
void leungMuntzJoin5(const Relation& X,
                    const Relation& Y,
                    const ShouldReadX& shouldReadX,
                    const ShouldReadY& shouldReadY,
                    const CanCleanupX& canCleanupX,
                    const CanCleanupY& canCleanupY,
                    const CheckingConsumer& checkingConsumer) noexcept
{
	auto tauX = 1 + static_cast<Timestamp>(( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
	auto tauY = 1 + static_cast<Timestamp>(( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;
//	auto tauX = 1 + static_cast<Timestamp>(round( (X[X.size() - 1].start - X[0].start) / static_cast<double>(X.size() - 1) )) ;
//	auto tauY = 1 + static_cast<Timestamp>(round( (Y[Y.size() - 1].start - Y[0].start) / static_cast<double>(Y.size() - 1) )) ;

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
		#ifdef LM_VERBOSE
		if (shouldReadYNextIteration)
			std::cout << "  Reading Y " << *bufferY << std::endl;
		else
			std::cout << "  Reading X " << *bufferX << std::endl;
		#endif


		unsigned potentialPruneX = 0;
		unsigned potentialPruneY = 0;

		{
			auto x = workspaceX.begin();
			while (x != workspaceX.end())
			{
				if (shouldReadYNextIteration)
					checkingConsumer(*x, *bufferY);


				if (canCleanupX(*x, bufferY->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase X " << *x << std::endl;
					#endif
					workspaceX.erase(x);
				}
				else
				{
					if (canCleanupX(*x, bufferY->start + tauY))
						potentialPruneX++;

					++x;
				}
			}
		}

		{
			auto y = workspaceY.begin();
			while (y != workspaceY.end())
			{
				if (!shouldReadYNextIteration)
					checkingConsumer(*bufferX, *y);


				if (canCleanupY(*y, bufferX->start))
				{
					#ifdef LM_VERBOSE
					std::cout << "    Erase Y " << *y << std::endl;
					#endif
					workspaceY.erase(y);
				}
				else
				{
					if (canCleanupY(*y, bufferX->start + tauX))
						potentialPruneY++;

					++y;
				}
			}
		}



		if (shouldReadYNextIteration)
		{
			workspaceY.insert(*bufferY);
			++bufferY;
		}
		else
		{
			workspaceX.insert(*bufferX);
			++bufferX;
		}


		/* DECIDING */

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
			#ifdef LM_VERBOSE
			std::cout << "POTENTIAL PRUNE X " << potentialPruneX << " and Y " << potentialPruneY << std::endl;
			#endif

//			if (potentialPruneX == potentialPruneY)
//				shouldReadYNextIteration = !shouldReadYNextIteration;
//			else
				shouldReadYNextIteration = potentialPruneX > potentialPruneY;
		}

		#ifdef COUNTERS
		lmCounterActiveCountX += workspaceX.size();
		lmCounterActiveCountY += workspaceY.size();
		lmCounterActiveCountTimes++;
		lmCounterActiveMaxX = std::max(lmCounterActiveMaxX, workspaceX.size());
		lmCounterActiveMaxY = std::max(lmCounterActiveMaxY, workspaceY.size());
		lmCounterActiveMax = std::max(lmCounterActiveMax, workspaceX.size() + workspaceY.size());
		#endif
	}
}

