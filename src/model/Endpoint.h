#pragma once

#include <ostream>
#include "model/Tuple.h"



typedef unsigned TID;



class Endpoint
{
public:
	enum class Type : Timestamp {START = 0, END = 1};

private:
	Timestamp timestampAndType;
	TID tid;

public:
	Endpoint() { }

	Endpoint(Timestamp timestampAndType, TID tid)
	:
		timestampAndType(timestampAndType),
		tid(tid)
	{
	}

public:
	Endpoint(Timestamp timestamp, Type type, TID tid) noexcept
	:
		timestampAndType(timestamp << 1 | static_cast<Timestamp>(type)),
		tid(tid)
	{
		assert(getTimestamp() == timestamp);
		assert(getType() == type);
	}


	TID getTID() const noexcept
	{
		return tid;
	}


	Timestamp getTimestamp() const noexcept
	{
		return timestampAndType >> 1;
	}


	Type getType() const noexcept
	{
		return static_cast<Type>(timestampAndType & 1);
	}


	bool operator < (const Endpoint& rhs) const noexcept
	{
		return timestampAndType < rhs.timestampAndType;
	}


	bool operator <= (const Endpoint& rhs) const noexcept
	{
		return timestampAndType <= rhs.timestampAndType;
	}


	bool isStart() const noexcept
	{
		return getType() == Type::START;
	}


	bool isEnd() const noexcept
	{
		return !isStart();
	}


	static constexpr Timestamp calculateShiftArgument(Timestamp delta)
	{
		return delta << 1;
	}

	static constexpr Timestamp calculateShiftArgument(Timestamp delta, Type fromType, Type toType)
	{
		return calculateShiftArgument(delta) + static_cast<Timestamp>(toType) - static_cast<Timestamp>(fromType);
	}


	Endpoint shiftedBy(Timestamp shiftArgument) const noexcept
	{
		return Endpoint(timestampAndType + shiftArgument, tid);
	}


	friend void swap(Endpoint& a, Endpoint& b) noexcept
	{
		using std::swap;
		swap(a.timestampAndType, b.timestampAndType);
		swap(a.tid, b.tid);
	}


	friend std::ostream& operator << (std::ostream &out, const Endpoint& endpoint)
	{
		return out << '@' << endpoint.getTimestamp() << (endpoint.isStart() ? 'S' : 'E') << endpoint.getTID() + 1;
	}
};

