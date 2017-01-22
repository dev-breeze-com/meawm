/**
 * Meawm - Simple and configurable window manager
 *
 * @file   Meawm.hh
 * @author David Reveman <david@meawm.org>
 * @date   02-May-2001 00:49:45
 *
 * @brief Definition of Meawm and WindowObject classes
 * Function declarations and variable definitions for Meawm class.
 * Copyright (C) David Reveman.  All rights reserved.
 *
 * @author Pierre Innocent <dev@breezeos.com>
 * Copyright (C) Tsert.Inc, All rights reserved.
 *
 */

#ifndef __Meawm_hh
#define __Meawm_hh

extern "C" {
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
}

#include <list>
using std::list;

#include <map>
using std::map;
using std::make_pair;

typedef struct _WaAction WaAction;

class Meawm;

#define __m_wastrdup(_str) (((__m_wastrdup_tmp = \
                              new char[strlen(_str) + 1]) && \
                             sprintf(__m_wastrdup_tmp, "%s", _str))? \
                            __m_wastrdup_tmp : __m_wastrdup_tmp)

class WindowObject {
public:
    inline WindowObject(Window win_id, int win_type) {
        id = win_id;
        type = win_type;
        actionlist = NULL;
    }

    Window id;
    int type;
    list<WaAction *> *actionlist;
};

#define EastType  1
#define WestType -1

struct waoptions {
    char *display;
    char *rcfile;
    char *stylefile;
    char *actionfile;
    char *menufile;
	bool compositor;
	bool applauncher;
};

#define WARNING cerr << "meawm: warning: " << __FUNCTION__ << ": "
#define ERROR cerr << "meawm: error: " << __FUNCTION__ << ": "
			
#define LISTDEL(list) \
    while (! list.empty()) { \
        delete list.back(); \
        list.pop_back(); \
    }

#define LISTDELITEMS(list) \
    while (! list.empty()) { \
        delete list.back(); \
    }

#define LISTPTRDEL(list) \
    while (! list->empty()) { \
        delete list->back(); \
        list->pop_back(); \
    }

#define LISTCLEAR(list) \
    while (! list.empty()) { \
        list.pop_back(); \
    }

#define LISTPTRCLEAR(list) \
    while (! list->empty()) { \
        list->pop_back(); \
    } \
    delete list;

#define LISTPTRDELITEMS(list) \
    while (! list->empty()) { \
        delete list->back(); \
    }

#define MAPCLEAR(map) \
    while (! map.empty()) { \
        map.erase(map.begin()); \
    }

#define MAPPTRCLEAR(map) \
    while (! map->empty()) { \
        map->erase(map->begin()); \
    } \
    delete map;

#define FrameType       (1L <<  0)
#define WindowType      (1L <<  1)
#define TitleType       (1L <<  2)
#define LabelType       (1L <<  3)
#define ButtonType      (1L <<  4)
#define HandleType      (1L <<  5)
#define LGripType       (1L <<  6)
#define RGripType       (1L <<  7)
#define RootType        (1L <<  8)
#define WEdgeType       (1L <<  9)
#define EEdgeType       (1L << 10)
#define NEdgeType       (1L << 11)
#define SEdgeType       (1L << 12)
#define MenuTitleType   (1L << 13)
#define MenuItemType    (1L << 14)
#define MenuCBItemType  (1L << 15)
#define MenuSubType     (1L << 16)
#define DockHandlerType (1L << 17)
#define DockAppType     (1L << 18)
#define MenuType        (1L << 19)
#define SystrayType     (1L << 20)

enum {
    MoveType,
    MoveOpaqueType,
    ResizeType,
    ResizeOpaqueType,
    EndMoveResizeType
};

#define MaxCBoxType     1
#define MinCBoxType     2
#define ShadeCBoxType   3
#define StickCBoxType   4
#define TitleCBoxType   5
#define HandleCBoxType  6
#define BorderCBoxType  7
#define AllCBoxType     8
#define AOTCBoxType     9
#define AABCBoxType    10
#define FsCBoxType     11

#define CloseCBoxType  12

#include "Screen.hh"
#include "Timer.hh"
#include "Net.hh"

class Meawm {
public:
    Meawm(int argc, char **, struct waoptions *);
    virtual ~Meawm(void);

    WindowObject *FindWin(Window, int);
	void compositing(bool);
	void applaunching(bool);

    struct waoptions *options;
    Display *display;
    ResourceHandler *rh;
    EventHandler *eh;
    NetHandler *net;
    Timer *timer;
    Cursor session_cursor, move_cursor;
    Cursor resizeleft_cursor, resizeright_cursor;
    unsigned long double_click, screenmask;

	int compositing_pid;
	int applauncher_pid;
    char *applauncher;
    char *compositor;
    char *pathenv;
    bool wmerr;

    map<Window, WindowObject *> window_table;
    list<WaScreen *> wascreen_list;

#ifdef SHAPE
    int shape, shape_event;
#endif // SHAPE

#ifdef XINERAMA
    int xinerama;
    XineramaScreenInfo *xinerama_info;
    int xinerama_info_num;
#endif // XINERAMA

#ifdef RANDR
    int randr, randr_event;
#endif // RANDR
    
};

bool validatedrawable(Drawable, unsigned int * = NULL, unsigned int * = NULL);
const bool validateclient_mapped(Window);
void wawarning(char *, ...);
void waerror(char *, ...);
int waexec(const char *, char *);
int xerrorhandler(Display *, XErrorEvent *);
int wmrunningerror(Display *, XErrorEvent *);
void signalhandler(int);
void showing(char *);
void restart(char *);
void quit(int);
char **commandline_to_argv(char *, char **);
char *expand(char *, WaWindow *);

#endif // __Meawm_hh
