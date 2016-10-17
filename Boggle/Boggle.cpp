#include <stdio.h>
#include "BoggleInterface.h"
#include "Core/Utils.h"
#include "Tests/TestDictonary.h"
#include "Tests/TestThreadPool.h"
#include "Tests/TestBoggleSolver.h"
#include <Windows.h>

int main()
{

	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, 0, 0, r.right-r.left, 800, TRUE);


#if _DEBUG
	LoadDictionary("english.txt");
//	LoadDictionary("unity.txt");
//	LoadDictionary("englishsmall.txt");
#else
	LoadDictionary("english.txt");
#endif
	// ~77 MB for cCItDictionaryFull
	//	~68 MB for cCItDictionaryLight with hashes for WordId
	//	~50 MB for a simple map with StringHash
	// the StringHash has collisions on the english.txt
	// Spooky hash 64 doesn't have collision
	DoBoggleSolverTest ();
	FreeDictionary ();

	BREAKPOINT_RELEASE;
	return 0x0;
}