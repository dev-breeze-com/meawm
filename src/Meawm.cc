/**
 * Meawm - Simple and configurable window manager
 *
 * @file   Meawm.cc
 * @author David Reveman <david@waimea.org>
 * @date   02-May-2001 00:48:03
 *
 * @brief Implementation of Meawm and WindowObject classes
 *
 * Meawm Window Manager.
 *
 * Copyright (C) David Reveman. All rights reserved.
 *
 * @author Pierre Innocent <dev@breezeos.com>
 * Copyright (C) Tsert.Inc, All rights reserved.
 *
 */

#ifdef    HAVE_CONFIG_H
#  include "../config.h"
#endif // HAVE_CONFIG_H

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
    
#ifdef    SHAPE
#  include <X11/extensions/shape.h>
#endif // SHAPE

#ifdef    XINERAMA
#  include <X11/extensions/Xinerama.h>
#endif // XINERAMA

#ifdef    RANDR
#  include <X11/extensions/Xrandr.h>
#endif // RANDR
    
#ifdef    HAVE_STDIO_H
#  include <stdio.h>
#endif // HAVE_STDIO_H

#ifdef    STDC_HEADERS
#  include <stdlib.h>
#endif // STDC_HEADERS

#ifdef    HAVE_UNISTD_H
#  include <sys/types.h>
#  include <sys/wait.h>
#  include <unistd.h>
#endif // HAVE_UNISTD_H
    
#ifdef    HAVE_SIGNAL_H
#  include <signal.h>
#endif // HAVE_SIGNAL_H
}

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include "Meawm.hh"

#if 0
extern "C" {
	int compton_init(Display*, int, char**);
	void compton_step(XEvent*);
	void compton_release();
};
#endif

Meawm *meawm;
char **argv;
bool hush;
int errors;

/**
 * @fn    Meawm(char **av)
 * @brief Constructor for Meawm class
 *
 * Here we open a connection to the display and set signal and xerror
 * handler functions. A list for all WaWindow is created and one list for all
 * WaMenus. An hash_map is also created for effective window searching. Then
 * we load the configuration file, menu file and actions file. We create one
 * WaScreen for the displays default screen and an eventhandler for handling
 * all events.
 *
 * @param av Vector of command line arguments
 * @param _options Parsed command line options
 */
Meawm::Meawm(int argc, char **av, struct waoptions *_options) {
    struct sigaction action;
    int dummy;

    argv = av;
    options = _options;

    XSetErrorHandler((XErrorHandler) xerrorhandler);

    if (! (display = XOpenDisplay(options->display))) {
        cerr << "error: can't open display: " << options->display << endl;
        exit(1);
    }

    meawm = this;
    hush = wmerr = false;
    errors = 0;
    eh = NULL;
    timer = NULL;

	pathenv = NULL;
	compositor = NULL;
	applauncher = NULL;

    action.sa_handler = signalhandler;
    action.sa_mask = sigset_t();
    action.sa_flags = SA_NOCLDSTOP | SA_NODEFER; 

    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGCHLD, &action, NULL);
    sigaction(SIGHUP, &action, NULL);
    
    session_cursor = XCreateFontCursor(display, XC_left_ptr);
    move_cursor = XCreateFontCursor(display, XC_fleur);
    resizeleft_cursor = XCreateFontCursor(display, XC_ll_angle);
    resizeright_cursor = XCreateFontCursor(display, XC_lr_angle);

#ifdef SHAPE
    shape = XShapeQueryExtension(display, &shape_event, &dummy);
#endif // SHAPE

#ifdef XINERAMA
    xinerama = XineramaQueryExtension(display, &dummy, &dummy);
    if (xinerama)
        xinerama = XineramaIsActive(display);
    else
        xinerama = false;

    if (xinerama) {
        xinerama_info = XineramaQueryScreens(display, &xinerama_info_num);
    }
#endif // XINERAMA

#ifdef RANDR
    randr = XRRQueryExtension(display, &randr_event, &dummy);
