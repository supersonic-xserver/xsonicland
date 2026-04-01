/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#ifndef _OSDEP_H_
#define _OSDEP_H_ 1

#include <X11/Xdefs.h>

#if defined(XDMCP) || defined(HASXDMAUTH)
#include <X11/Xdmcp.h>
#endif

#include <limits.h>
#include <stddef.h>
#include <X11/Xos.h>
#include <X11/Xmd.h>

#ifndef __has_builtin
# define __has_builtin(x) 0     /* Compatibility with older compilers */
#endif

/* If EAGAIN and EWOULDBLOCK are distinct errno values, then we check errno
 * for both EAGAIN and EWOULDBLOCK, because some supposedly POSIX
 * systems are broken and return EWOULDBLOCK when they should return EAGAIN
 */
#ifndef WIN32
# if (EAGAIN != EWOULDBLOCK)
#  define ETEST(err) (err == EAGAIN || err == EWOULDBLOCK)
# else
#  define ETEST(err) (err == EAGAIN)
# endif
#else   /* WIN32 The socket errorcodes differ from the normal errors */
#define ETEST(err) (err == EAGAIN || err == WSAEWOULDBLOCK)
#endif

#if defined(XDMCP) || defined(HASXDMAUTH)
typedef Bool (*ValidatorFunc) (ARRAY8Ptr Auth, ARRAY8Ptr Data, int packet_type);
typedef Bool (*GeneratorFunc) (ARRAY8Ptr Auth, ARRAY8Ptr Data, int packet_type);
typedef Bool (*AddAuthorFunc) (unsigned name_length, const char *name,
                               unsigned data_length, char *data);
#endif

typedef struct _connectionInput *ConnectionInputPtr;
typedef struct _connectionOutput *ConnectionOutputPtr;

struct _osComm;

#define AuthInitArgs void
typedef void (*AuthInitFunc) (AuthInitArgs);

#define AuthAddCArgs unsigned short data_length, const char *data, XID id
typedef int (*AuthAddCFunc) (AuthAddCArgs);

#define AuthCheckArgs unsigned short data_length, const char *data, ClientPtr client, const char **reason
typedef XID (*AuthCheckFunc) (AuthCheckArgs);

#define AuthFromIDArgs XID id, unsigned short *data_lenp, char **datap
typedef int (*AuthFromIDFunc) (AuthFromIDArgs);

#define AuthGenCArgs unsigned data_length, const char *data, XID id, unsigned *data_length_return, char **data_return
typedef XID (*AuthGenCFunc) (AuthGenCArgs);

#define AuthRemCArgs unsigned short data_length, const char *data
typedef int (*AuthRemCFunc) (AuthRemCArgs);

#define AuthRstCArgs void
typedef int (*AuthRstCFunc) (AuthRstCArgs);

typedef void (*OsCloseFunc) (ClientPtr);

typedef int (*OsFlushFunc) (ClientPtr who, struct _osComm * oc, char *extraBuf,
                            int extraCount);

typedef struct _osComm {
    int fd;
    ConnectionInputPtr input;
    ConnectionOutputPtr output;
    XID auth_id;                /* authorization id */
    CARD32 conn_time;           /* timestamp if not established, else 0  */
    struct _XtransConnInfo *trans_conn; /* transport connection object */
    int flags;
} OsCommRec, *OsCommPtr;

#define OS_COMM_GRAB_IMPERVIOUS 1
#define OS_COMM_IGNORED         2

extern int FlushClient(ClientPtr /*who */ ,
                       OsCommPtr /*oc */ ,
                       const void * /*extraBuf */ ,
                       int      /*extraCount */
    );

extern void FreeOsBuffers(OsCommPtr     /*oc */
    );

void
CloseDownFileDescriptor(OsCommPtr oc);

#include "dix.h"
#include "ospoll.h"

extern struct ospoll    *server_poll;

Bool
listen_to_client(ClientPtr client);

extern Bool NewOutputPending;

/* in access.c */
extern Bool ComputeLocalClient(ClientPtr client);

