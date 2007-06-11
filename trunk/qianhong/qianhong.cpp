#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <fstream>
#include <set>
using namespace std;
#include "engine.h"

#define VERSION  "0.1"

// globals (shaRed between main thread and AI thread)
Engine *pEngine;
int level = 6;

HANDLE hBGThread = NULL;

bool bAIThreadRunning = false;
bool bBGThreadRunning = false;
bool bEnableBGThinking = true;
bool bAbort = false;
bool bDebug = false;

//int numbannedMoves = 0;
//uint32 bannedMoves[128];
set<uint> ban;

int bg_ply;
int game_ply;
uint guessed_move;

char message[80];


// plugin output macros
#define DEBUG(f)        { if (bDebug) fprintf (stderr,"(%2d:%2d) "f,game_ply,bg_ply); }
#define DEBUG1(f,a)     { if (bDebug) fprintf (stderr,"(%2d:%2d) "f,game_ply,bg_ply,a); }
#define DEBUG2(f,a,b)   { if (bDebug) fprintf (stderr,"(%2d:%2d) "f,game_ply,bg_ply,a,b); }
#define DEBUG3(f,a,b,c) { if (bDebug) fprintf (stderr,"(%2d:%2d) "f,game_ply,bg_ply,a,b,c); }

#define REPLY(f)        { printf (f);       DEBUG(f); }
#define REPLY1(f,a)     { printf (f,a);     DEBUG1(f,a); }
#define REPLY2(f,a,b)   { printf (f,a,b);   DEBUG2(f,a,b); }
#define REPLY3(f,a,b,c) { printf (f,a,b,c); DEBUG3(f,a,b,c); }


//------------------------------------------------------------------------------
// This screen-display stuff is just to help in debugging
// (Qianhong does not send a "SCR" command -- except at the end of a failed test,
// but you can ignore that if you want...)

const char *ColorStr(int color)
{
	switch (color)
	{
	case Red: return "Red";
	case Black: return "Black";
	}
	return "BadColor";
}


void WriteDiagram()
{
	const char guychar[] = "KAAEERRHHCCPPPPPkaaeerrhhccppppp";
	const char *pszGrid[20] =
	{
	" .-----------------------. ",
	" |  |  |  | \\|/ |  |  |  | ",
	" |--+--+--+--+--+--+--+--| ",
	" |  |  |  | /|\\ |  |  |  | ",
	" |--+--+--+--+--+--+--+--| ",
	" |  |  |  |  |  |  |  |  | ",
	" |--+--+--+--+--+--+--+--| ",
	" |  |  |  |  |  |  |  |  | ",
	" |-----------------------| ",
	" |                       | ",
	" |-----------------------| ",
	" |  |  |  |  |  |  |  |  | ",
	" |--+--+--+--+--+--+--+--| ",
	" |  |  |  |  |  |  |  |  | ",
	" |--+--+--+--+--+--+--+--| ",
	" |  |  |  | \\|/ |  |  |  | ",
	" |--+--+--+--+--+--+--+--| ",
	" |  |  |  | /|\\ |  |  |  | ",
	" .-----------------------. ",
	""
	};

	for (int y = 9; y >= 0; y--)
	{
		char line[32];
		strcpy (line, pszGrid[(9 - y) * 2]);

		for (int x = 0; x < 9; x++)
		{
			int px = x * 3;
			int piece = pEngine->_square(xy_square(x, y));

			if (piece != EmptyIndex)
			{
                uint color = piece_color(piece);
				char c = guychar[piece];
				if (color == Black)
				{
					line[px]   = '[';
					line[px+1] = c | 0x20;
					line[px+2] = ']';
				}
				else
				{
					line[px]   = '(';
					line[px+1] = c;
					line[px+2] = ')';
				}
			}
		}

		fprintf (stderr, "\t%s\n", line);
		fprintf (stderr, "\t%s\n", pszGrid[(9 - y) * 2 + 1]);
	}

	fprintf (stderr, "\t\t%s do next move\n", ColorStr(pEngine->_player()));
}

void PrintScr (bool skipscr)
{
	if (!skipscr)
	{
		fprintf (stderr, "%s\n\n", message);

		WriteDiagram();

		if (pEngine->_in_checked())
		{
			fprintf (stderr, "check\n");
		}
		else
		{
			fprintf (stderr, "\n");
		}
	}

	fprintf (stderr, "Move %d> ", (game_ply>>1) + 1);
}

// end of screen-display stuff
//------------------------------------------------------------------------------


