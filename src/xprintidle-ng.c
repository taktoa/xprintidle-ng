// This program prints the "idle time" of the user to stdout.  The "idle
// time" is the number of milliseconds since input was received on any
// input device.  If unsuccessful, the program prints a message to stderr
// and exits with a non-zero exit code.
//
// Copyright © 2005, 2008 Magnus Henoch           <henoch@dtek.chalmers.se>
// Copyright © 2006, 2007 Danny Kukawka           <danny.kukawka@web.de>
// Copyright © 2008       Eivind Magnus Hvidevold <hvidevold@gmail.com>
// Copyright © 2015       Remy Goldschmidt        <taktoa@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of version 2 of the GNU General Public License
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to
//
//     Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor
//     Boston, MA 02110-1301, USA.
//
// The function workaroundX11 was adapted from kpowersave-0.7.3 by
// Eivind Magnus Hvidevold <hvidevold@gmail.com>.
// kpowersave is licensed under the GNU GPL, version 2 _only_.

#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>
#include <X11/extensions/scrnsaver.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <config.h>
#include "system.h"
#include "errno.h"
#include "error.h"
#include "progname.h"
#include "xalloc.h"

#define AUTHORS             "Magnus Henoch"
#define PROJECT_URL         "https://github.com/taktoa/xprintidle-ng"
#define TRANSLATION_URL     "https://github.com/taktoa/xprintidle-ng/issues"
#define BUG_URL             "https://github.com/taktoa/xprintidle-ng/issues"

#define TIME_FORMAT_DEFAULT "%m"

// Forward declarations.
static unsigned long workaroundX11(Display *dpy, unsigned long _idleTime);
static void check_xss_supported(int event_basep, int error_basep);
static void try_xss_query(XScreenSaverInfo ssi);
static void print_idle_time();
static void signal_callback_handler(int sig, siginfo_t *siginfo, void *context);
static void mbprint(const char* msg);
static void print_help(FILE *out);
static void print_version(FILE *out);
const char* time_format;
Display *dpy;

int main(int argc, char* argv[]) {
    int lose = 0;

    // Option enum
    enum {
        OPT_HELP = CHAR_MAX + 1,
        OPT_VERSION
    };

    // Option format struct
    static const struct option longopts[] = {
        {"format",  required_argument, NULL, 'f'},
        {"help",    no_argument,       NULL, OPT_HELP},
        {"version", no_argument,       NULL, OPT_VERSION},
        {NULL, 0, NULL, 0}
    };

    // Set program name to $0
    set_program_name(argv[0]);

    // Set locale via LC_ALL.
    setlocale(LC_ALL, "");

#if ENABLE_NLS
    // Set the text message domain.
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
#endif

    // Close stdout upon exiting
    atexit(close_stdout);

    char* time_format_raw = _(TIME_FORMAT_DEFAULT);
    
    int optc;
    while((optc = getopt_long(argc, argv, "f:", longopts, NULL)) != -1) {
        switch(optc) {
        case OPT_VERSION:
            print_version(stdout);
            exit(EXIT_SUCCESS);
            break;
        case OPT_HELP:
            print_help(stdout);
            exit(EXIT_SUCCESS);
            break;
        case 'f':
            time_format_raw = optarg;
            break;
        default:
            lose = true;
            break;
        }
    }
    
    if(lose || optind < argc) {
        // Print error message and exit.
        error(0, 0, "%s: %s", _("extra operand"), argv[optind]);
        print_help(stderr);
    }

    time_format = time_format_raw;

    print_idle_time();

    exit(EXIT_SUCCESS);
}

// Open the display
static void open_display() {
    dpy = XOpenDisplay(NULL);
    if(dpy == NULL) {
        fprintf(stderr, _("Error: couldn't open display\n"));
        exit(EXIT_FAILURE);
    }
}

// Check if the XScreenSaver extension is supported by the X server
static void check_xss_supported(int event_basep, int error_basep) {
    if(!XScreenSaverQueryExtension(dpy, &event_basep, &error_basep)) {
        fprintf(stderr, _("Error: screen saver extension not supported\n"));
        exit(EXIT_FAILURE);
    }
}

// Try to query the current idle time
static void try_xss_query(XScreenSaverInfo ssi) {
    if(!XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), &ssi)) {
        fprintf(stderr, _("Error: couldn't query screen saver info\n"));
        exit(EXIT_FAILURE);
    }
}

// Print the current idle time
static void print_idle_time() {
    open_display();

    XScreenSaverInfo ssi;
    int event_basep, error_basep;

    // Create sigaction struct
    struct sigaction act;
    memset(&act, '\0', sizeof(act));

    // Use the sa_sigaction field because the handles
    // have two additional parameters
    act.sa_sigaction = &signal_callback_handler;

    // The SA_SIGINFO flag tells sigaction() to use the
    // sa_sigaction field, not sa_handler.
    act.sa_flags = SA_SIGINFO;

    // Register signal and signal handler
    if(sigaction(SIGTERM, &act, NULL) < 0) {
        perror(_("Error: sigaction"));
        exit(EXIT_FAILURE);
    }

    // Enable line buffering on stdout
    setlinebuf(stdout);

    // Get the current idle time
    check_xss_supported(event_basep, error_basep);
    try_xss_query(ssi);
    printf("%lu\n", workaroundX11(dpy, ssi.idle));
}

