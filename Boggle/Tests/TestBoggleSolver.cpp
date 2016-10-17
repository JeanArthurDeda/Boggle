#include "TestBoggleSolver.h"
#include "../BoggleInterface.h"

// ****************************************************************************

void DoBoggleSolverTest()
{
	Results R;

	printf ("3x3=====================================================================\n");
	char Tablesmall[] =
	{
		"dzxeaiqut"
	};

	printf ("Test Table Unty\n");
	R = FindWords (Tablesmall, 3, 3);
	for (u32 i = 0; i < R.Count; ++i)
		printf ("%02d `%s`\n", i, R.Words[i]);
	printf ("Score %d\n", R.Score);
	FreeWords (R);


	printf ("4x4=====================================================================\n");
	char TableSmall[] =
	{
		"beebzdgapopidoom"
	};

	printf ("Test TableSmall\n");
	R = FindWords (TableSmall, 4, 4);
	for (u32 i = 0; i < R.Count; ++i)
		printf ("%02d `%s`\n", i, R.Words[i]);
	printf ("Score %d\n", R.Score);
	FreeWords (R);


	// =========================
	// Generate a table of 20x20
	//==========================
	printf ("20x20===================================================================\n");
	char Table20x20[20*20+1]="ntnmfwpkamdargmvukwfbqfghxgpadvppmhmlrgridxmwjmldqeeefnyebgooldccemgxwwnidkrwavexcteolemxkisuyilyptfrlyltcaxwylmpqtwaujsycvgiwfjgkrkorbcxtstyvhvbnpmwfwdnijsqtliwbeutyvwkcxglaltnfwpesfqerqgbcobieyvoypmkcaklawifhswaqpcsgqwwmqxhupgaecidwhmbtguqjontepscintifbwjcuafcwuqlhojkapobswporkgvledeixldisxidbjpacdbejcekcehxmbpbrhxhpnbdsdnerctwchlktgkshmsbkmfopbnlkbfmckvbirpjtfsxbjlcvdubywcoglellmoyumbnpyqjesmbr";

	R = FindWords (Table20x20, 20, 20);
	for (u32 i = 0; i < R.Count; ++i)
		printf ("%02d `%s`\n", i, R.Words[i]);
	printf ("Score %d\n", R.Score);
	FreeWords (R);


	/// =======================
	// Generate a 3000x3000 table
	/// =======================
	printf ("3000x3000===============================================================\n");
	srand (0);
	char *Table = reinterpret_cast<char*>(malloc(3000*3000));
	for (u32 i = 0; i < 3000*3000; ++i)
		Table[i] = (rand () % ('z'-'a')) + 'a';
	R = FindWords (Table, 3000, 3000);
	free (Table);
	printf ("Num Words %d\n", R.Count);
	printf ("Score %d\n", R.Score);
	FreeWords (R);


	u32 const MaxWidth = 3000;
	u32 const MaxHeight = 3000;
	for (u32 Test = 0; Test < 10; ++Test)
	{
		u32 const Width = rand () % MaxWidth;
		u32 const Height = rand () % MaxWidth;

		printf ("%04dx%04d===============================================================\n", Width, Height);
		char *Table = reinterpret_cast<char*>(malloc(Width * Height));
		for (u32 i = 0; i < Width * Height; ++i)
			Table[i] = (rand () % ('z'-'a')) + 'a';
		R = FindWords (Table, Width, Height);
		free (Table);
		printf ("Num Words %d\n", R.Count);
		printf ("Score %d\n", R.Score);
		FreeWords (R);
	}

}

// ****************************************************************************