char *movestr(char *str, int pos)
{
    uint src, dst;
    src = move_src(pos);
    dst = move_dst(pos);

	str[0]=square_x(src)+'A';
	str[1]=square_y(src)+'0';
	str[2]='-';
	str[3]=square_x(dst)+'A';
	str[4]=square_y(dst)+'0';
	str[5]=0;
	return str;
}


bool ICCStoPos (char *str, uint& move)
{
    fprintf (stderr, "^_^Move <%s> \n", str);
	int sx, sy, dx, dy;
	sx = (str[0] & 0xDF) - 'A';
	sy = (str[1]) - '0';
	dx = (str[3] & 0xDF) - 'A';
	dy = (str[4]) - '0';
    fprintf (stderr, "@@Move <%d><%d><%d><%d> \n", sx, sy, dx, dy);
fflush(stderr);
	if (sx < 0 || sx > 8 || dx < 0 || dx > 8 ||
		sy < 0 || sy > 9 || dy < 0 || dy > 9)
	{
		return false;
	}
    move = create_move(xy_square(sx, sy), xy_square(dx, dy));
	return true;
}


bool ICCSMove (char *str)
{
	uint move;

	if (!ICCStoPos (str, move))
	{
		return false;
	}

	if (!pEngine->make_move(move))
	{
		return false;
	}

	return true;
}


//------------------------------------------------------------------------------
// FEN notation is used to start with an alternate board setup.
// (See www.nchess.com for details on FEN.)
//------------------------------------------------------------------------------
bool LoadFEN (const char *str)
{
    string fen(str);
    XQ xq(fen);
    if (xq.piece(0) == InvaildSquare)
        return false;
    pEngine->reset(xq);
    game_ply = pEngine->_ply();
    bg_ply = -1;
	return true;
}


//------------------------------------------------------------------------------
// The BG thread is used for background AI thinking between "AI" commands.
//
// This thread is a slave of the AI thread, and the AI thread guarantees that it
// will not touch pEngine (except stop_thinking) while this thread is running.
//------------------------------------------------------------------------------
/*
void BGThread (void *arg)
{
	bBGThreadRunning = true;

	// play guessed move, then start AI thinking on next move
	if (pEngine->make_move(guessed_move)
	{
		bg_ply = pEngine->_ply();
		char str[10];
		DEBUG2 ("[BG] Thinking for ply %d, guessed move = %s\n", bg_ply, movestr (str, guessed_move));

		_pvline[0] = 0;
		pEngine->search(level, 0);
	}
	else
	{
		bg_ply = -1;
		char str[10];
		DEBUG1 ("[BG] Cannot play guessed move! (%s)\n", movestr (str, guessed_move));
	}

	bBGThreadRunning = false;
	_endthread();
}//*/

//*
void Kill_BG_Thread ()
{
	if (bAIThreadRunning)
	{
		DEBUG ("Killing BG thread...\n");

		pEngine->stop();

		if (WaitForSingleObject (hBGThread, 5000) != WAIT_OBJECT_0)
		{
			REPLY ("ERROR - BG Thread not responding\n");
		}
		// NOTE: no need to close the handle (_endthread does it)

		DEBUG ("Done killing BG thread\n");
	}

	bg_ply = -1;
	while (pEngine->_ply() > game_ply)
	{
		pEngine->unmake_move();
	}
}//*/


//------------------------------------------------------------------------------
// The AI thread is used for "AI" and "HINTS" commands.
//
// It is aborted when a interrupting command is received by the main thread.
// The main thread sets pEngine->stop_thinking to cause the AI to break,
// and sets bAbort so this thread knows it was aborted.
//------------------------------------------------------------------------------
void AIThread (void *arg)
{

	bAIThreadRunning = true;
	int hints = (int)arg;
    uint move;
    move = pEngine->search(level, ban);
    ban.clear();//thread safe???
    pEngine->make_move(move);
    char str[20];
    REPLY1 ("%s\n", movestr (str, move))
    fflush (stdout);
    bAIThreadRunning = false;
	_endthread();
}


