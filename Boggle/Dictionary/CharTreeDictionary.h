#pragma once

#ifndef CHAR_TREE_DICTIONARY_H__
#define CHAR_TREE_DICTIONARY_H__

#include "../Core/Types.h"
#include <map>

class cCharTreeDictionary
{
public:
	cCharTreeDictionary ();
	virtual ~cCharTreeDictionary ();

	// ==============
	// Tree structure
	// ==============
	class cLeaf
	{
	public:
		static u32 const						kEmptyId = 0xffffffff;
		static u32 const						kNumLeafs = 'z'-'a'+1;
		typedef cLeaf*							tLeafs[kNumLeafs];

		cLeaf () : WordId (kEmptyId)			{ memset (ppLeafs, 0, sizeof (ppLeafs));}
		cLeaf (u32 const Id) : WordId (Id)		{ memset (ppLeafs, 0, sizeof (ppLeafs));}
		~cLeaf ();

		u32										WordId;
		tLeafs									ppLeafs;
	};

	// for easy access
	typedef cLeaf::tLeafs						tLeafs;
	static u8									Index (char const c) {return static_cast<u8>(c-'a'); }
	static char									Char (u8 const Index) {return static_cast<char>(Index) + 'a'; }

	tLeafs const &								GetRoot () const { return m_ppRoot; }
	tLeafs &									GetRoot () { return m_ppRoot; }

	// ==================
	// Dictionary Helpers
	// ==================

	// A word iteration callback. Return false to break the iteration.
	typedef bool(*tWordIteratorCB)(char const * const Word, u32 const WordSize, u32 const WordId, void *pUserData);

	u32											Add (char const * const Word, u32 WordSize = 0);
	void										Empty ();
	bool										Iterate(tWordIteratorCB WordCB, void *pUserData = NULL) const;

	inline u32									GetNumWords() const { return m_NumWords; }
	inline u32									GetMaxWordSize() const { return m_MaxWordSize; }


protected:
	cLeaf*										m_ppRoot['z'-'a'+1];
	u32											m_NumWords;
	u32											m_MaxWordSize;

	u32											AddRec (cLeaf::tLeafs &ppLeafs, char const * const String, u32 const StringSize);
	bool										IterateRec (cLeaf::tLeafs &ppLeafs, char * const String, u32 const StringSize, tWordIteratorCB WordCB, void * pUserData) const;
};



#endif // !CHARACTER_TREE_DICTIONARY_H__