#endif // RANDR
    
    rh = new ResourceHandler(this, options);
    net = new NetHandler(this);
    rh->LoadConfig(this, options);

    int i, screens = 0;
    WaScreen *ws;

    for (i = 0; i < ScreenCount(display); ++i) {
        if (screenmask & (1L << i)) {
            ws = new WaScreen(display, i, this);
            if (! wmerr) {
                wascreen_list.push_back(ws);
                screens++;
            }
            else wmerr = false;
        }
    }
    if (! screens) {
        cerr << "meawm: error: no managable screens found on display " <<
            DisplayString(display) << endl;
        exit(1);
    }
    
    eh = new EventHandler(this);
    timer = new Timer(this);

	compositing_pid = (-1);
	applauncher_pid = (-1);

    if (options->applauncher) {
		applaunching( true );
	}

    if (options->compositor) {
		//compton_init( display, argc, argv );
		compositing( true );
	}
}

/**
 * @fn    ~Meawm(void)
 * @brief Destructor for Meawm class
 *
 * Deletes all WaScreens. Closes the connection to the display.
 */
Meawm::~Meawm(void)
{
    XSetErrorHandler(NULL);
    LISTDEL(wascreen_list);

    delete net;
    delete rh;

    MAPCLEAR(window_table);
    if (eh) delete eh;
    if (timer) delete timer;

	//compton_release();
    delete [] pathenv;
    delete [] compositor;
    delete [] applauncher;

    XSync(display, false);
    XCloseDisplay(display);
}

/**
 * @fn    compositing(bool)
 * @brief Start or stop cmpositing
 *
 * Start or stop the compositing manager.
 *
 * @param whether to start or stop the compositing manager.
 *
 */
void Meawm::compositing(bool flag)
{
	const char *cmd = compositor;

	if ( !cmd )
		cmd = "/usr/share/meawm/scripts/compositing.sh";

    //cerr << "meawm: compositing " << (flag?"true":"false") << " PID=" << compositing_pid << endl;

	if (compositing_pid > 0) {
		::kill( compositing_pid, SIGINT );
		::kill( compositing_pid, SIGKILL );
	}

	if ( flag ) {
		compositing_pid = waexec( cmd, 0L );
		compositing_pid += 1;
	}
}

/**
 * @fn    applaunching(bool)
 * @brief Start or stop the application launcher
 *
 * Start or stop the application launcher.
 *
 * @param whether to start or stop the application launcher.
 *
 */
void Meawm::applaunching(bool flag)
{
	if ( applauncher ) {
		if (applauncher_pid > 0) {
			::kill( applauncher_pid, SIGINT );
			::kill( applauncher_pid, SIGKILL );
		}

		if ( flag ) {
			applauncher_pid = waexec( applauncher, 0L );
			applauncher_pid += 1;
		}
	}
}

/**
 * @fn    FindWin(Window id, int mask)
 * @brief Find WindowObject
 *
 * Returns WindowObject matching id and mask. NULL if no match was found.
 *
 * @param id Window id to use for matching
 * @param mask Window type mask to use for matching
 *
 * @return Matching WindowObject
 */
WindowObject *Meawm::FindWin(Window id, int mask) {
    map<Window, WindowObject *>::iterator it;
    if ((it = window_table.find(id)) != window_table.end()) {
        if (((*it).second)->type & mask)
            return (*it).second;
    }
    return NULL;
}


/**
 * @fn    validatedrawable(Drawable d, unsigned int *w, unsigned int *h)
 * @brief Validates if a drawable exists
 *
 * Tries to get geometry for the drawable. The drawable is valid
 * if no XError is generated.
 *
 * @param d Resource ID used for drawable validation
 * @param w Return the drawable's width
 * @param h Return the drawable's height
 *
 * @return True if drawable is valid, otherwise false
 */
bool validatedrawable(Drawable d, unsigned int *w, unsigned int *h) {
    int ret, _d;
    unsigned int _ud;
    Window _wd;
    
    XSync(meawm->display, false);
    errors = 0;
    hush = 1;
    if (w == NULL)
        XGetGeometry(meawm->display, d, &_wd, &_d, &_d, &_ud, &_ud, &_ud,
                     &_ud);
    else
        XGetGeometry(meawm->display, d, &_wd, &_d, &_d, w, h, &_ud, &_ud);
    XSync(meawm->display, false);
    hush = 0;
    ret = ( errors == 0 );
    errors = 0;
    return ret;
}