//------------------------------------------------------------------------------
// The plugin.
// (See jcraner.com/qianhong for the plugin specification.)
//------------------------------------------------------------------------------
void Plugin ()
{
	HANDLE hThread;

	char line[80];
	char cmd[80];
	char arg[80];

	pEngine = new Engine(XQ("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1"), 23);

	while (1)
	{
		while (fgets (line, 79, stdin) == NULL)
		{
		}
		cmd[0] = 0;
		arg[0] = 0;
		if (sscanf (line, "%s %s", cmd, arg) < 1)
			continue;

		// Stop the AI thread if it is running. It will send
		// an ABORTED reply to Qianhong.
		if (bAIThreadRunning)
		{
         // abort for all commands except "TIMEOUT",
         // which causes a result to be returned
            if (stricmp (cmd, "timeout") != 0)
            {
                bAbort = true;
            }
			if (WaitForSingleObject (hThread, 1000) != WAIT_OBJECT_0)
			{
				REPLY ("ERROR - AI Thread not responding\n");
				break;
			}
			// NOTE: no need to close the handle (_endthread does it)
		}

		// Echo command to stderr (for debug mode)
		DEBUG2 ("%s %s\n", cmd, arg);

		if (stricmp (cmd, "quit") == 0)
			break;
		if (stricmp (cmd, "exit") == 0)
			break;

      if (stricmp (cmd, "timeout") == 0)
      {
         // Do nothing; we have already set stop_thinking (without setting bAbort),
         // so any AI or HINTS command in progress should quickly reply with a move
      }
		else if (stricmp (cmd, "abort") == 0)
		{
			//Kill_BG_Thread();

			// This will have caused the thread to abort (if running).
			// The only reply should be from any pending "AI" or "HINTS"
			// command that this may have aborted, so do nothing else here.
		}
		else if (stricmp (cmd, "scr") == 0)
		{
			// This is just a debugging command
			strcpy (message, "Peeking...");
			PrintScr (false);
		}

		// BGTHINK command
		else if (stricmp (cmd, "bgthink") == 0)
		{
			if (stricmp (arg, "off") == 0)
			{
				bEnableBGThinking = false;
				REPLY ("OK - Disabled BG thinking\n");
			}
			else if (stricmp (arg, "on") == 0)
			{
				bEnableBGThinking = true;
				REPLY ("OK - Enabled BG thinking\n");
			}
			else
			{
				REPLY1 ("ERROR - Bad param: %s\n", arg);
			}
		}

		// LEVEL command
		else if (stricmp (cmd, "level") == 0)
		{
			// If no arg, print current level, else set new level
			if (arg[0] == 0)
			{
				REPLY1 ("%d\n", level);
			}
			else
			{
				int newlevel = atoi (arg);
				if (newlevel >= 6 && newlevel <= 9)
				{
					level = newlevel;
					REPLY1 ("OK - Set AI level to %d\n", level);
				}
				else
				{
					REPLY3 ("ERROR - Bad level %s (use %d to %d)\n",
						arg, 6, 9);
				}
			}
		}

		// FEN command
		else if (stricmp (cmd, "fen") == 0)
		{
			//Kill_BG_Thread();
			if (LoadFEN (line))
			{
				REPLY ("OK\n");
			}
			else
			{
				REPLY1 ("ERROR - %s\n", message);
			}
		}

		// ban command
		else if (stricmp (cmd, "ban") == 0)
		{
			//Kill_BG_Thread();

			// Read 'arg' number of moves from stdin and add to banned list
			int moves = atoi (arg);
			int error = 0;
			int i = 0;

			ban.clear();

			while (i < moves)
			{
				arg[0] = 0;
				fgets (line, 79, stdin);
				sscanf (line, "%s", arg);
				DEBUG1("%s\n", arg);

				if (!error)
				{
					uint move;

					if (ICCStoPos (arg, move))
					{
                        ban.insert(move);
					}
					else
					{
						REPLY2 ("ERROR - Bad move (%d): %s\n", i+1, arg);
						error = 1;
					}
				}

				++i;
			}

			if (!error)
			{
				REPLY ("OK\n");
			}
		}

		// AI command
		else if (stricmp (cmd, "ai") == 0)
		{
            //Kill_BG_Thread();
			// Reset local banned move count
			// Start AI thinking in separate thread
			hThread = (HANDLE)_beginthread (AIThread, 0, (void*)0);
			if (hThread == (HANDLE)-1)
			{
				REPLY ("ERROR - Error creating thread\n");
			}
		}

		// HINTS command
		else if (stricmp (cmd, "hints") == 0)
		{
            /*
			Kill_BG_Thread();

			// Copy banned moves list
			pEngine->num_banned_moves = numbannedMoves;
			memcpy (pEngine->banned_moves, bannedMoves, sizeof(pEngine->banned_moves));

			// Reset local banned move count
			numbannedMoves = 0;

			// Start AI thinking in separate thread with arg 1 for hint mode
			hThread = (HANDLE)_beginthread (AIThread, 0, (void*)1);
			if (hThread == (HANDLE)-1)
			{
				REPLY ("ERROR - Error creating thread\n");
			}//*/
		}

		// LOAD command
		else if (stricmp (cmd, "load") == 0)
		{
			//Kill_BG_Thread();

			// Read 'arg' number of moves from stdin and play each one
			// On error, keep reading the moves but don't play any more of them

			int moves = atoi (arg);
			int error = 0;
			int i = 0;

			while (i < moves)
			{
				arg[0] = 0;
				fgets (line, 79, stdin);
				sscanf (line, "%s", arg);

				if (!error && !ICCSMove (arg))
				{
					REPLY2 ("ERROR - Bad move (%d): %s\n", i+1, arg);
					error = 1;
				}

				++i;
			}

			game_ply = pEngine->_ply();

			if (!error)
			{
				REPLY ("OK\n");
			}
		}

		// PLAY command
		else if (stricmp (cmd, "play") == 0)
		{
			uint move;
			ICCStoPos (arg, move);

			// If BG thread has already played this move then just increment game_ply
			if (bg_ply == game_ply + 1 &&
				 guessed_move == move)
			{
				game_ply = bg_ply;
				REPLY ("OK\n");
			}
			else
			{
				//Kill_BG_Thread();

				// Play move
				if (!pEngine->make_move(move))
				{
					game_ply = pEngine->_ply();
					REPLY1 ("ERROR - Bad move: %s\n", arg);
				}
				else
				{
					game_ply = pEngine->_ply();
					REPLY ("OK\n");
				}
			}
		}

		// UNDO command
		else if (stricmp (cmd, "undo") == 0)
		{
			//Kill_BG_Thread();

			// Undo last move
			if (pEngine->_ply())
			{
                pEngine->unmake_move();
				game_ply = pEngine->_ply();
				REPLY ("OK\n");
			}
			else
			{
				game_ply = pEngine->_ply();
				REPLY ("ERROR\n");
			}
		}

		// bad commands
		else
		{
			REPLY1 ("ERROR - Bad command: %s\n", cmd);
		}

		// IMPORTANT! The output must be flushed for Qianhong to
		// be able to read it from the pipe. Failure to do so will
		// result in it hanging...
		fflush (stdout);
	}

	//Kill_BG_Thread();

	delete pEngine;
	// Qianhong doesn't actually read this, but it's part of the protocol
	REPLY ("BYE\n");
}


