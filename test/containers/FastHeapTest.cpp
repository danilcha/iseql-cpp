#include <gtest/gtest.h>
#include "containers/FastHeap.h"




TEST(Containers, FastHeap)
{
	FastHeap<int> h{2};

	auto p1 = h.alloc();
	auto p2 = h.alloc();
	ASSERT_NE(p1, p2);

	h.free(p1);
	auto p3 = h.alloc();
	ASSERT_EQ(p3, p1);
}