/**
 * @fn    validateclient_mapped(Window id)
 * @brief Validates if a window exist and is mapped
 *
 * First we check that the window exists, if it exists we check the event
 * queue so that there's no UnmapNotify event from the window in it. If there's
 * no UnmapNotify event in the event queue then the window exists and is
 * mapped.
 *
 * @param id Resource ID used for window validation
 *
 * @return True if window is valid, otherwise false
 */
const bool validateclient_mapped(Window id) {
    XFlush(meawm->display);
    
    XEvent e;
    if (validatedrawable((Drawable) id)) {
        if (XCheckTypedWindowEvent(meawm->display, id, UnmapNotify, &e)) {
            XPutBackEvent(meawm->display, &e);
            return false;
        }
        return true;
    }
    return false;
}


/**
 * @fn    waexec(const char *command, char *displaystring)
 * @brief Executes a command line
 *
 * Executes a command line in the 'sh' shell.
 *
 * @param format Command line to execute
 * @param displaystring Displaystring to export prior to execution
 */
int waexec(const char *command, char *displaystring) {
	int pid = fork();

    if ( !pid ) {
        setsid();
        if (displaystring) { putenv(displaystring); }
        execl("/bin/sh", "/bin/sh", "-c", command, NULL);
        exit(0);
    }
	return pid;
}

/**
 * @fn    xerrorhandler(Display *d, XErrorEvent *e)
 * @brief X error handler function
 *
 * Prints error message then a X error occurs.
 *
 * @param d X display 
 * @param e X error event
 *
 * @return always 0
 */
int xerrorhandler(Display *d, XErrorEvent *e) {
    char buff[128];
    map<Window, WindowObject *>::iterator it;

    errors++;

    if (! hush) {
        XGetErrorDatabaseText(d, "XlibMessage", "XError", "", buff, 128);
        cerr << buff;
        XGetErrorText(d, e->error_code, buff, 128);
        cerr << ":  " << buff << endl;
        XGetErrorDatabaseText(d, "XlibMessage", "MajorCode", "%d", buff, 128);
        cerr << "  ";
        fprintf(stderr, buff, e->request_code);
        sprintf(buff, "%d", e->request_code);
        XGetErrorDatabaseText(d, "XRequest", buff, "%d", buff, 128);
        cerr << " (" << buff << ")" << endl;
        XGetErrorDatabaseText(d, "XlibMessage", "MinorCode", "%d", buff, 128);
        cerr << "  ";
        fprintf(stderr, buff, e->minor_code);
        cerr << endl;
        XGetErrorDatabaseText(d, "XlibMessage", "ResourceID", "%d", buff, 128);
        cerr << "  ";
        fprintf(stderr, buff, e->resourceid);
        if (((it = meawm->window_table.find(e->resourceid)) !=
             meawm->window_table.end()) &&
            (((*it).second)->type == WindowType))
            cerr << " (" << ((WaWindow *) (*it).second)->name << ")";
        cerr << endl;
    }
    return 0;
}

/**
 * @fn    wmrunningerror(Display *d, XErrorEvent *)
 * @brief X error handler function
 *
 * X error handler function used when setting input mask of root window.
 * If X error occurs another window manager is running on that screen.
 *
 * @param d X display
 *
 * @return 0
 */
int wmrunningerror(Display *d, XErrorEvent *) {
    meawm->wmerr = true;
    return 0;
}

/**
 * @fn    signalhandler(int sig)
 * @brief Signal handler function
 *
 * When one of the signals we handle arrives this function is called. Depending
 * on what type of signal we received we do something, ex. restart, exit.
 *
 * @param sig The signal we received
 */
void signalhandler(int sig) {
    int status;
    
    switch(sig) {
        case SIGINT:
        case SIGTERM:
            quit(EXIT_SUCCESS);
            break;
        case SIGHUP:
            restart(NULL);
            break;
        case SIGCHLD:
            waitpid(-1, &status, WNOHANG | WUNTRACED);
            break;
        default:
            quit(EXIT_FAILURE);
    }
}