void PrintInfo ()
{
	printf ("folium AI Plugin for Qianhong\n");
	printf ("\n");
	printf ("folium engine is Copyright 2007(C) Wangmao Lin\n");
}


//------------------------------------------------------------------------------
// The plugin is run in two modes using the first arg as an option:
//
//  argv[1]      Plugin Mode
//  ----------   -------------
//  "-info"      Write info to stdout and exit. The info has to be in a certain
//               format. It tells Qianhong about the capabilities of the plugin.
//
//  "-plugin"    Run in plugin mode. Optional "debug" parameter signals that
//               the plugin has a console and may output anything it wants to
//               stderr to be seen on the console.
//------------------------------------------------------------------------------
int main (int argc, char **argv)
{
	if (argc > 1 && strcmp (argv[1], "-plugin") == 0)
	{
		// Set optional debug mode
		if (argc > 2 && strcmp (argv[2], "debug") == 0)
			bDebug = true;
		// Run the plugin
		Plugin ();
	}
	else if (argc > 1 && strcmp (argv[1], "-info") == 0)
	{
		// Output the plugin info using protocol "QHPLUGIN V1.3"

		// plugin protocol version
		printf ("QHPLUGIN V1.3\n");
		// AI engine name
		printf ("folium\n");
		// levels
		printf ("LEVELS 4\n");
		printf ("6  - Default\n");
		printf ("7\n");
		printf ("8\n");
		printf ("9\n");
		// undo support (0 for no, 1 for yes)
		printf ("UNDO 1\n");
		// hint support (0 for no, 1 for yes)
		printf ("HINTS 0\n");
		// rules support (0 for no, 1 for yes)
		printf ("RULES 1\n");
		// BG think support (0 for no, 1 for yes)
		printf ("BGTHINK 0\n");
		// time limit support (0 for no, 1 for yes)
		printf ("TIMEOUT 0\n");
		// additional info (goes until the ENDINFO line)
		PrintInfo ();
		printf ("ENDINFO\n");
	}
	else
	{
		PrintInfo ();
		printf ("See www.jcraner.com/qianhong/ for details\n");
	}

	return 0;
}

