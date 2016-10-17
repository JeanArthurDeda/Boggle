#include "ProgressBar.h"
#include "Utils.h"
#include <stdio.h>

// ****************************************************************************

cProgressBar::cProgressBar()
{
	Start();
}

// ****************************************************************************

cProgressBar::~cProgressBar()
{
}

// ****************************************************************************

void cProgressBar::Start()
{
	time(&m_Start);
	m_LastUpdate = m_Start;
	m_DisplayAscii = false;
	m_Line = 0;
}

// ****************************************************************************

void cProgressBar::Update(float const Ratio, float const UpdateSeconds)
{
	time_t Now;
	time(&Now);
	double const Seconds = difftime(Now, m_LastUpdate);
	if (Seconds >= UpdateSeconds || (UpdateSeconds == 0.0))
	{
		// compute the eta
		double const Duration = difftime(Now, m_Start);
		double const Eta = Duration / Ratio;

		if (Eta > 10.0)
			m_DisplayAscii = true;

		printf("%5.1f%% of %5.1fs", Ratio * 100, Eta);

		if (m_DisplayAscii)
		{
			printf(" %s", s_Ascii[m_Line]);
			m_Line++;
			if (m_Line >= ARRAY_SIZE(s_Ascii))
				m_Line = 0;
		}
		printf("\n");

		m_LastUpdate = Now;
	}
}

// ****************************************************************************

char * cProgressBar::s_Ascii[91] =
{
	"                        /''''.             /'''.",
	"                       : :'':/           % /''.'.",
	"                   MMMM; ; ' /          / /   : :",
	"                   WWWW:-:##MMMM--'''-- // ,-'  ;",
	"                 /'    :     'WMMM    ::.' .-''",
	"                 ; .MM /.-- ....WMM'     .-'",
	"              ...---WW'' / /    'm'       ':",
	"        ...--''      ..-'  :  MM :         ':",
	"      /''MMMMM.   .''      . MM %           :",
	"-.-''#-. 'MMMMMMM.-.                       /'",
	"----#--   'WMMW''   '.                    .'",
	"....,--  #       -...                   .'",
	"    :  .-' '.     '-.:*-'''--   .'''--'%'",
	"       &.    '.   .-' '-. #-.#-.",
	"        &. .-''-.-'      #-. #-. '.",
	"         '''            /   #.=-.",
	"                      ./  .-'    '.",
	"                  .='' '-=''      :",
	"                 /  '             :",
	"                 :   :             :",
	"                ;   .%             :",
	"                :   -.         :   :",
	"               :'  ./          :   :",
	"               :   -.          ;   :",
	"              :'  ./          /   /",
	"              :   =.          :   :",
	"              :   /           :  .:.",
	"             /  .:          .-++    +.",
	"             :  ::.      .-+          &",
	"            /  :+%=-....-+           . :",
	"            :  :        +-. .-+-.  . : :",
	"           /   +++++-.     +-...++. :) )",
	"           :       .-+++-.       .+.)=+",
	"           :....=+!       +-.   .+ :",
	"              : : :          +++   :",
	"              +.+.+.               :",
	"                -+-:               ;",
	"                   &              /",
	"                    &             :",
	"                     &            :",
	"                     !            +....",
	"                     +;                +-.",
	"                      +.          ++++-.  +.",
	"                       +-  .     .:     +.  +.",
	"                       :   +.    ./       +.  +.",
	"                       &    :    :         +.  +.",
	"                       :   :    ;          +.  +.",
	"                       :   :   /            +.  +.",
	"                       :   :   :             +.  +.",
	"                       :   :   :             +.  +.",
	"                       :   :   :             +.  +.",
	"                       :   :   :              +.  +.      ",
	"                       :   :   :               +.  +.   -+",
	"                       :   :   :                +.   '+-+ ",
	"                       :   :   :                 +.      .",
	"                       :   :   :                  +--...-+",
	"                       :  /    :",
	"                       :  :    :",
	"                       :  :    :",
	"                       ;  :    :",
	"                      /   :    :",
	"                     +    :    +.",
	"                   .+     :      +-.",
	"                 .+        :        ,+-.",
	"               .+          !            +-.",
	"             ;+           + +           -. +.",
	"           .+  +   +     .+   +.        .  +.;",
	"           : .+ .+    .+       +.       +...)",
	"           +-.  :   .-+          +-......).",
	"             +--+--+",
	"",
	"                      __---__",
	"                   _-       _--______",
	"              __--( /     \\ )XXXXXXXXXXXXX_",
	"            --XXX(   O   O  )XXXXXXXXXXXXXXX-",
	"           /XXX(       U     )        XXXXXXX\\",
	"         /XXXXX(              )--_  XXXXXXXXXXX\\",
	"        /XXXXX/ (      O     )   XXXXXX   \\XXXXX\\",
	"        XXXXX/   /            XXXXXX   \\__ \\XXXXX----",
	"        XXXXXX__/          XXXXXX         \\__----  -",
	"---___  XXX__/          XXXXXX      \\__         ---",
	"  --  --__/   ___/\\  XXXXXX            /  ___---=",
	"    -_    ___/    XXXXXX              '--- XXXXXX",
	"      --\\/XXX\\ XXXXXX                      /XXXXX",
	"        \\XXXXXXXXX                        /XXXXX/",
	"         \\XXXXXX                        _/XXXXX/",
	"           \\XXXXX--__/              __-- XXXX/",
	"            --XXXXXXX---------------  XXXXX--",
	"               \\XXXXXXXXXXXXXXXXXXXXXXXX-",
	"                 --XXXXXXXXXXXXXXXXXX- ",
	""
};