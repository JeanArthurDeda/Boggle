#pragma once

#include "Histogram.h"

// ****************************************************************************

void cHistogram::ComputeCalculus()
{
	i32 PrevSign = 0;
	i32 Sign = 0;

	for (u32 i = GetRangeMin(); i <= GetRangeMax(); ++i)
	{

		Sign = Get (i + 1) - Get (i);
		if (Sign > 0)
			Sign = 1;
		if (Sign < 0)
			Sign = -1;
		if ((Sign != 0) && (PrevSign != Sign))
		{
			if (Sign > 0)
				m_LocalMin.push_back(i);
			if (Sign < 0)
				m_LocalMax.push_back(i);
		}

		PrevSign = Sign;
	}
}

// ****************************************************************************

void cHistogram::Empty()
{
	m_Histogram.clear ();
	m_LocalMin.clear ();
	m_LocalMax.clear ();
	m_RangeMin = 0;
	m_RangeMax = 0;
	m_CountMax = 0;

}

// ****************************************************************************
