#pragma once

#include <containers/GaplessList.h>
#include "model/Relation.h"




template <typename Consumer>
void leungMuntzReverseDuringStrictJoin(const Relation& X, const Relation& Y, const Consumer& consumer) noexcept
{
	auto consumeIfCovers = [&consumer] (const Tuple& x, const Tuple& y)
	{
		if (x.start < y.start && y.end < x.end)
			consumer(x, y);
	};

	double tauX = (X[X.size() - 1].start - X[0].start) / (X.size() - 1.0);
	double tauY = (Y[Y.size() - 1].start - Y[0].start) / (Y.size() - 1.0);

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
				consumeIfCovers(*x, *bufferY);

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
				consumeIfCovers(*bufferX, *y);

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

