/*
 * echo2.c --
 *
 *      Produce a page containing all the inputs (fcgiapp version)
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#ifndef lint
static const char rcsid[] = "$Id: echo2.c,v 1.3 1999/06/10 21:20:35 roberts Exp $";
#endif /* not lint */

#if defined HAVE_UNISTD_H || __linux__
#include <unistd.h>
#endif

#include <stdlib.h>
#include "fcgiapp.h"

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

static void PrintEnv(FCGX_Stream *out, char *label, char **envp)
{
    FCGX_FPrintF(out, "%s:<br>\n<pre>\n", label);
    for( ; *envp != NULL; envp++) {
        FCGX_FPrintF(out, "%s\n", *envp);
    }
    FCGX_FPrintF(out, "</pre><p>\n");
}

void main ()
{
    FCGX_Stream *in, *out, *err;
    FCGX_ParamArray envp;
    int count = 0;

    while (FCGX_Accept(&in, &out, &err, &envp) >= 0) {
        char *contentLength = FCGX_GetParam("CONTENT_LENGTH", envp);
        int len = 0;

        FCGX_FPrintF(out,
           "Content-type: text/html\r\n"
           "\r\n"
           "<title>FastCGI echo (fcgiapp version)</title>"
           "<h1>FastCGI echo (fcgiapp version)</h1>\n"
           "Request number %d,  Process ID: %d<p>\n", ++count, getpid());

        if (contentLength != NULL)
            len = strtol(contentLength, NULL, 10);

        if (len <= 0) {
            FCGX_FPrintF(out, "No data from standard input.<p>\n");
        }
        else {
            int i, ch;

            FCGX_FPrintF(out, "Standard input:<br>\n<pre>\n");
            for (i = 0; i < len; i++) {
                if ((ch = FCGX_GetChar(in)) < 0) {
                    FCGX_FPrintF(out,
                        "Error: Not enough bytes received on standard input<p>\n");
                    break;
                }
                FCGX_PutChar(ch, out);
            }
            FCGX_FPrintF(out, "\n</pre><p>\n");
        }

        PrintEnv(out, "Request environment", envp);
        PrintEnv(out, "Initial environment", environ);
    } /* while */
}
