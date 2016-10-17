#pragma once

#ifndef BOGGLE_DICTIONARY_H__
#define BOGGLE_DICTIONARY_H__

#include "Core\Types.h"
#include "Core/Histogram.h"
#include "Dictionary/CharTreeDictionary.h"


class cBoggleDictionary
{
public:
	cBoggleDictionary();
	~cBoggleDictionary();


	void							AddWord (char const * const Word, u32 WordSize = 0);
	void							ComputeStatistics ();

	void							Empty ();


	cCharTreeDictionary const &		GetDictionary() const			{ return m_Dictionary; }
	cHistogram const &				GetHistogram() const			{ return m_Histogram; }
	u32								GetNumWords () const			{ return m_Dictionary.GetNumWords (); }
	u32								GetMaxWordSize () const			{ return m_Dictionary.GetMaxWordSize (); }
	u32								GetMaxConsonats () const		{ return m_MaxConsonants; }
	u32								GetMaxVowels () const			{ return m_MaxVowels; }
	u32								GetMaxConscConsonats () const	{ return m_MaxConsConsonants; }
	u32								GetMaxConscVowels () const		{ return m_MaxConsVowels; }
	u32								Get80PercMaxWordSize () const	{ return m_80PercMaxWordSize;}
	u32								Get95PercMaxWordSize () const	{ return m_95PercMaxWordSize;}


protected:
	cCharTreeDictionary				m_Dictionary;
	cHistogram						m_Histogram;

	u32								m_MaxConsonants;
	u32								m_MaxVowels;
	u32								m_MaxConsConsonants;
	u32								m_MaxConsVowels;
	u32								m_80PercMaxWordSize;
	u32								m_95PercMaxWordSize;
};

#endif // !BOGGLE_DICTIONARY_H__

