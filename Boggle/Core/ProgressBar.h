#pragma once
#ifndef PROGRESS_BAR_H__
#define PROGRESS_BAR_H__

#include "Types.h"
#include <time.h>

class cProgressBar
{
public:
	cProgressBar();
	~cProgressBar();

	void				Start ();
	void				Update(float const Ratio, float const UpdateSeconds = 2.0f);

protected:
	time_t				m_Start;
	time_t				m_LastUpdate;
	bool				m_DisplayAscii;
	u32					m_Line;

	static char *		s_Ascii[91];

};


#endif // !PROGRESS_BAR_H__