/**
 * @fn    restart(char *command)   
 * @brief Restarts program
 *
 * Deletes the meawm object and restarts window manager.
 *
 * @param command Program name to execute when restarting
 */
void restart(char *command) {
	std::cout << "void restart(char *command)\n";
	std::cout.flush();
    char *tmp_argv[128];
    char *__m_wastrdup_tmp;
    
    if (command) {
        commandline_to_argv(__m_wastrdup(command), tmp_argv);
        delete meawm;
        execvp(*tmp_argv, tmp_argv);
        perror(*tmp_argv);
        exit(EXIT_FAILURE);
    } else
        delete meawm;
    execvp(argv[0], argv);
    perror(argv[0]);
    exit(EXIT_FAILURE);
}

void showing(char *command) {
	std::cout << "void showing(char *command)\n";
	std::cout.flush();
}

/**
 * @fn    quit(void)   
 * @brief Ends program execution
 *
 * Deletes the meawm object and then ends program execution.
 *
 * @param status Return status
 */
void quit(int status) {
	std::cout << "void quit(int status)\n";
	std::cout.flush();
    delete meawm;
    exit(status);
}

/**
 * @fn    commandline_to_argv(char *s)   
 * @brief Parse command line to arguments
 *
 * Parses a command line string into an vector of arguments. Characters
 * between '"' are parsed as one argument. Dangerous because it destroys
 * command line string given as parameter.
 *
 * @param s Command line string
 * @param tmp_argv Vector that should point to the different parameters
 *
 * @return argument vector
 */
char **commandline_to_argv(char *s, char **tmp_argv) {
    int i;
    
    for (i = 0;;) {
        for (; *s == ' ' || *s == '\t'; s++);
        if (*s == '"') {
            tmp_argv[i++] = ++s;
            for (; *s != '"' && *s != '\0'; s++);
            if (*s == '\0') break;
            *s = '\0';
            s++;
            if (*s == '\0') break;
        }
        else {
            tmp_argv[i++] = s;
            for (; *s != ' ' && *s != '\t' && *s != '\0'; s++);
            if (*s == '\0') break;
            *s = '\0';
            s++;
        }
    }
    tmp_argv[i] = NULL;
    return tmp_argv;
}

/**
 * @fn    expand(char *org, WaWindow *w)
 * @brief Window info expansion
 *
 * Expands a special window info characters in a string. Special info
 * characters are:
 * '\p' replaced with _NET_WM_PID hint for window
 * '\n' replaced with name part of windows WM_CLASS hint
 * '\c' replaced with class part of windows WM_CLASS hint
 * '\h' replaced with WM_CLIENT_MACHINE hint
 *
 * @param org Original string used as source for expansion
 * @param w WaWindow to get expansion info from
 *
 * @return Expanded version of original string, or NULL if no expansion have
 *         been made.
 */
char *expand(char *org, WaWindow *w) {
    int i;
    const char *insert;
    char *expanded, *tmp;
    bool cont, found = false;

    if (! org) return NULL;

    expanded = org;
    for (i = 0; expanded[i] != '\0';) {
        cont = false;
        for (; expanded[i] != '\0' && expanded[i] != '\\'; i++);
        if (expanded[i] == '\0') break;
        switch (expanded[i + 1]) {
            case 'p':
                if (w->pid) insert = w->pid;
                else insert = "";
                break;
            case 'h':
                if (w->host) insert = w->host;
                else insert = "";
                break;
            case 'n':
                if (w->classhint->res_name) insert = w->classhint->res_name;
                else insert = "";
                break;
            case 'c':
                if (w->classhint->res_class) insert = w->classhint->res_class;
                else insert = "";
                break;
            default:
                cont = true;
        }
        if (cont) continue;
        int ilen = strlen(insert);
        tmp = new char[strlen(expanded) + ilen + 1];
        expanded[i] = '\0';
        sprintf(tmp, "%s%s%s", expanded, insert, &expanded[i + 2]);
        if (found) delete [] expanded;
        else expanded[i] = '\\';
        expanded = tmp;
        found = true;
        i += ilen;
    }
    if (found) return expanded;
    else return NULL;
}