/* in auth.c */
extern void GenerateRandomData(int len, char *buf);

/* in mitauth.c */
extern XID MitCheckCookie(AuthCheckArgs);
extern XID MitGenerateCookie(AuthGenCArgs);
extern int MitAddCookie(AuthAddCArgs);
extern int MitFromID(AuthFromIDArgs);
extern int MitRemoveCookie(AuthRemCArgs);
extern int MitResetCookie(AuthRstCArgs);

/* in xdmauth.c */
#ifdef HASXDMAUTH
extern XID XdmCheckCookie(AuthCheckArgs);
extern int XdmAddCookie(AuthAddCArgs);
extern int XdmFromID(AuthFromIDArgs);
extern int XdmRemoveCookie(AuthRemCArgs);
extern int XdmResetCookie(AuthRstCArgs);
#endif

#ifdef XDMCP
/* in xdmcp.c */
extern void XdmcpUseMsg(void);
extern int XdmcpOptions(int argc, char **argv, int i);
extern void XdmcpRegisterConnection(int type, const char *address, int addrlen);
extern void XdmcpRegisterAuthorizations(void);
extern void XdmcpRegisterAuthorization(const char *name, int namelen);
extern void XdmcpInit(void);
extern void XdmcpReset(void);
extern void XdmcpOpenDisplay(int sock);
extern void XdmcpCloseDisplay(int sock);
extern void XdmcpRegisterAuthentication(const char *name,
                                        int namelen,
                                        const char *data,
                                        int datalen,
                                        ValidatorFunc Validator,
                                        GeneratorFunc Generator,
                                        AddAuthorFunc AddAuth);

struct sockaddr_in;
extern void XdmcpRegisterBroadcastAddress(const struct sockaddr_in *addr);
#endif

#ifdef HASXDMAUTH
extern void XdmAuthenticationInit(const char *cookie, int cookie_length);
#endif

#ifdef WIN32
#include <X11/Xwinsock.h>
struct utsname {
    char nodename[512];
};

static inline void uname(struct utsname *uts) {
    gethostname(uts->nodename, sizeof(uts->nodename));
}
#endif /* WIN32 */

void AutoResetServer(int sig);

/* clone fd so it gets out of our select mask */
int os_move_fd(int fd);

/* lookup builtin color by name */
Bool OsLookupColor(int screen,
                   char *name,
                   unsigned len,
                   unsigned short *pred,
                   unsigned short *pgreen,
                   unsigned short *pblue);

/* set signal mask - either on current thread or whole process,
   depending on whether multithreading is used */
int xthread_sigmask(int how, const sigset_t *set, sigset_t *oldest);

#ifdef DDXOSVERRORF
/* callback for DDX specific error printing, if any (may be NULL) */
extern void (*OsVendorVErrorFProc) (const char *, va_list args)
    _X_ATTRIBUTE_PRINTF(1, 0);
#endif

void MakeClientGrabPervious(ClientPtr client);
void MakeClientGrabImpervious(ClientPtr client);

int OnlyListenToOneClient(ClientPtr client);

void ListenToAllClients(void);

/* allow DDX to force using another clock */
void ForceClockId(clockid_t forced_clockid);

Bool WaitForSomething(Bool clients_are_ready);
void CloseDownConnection(ClientPtr client);

#if __has_builtin(__builtin_popcountl)
# define Xpopcountl __builtin_popcountl
#else
/*
 * Count the number of bits set to 1 in a 32-bit word.
 * Algorithm from MIT AI Lab Memo 239: "HAKMEM", ITEM 169.
 * https://dspace.mit.edu/handle/1721.1/6086
 */
static inline int
Xpopcountl(unsigned long mask)
{
    unsigned long y;

    y = (mask >> 1) & 033333333333;
    y = mask - y - ((y >> 1) & 033333333333);
    return (((y + (y >> 3)) & 030707070707) % 077);
}
#endif
#define Ones Xpopcountl

#endif                          /* _OSDEP_H_ */
