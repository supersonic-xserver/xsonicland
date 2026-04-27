#ifndef _XSERVER_OS_AUTH_H
#define _XSERVER_OS_AUTH_H

#include <X11/X.h>

#include "dix.h"

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

#define LCC_UID_SET     (1 << 0)
#define LCC_GID_SET     (1 << 1)
#define LCC_PID_SET     (1 << 2)
#define LCC_ZID_SET     (1 << 3)

void EnableLocalAccess(void);
void DisableLocalAccess(void);

void LocalAccessScopeUser(void);

void InitAuthorization(const char *filename);

int AuthorizationFromID(XID id,
                        unsigned short *name_lenp,
                        const char **namep,
                        unsigned short *data_lenp, char **datap);

XID CheckAuthorization(unsigned int namelength,
                       const char *name,
                       unsigned int datalength,
                       const char *data,
                       ClientPtr client,
                       const char **reason);

void ResetAuthorization(void);

int RemoveAuthorization(unsigned short name_length,
                        const char *name,
                        unsigned short data_length, const char *data);

int AddAuthorization(unsigned int name_length,
                     const char *name,
                     unsigned int data_length,
                     char *data);

XID GenerateAuthorization(unsigned int name_length,
                          const char *name,
                          unsigned int data_length,
                          const char *data,
                          unsigned int *data_length_return,
                          char **data_return);

void RegisterAuthorizations(void);

typedef struct sockaddr *sockaddrPtr;

void AddLocalHosts(void);
void ResetHosts(const char *display);

/* register local hosts entries for outself, based on listening fd */
void DefineSelf(int fd);

/* check whether given addr belongs to ourself */
void AugmentSelf(void *from, int len);

void AccessUsingXdmcp(void);

extern Bool defeatAccessControl;

#endif /* _XSERVER_OS_AUTH_H */
