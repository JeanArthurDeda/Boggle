#pragma once

#ifndef HISTOGRAM_H__
#define HISTOGRAM_H__

#include "Types.h"
#include <map>
#include <vector>

class cHistogram
{
public:
	cHistogram():
		m_RangeMin (0xffffff),
		m_RangeMax (0),
		m_CountMax (1)
	{}
	~cHistogram() {}

	typedef std::map<u32, u32>					tMap;
	typedef tMap::iterator						tIterator;
	typedef tMap::const_iterator				tConstIterator;


	inline void									Add(u32 const Value)
	{
		tIterator it = m_Histogram.find(Value);

		if (Value < m_RangeMin)
			m_RangeMin = Value;
		if (Value > m_RangeMax)
			m_RangeMax = Value;

		if (it == m_Histogram.end())
			m_Histogram[Value] = 1;
		else
		{
			it->second++;

			if (it->second > m_CountMax)
				m_CountMax = it->second;
		}
	}

	void								ComputeCalculus();
	void								Empty ();

	inline u32							GetRangeMin() const { return m_RangeMin; }
	inline u32							GetRangeMax() const { return m_RangeMax; }
	inline u32							GetCountMax() const { return m_CountMax; }
	inline u32							Get(u32 const Value) const
	{
		tConstIterator it = m_Histogram.find(Value);

		if (it == m_Histogram.end())
			return 0;
		return it->second;
	}
	inline u32							GetNumLocalMins() const { return static_cast<u32>(m_LocalMin.size()); }
	inline u32							GetLocalMin(u32 const Index) const { return m_LocalMin[Index]; }
	inline u32							GetLocalMax(u32 const Index) const { return m_LocalMax[Index]; }
	inline u32							GetNumLocalMaxs() const { return static_cast<u32>(m_LocalMax.size()); }

protected:

	std::map<u32, u32>					m_Histogram;
	std::vector<u32>					m_LocalMin;
	std::vector<u32>					m_LocalMax;
	u32									m_RangeMin;
	u32									m_RangeMax;
	u32									m_CountMax;
};

#endif // !HISTOGRAM_H__