// Print a string in multibyte format
static void mbprint(const char* msg) {
    size_t len = mbsrtowcs(NULL, &msg, 0, NULL);
    if(len == (size_t)-1) {
        error(EXIT_FAILURE, errno, _("conversion to multibyte string failed"));
    }
    wchar_t* mb_msg = xmalloc((len + 1) * sizeof(wchar_t));
    mbsrtowcs(mb_msg, &msg, len + 1, NULL);
    wprintf(L"%ls\n", mb_msg);
    free(mb_msg);
}

// Signal callback handler
static void signal_callback_handler(int sig,
                                    siginfo_t *siginfo,
                                    void *context) {
    XCloseDisplay(dpy);
}

// Print help info. The message is split into pieces for the translators.
static void print_help(FILE *out) {
    const char *lc_messages = setlocale(LC_MESSAGES, NULL);
    /* TRANSLATORS: --help output 1 (synopsis)
       no-wrap */
    fprintf(out, _("Usage: %s [OPTION]...\n"), program_name);
    /* TRANSLATORS: --help output 2 (brief description)
       no-wrap */
    fputs(_("Prints the user's X11 idle time.\n"), out);
    fputs("\n", out);
    /* TRANSLATORS: --help output 3: options
       no-wrap */
    fputs(_(" -f, --format[=]FMT  use FMT as the time format\n"), out);
    fputs(_(" -h, --help          display this help\n"), out);
    fputs(_("     --version       output version information\n"), out);
    fputs("\n", out);
    /* TRANSLATORS: --help output 4+ (reports)
       TRANSLATORS: the placeholder indicates the bug-reporting address
       for this application.
       no-wrap */
    fprintf(out, _("Report bugs to: %s\n"),            BUG_URL);
    fprintf(out, _("Report translation bugs to %s\n"), TRANSLATION_URL);
    fprintf(out, _("xprintidle-ng home page: %s\n"),   PROJECT_URL);
    exit(out == stderr ? EXIT_FAILURE : EXIT_SUCCESS);
}

// Print version and copyright information.
static void print_version(FILE *out) {
    fprintf(out, "%s (%s) %s\n", PACKAGE, PACKAGE_NAME, VERSION);
    /* xgettext: no-wrap */
    fputs("", out);

    // It is important to separate the year from the rest of the message,
    // as done here, to avoid having to retranslate the message when a new
    // year comes around.
    fprintf(out, _("Copyright © %d %s"), COPYRIGHT_YEAR, AUTHORS);
    fputs(_("License GPLv2: GNU GPL version 2\n"), out);
    fputs(_("<http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html>"), out);
    fputs(_("This is free software: "
            "you are free to change and redistribute it.\n"), out);
    fputs(_("There is NO WARRANTY, to the extent permitted by law.\n"), out);
}

/*!
 * This function works around an XServer idleTime bug in the
 * XScreenSaverExtension if dpms is running. In this case the current
 * dpms-state time is always subtracted from the current idletime.
 * This means: XScreenSaverInfo->idle is not the time since the last
 * user activity, as descriped in the header file of the extension.
 * This result in SUSE bug # and sf.net bug #. The bug in the XServer itself
 * is reported at https://bugs.freedesktop.org/buglist.cgi?quicksearch=6439.
 *
 * Workaround: Check if if XServer is in a dpms state, check the
 *             current timeout for this state and add this value to
 *             the current idle time and return.
 *
 * \param  _idleTime   an unsigned long value with the current idletime from
 *                     XScreenSaverInfo->idle
 * \return             an unsigned long with the corrected idletime
 */
static unsigned long workaroundX11(Display *dpy, unsigned long _idle) {
    int dummy;
    CARD16 standby, suspend, off;
    CARD16 state;
    BOOL onoff;

    if(DPMSQueryExtension(dpy, &dummy, &dummy)) {
        if(DPMSCapable(dpy)) {
            DPMSGetTimeouts(dpy, &standby, &suspend, &off);
            DPMSInfo(dpy, &state, &onoff);

            if(onoff) {
                switch(state) {
                case DPMSModeStandby:
                    // this check is a little bit paranoid, but just to be sure
                    if(_idle < (unsigned) (standby * 1000)) {
                        _idle += (standby * 1000);
                    }
                    break;
                case DPMSModeSuspend:
                    if(_idle < (unsigned) ((suspend + standby) * 1000)) {
                        _idle += ((suspend + standby) * 1000);
                    }
                    break;
                case DPMSModeOff:
                    if(_idle < (unsigned) ((off + suspend + standby) * 1000)) {
                        _idle += ((off + suspend + standby) * 1000);
                    }
                    break;
                case DPMSModeOn:
                    break;
                default:
                    break;
                }
            }
        }
    }

    return _idle;
}
