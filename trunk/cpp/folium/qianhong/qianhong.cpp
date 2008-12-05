#include <cstdio>
#include <cstring>
#include <fstream>
#include <set>
using namespace std;
#include <boost/thread.hpp>

#include "../engine.h"
using namespace folium;

#define VERSION  "0.1"
Engine *pEngine;
int level = 6;
bool bAIThreadRunning = false;
bool bBGThreadRunning = false;
bool bEnableBGThinking = true;
bool bAbort = false;
bool bDebug = false;
set<uint> ban;

int bg_ply;
int game_ply;
uint guessed_move;

char message[80];

#if defined(_WIN32) || defined(_WIN64)
#define STRIEQUALS(a, b) (0 == stricmp((a),(b)))
#else
#define STRIEQUALS(a, b) (0 == strcasecmp((a),(b)))
#endif

// plugin output macros
#define DEBUG(f)        { if (bDebug) fprintf (stderr,"(%2d:%2d) "f,game_ply,bg_ply); }
#define DEBUG1(f,a)     { if (bDebug) fprintf (stderr,"(%2d:%2d) "f,game_ply,bg_ply,a); }
#define DEBUG2(f,a,b)   { if (bDebug) fprintf (stderr,"(%2d:%2d) "f,game_ply,bg_ply,a,b); }
#define DEBUG3(f,a,b,c) { if (bDebug) fprintf (stderr,"(%2d:%2d) "f,game_ply,bg_ply,a,b,c); }

#define REPLY(f)        { printf (f);       DEBUG(f); }
#define REPLY1(f,a)     { printf (f,a);     DEBUG1(f,a); }
#define REPLY2(f,a,b)   { printf (f,a,b);   DEBUG2(f,a,b); }
#define REPLY3(f,a,b,c) { printf (f,a,b,c); DEBUG3(f,a,b,c); }

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
    XQ xq;
    if (!xq.set_fen(fen))
        return false;
    pEngine->reset(xq);
    game_ply = pEngine->_ply();
    bg_ply = -1;
    return true;
}

void AIThread ()
{
    bAIThreadRunning = true;
    uint move;
    move = pEngine->search(level, ban);
    ban.clear();//thread safe???
    pEngine->make_move(move);
    char str[20];
    REPLY1 ("%s\n", movestr (str, move))
    fflush (stdout);
    bAIThreadRunning = false;
}

void Plugin ()
{
    boost::thread *pThread = NULL;

    char line[80];
    char cmd[80];
    char arg[80];

    XQ xq;
    xq.set_fen("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r");
    pEngine = new Engine(xq, 23);

    while (1)
    {
        while (fgets (line, 79, stdin) == NULL)
        {
        }
        cmd[0] = 0;
        arg[0] = 0;
        if (sscanf (line, "%s %s", cmd, arg) < 1)
            continue;

        if (bAIThreadRunning)
        {
            if(STRIEQUALS(cmd, "timeout"))
            {
                bAbort = true;
            }
        }

        DEBUG2 ("%s %s\n", cmd, arg);

        if(STRIEQUALS(cmd, "quit"))
            break;
        if(STRIEQUALS(cmd, "exit"))
            break;

        if(STRIEQUALS(cmd, "timeout"))
        {
        }
        else if(STRIEQUALS(cmd, "abort"))
        {
        }
        else if(STRIEQUALS(cmd, "scr"))
        {
            strcpy (message, "Peeking...");
            PrintScr (false);
        }
        else if(STRIEQUALS(cmd, "bgthink"))
        {
            if(STRIEQUALS(cmd, "off"))
            {
                bEnableBGThinking = false;
                REPLY ("OK - Disabled BG thinking\n");
            }
            else if(STRIEQUALS(cmd, "on"))
            {
                bEnableBGThinking = true;
                REPLY ("OK - Enabled BG thinking\n");
            }
            else
            {
                REPLY1 ("ERROR - Bad param: %s\n", arg);
            }
        }
        else if(STRIEQUALS(cmd, "level"))
        {
            if (arg[0] == 0)
            {
                REPLY1 ("%d\n", level);
            }
            else
            {
                int newlevel = atoi (arg);
                if (newlevel >= 6 && newlevel <= 32)
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
        else if(STRIEQUALS(cmd, "fen"))
        {
            if (LoadFEN (line+4))
            {
                REPLY ("OK\n");
            }
            else
            {
                REPLY1 ("ERROR - %s\n", arg);
            }
        }
        else if(STRIEQUALS(cmd, "ban"))
        {
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
        else if(STRIEQUALS(cmd, "ai"))
        {
            if (pThread != NULL)
                delete pThread;
            pThread = new boost::thread(AIThread);
        }
        else if(STRIEQUALS(cmd, "hints"))
        {
        }
        else if(STRIEQUALS(cmd, "load"))
        {
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
        else if(STRIEQUALS(cmd, "play"))
        {
            uint move;
            ICCStoPos (arg, move);
            if (bg_ply == game_ply + 1 &&
                 guessed_move == move)
            {
                game_ply = bg_ply;
                REPLY ("OK\n");
            }
            else
            {
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
        else if(STRIEQUALS(cmd, "undo"))
        {
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
        else
        {
            REPLY1 ("ERROR - Bad command: %s\n", cmd);
        }
        fflush (stdout);
    }
    delete pEngine;
    REPLY ("BYE\n");
}


void PrintInfo ()
{
    printf ("folium AI Plugin for Qianhong\n");
    printf ("\n");
    printf ("folium engine is Copyright 2007(C) Wangmao Lin\n");
    printf ("Mail lwm3751@gmail.com for details\n");
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
    if (argc > 1 && STRIEQUALS(argv[1], "-plugin"))
    {
        if (argc > 2 && STRIEQUALS(argv[1], "debug"))
            bDebug = true;
        Plugin ();
    }
    else if (argc > 1 && STRIEQUALS(argv[1], "-info"))
    {
        // plugin protocol version
        printf ("QHPLUGIN V1.3\n");
        // AI engine name
        printf ("folium\n");
        // levels
        printf ("LEVELS 10\n");
        printf ("6  - Default\n");
        printf ("7\n");
        printf ("8\n");
        printf ("9\n");
        printf ("10\n");
        printf ("11\n");
        printf ("12\n");
        printf ("13\n");
        printf ("14\n");
        printf ("15\n");
        printf ("UNDO 1\n");
        printf ("HINTS 0\n");
        printf ("RULES 1\n");
        printf ("BGTHINK 0\n");
        printf ("TIMEOUT 0\n");
        PrintInfo ();
        printf ("ENDINFO\n");
    }
    else
    {
        PrintInfo ();
        printf ("Mail lwm3751@gmail.com for details\n");
    }

    return 0;
}

