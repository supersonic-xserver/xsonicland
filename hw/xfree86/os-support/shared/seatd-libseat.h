/*
 * (seatd backend for os-support / libseat) — this header is self-contained
 * and is included both by seatd-libseat.c and, in principle, by the seatd
 * main loop, so it re-declares every type it uses.
 */

#ifndef SEATD_LIBSEAT_H
#define SEATD_LIBSEAT_H

#include <libseat.h>
#include <xorg-config.h>
#include "os.h"
#include "xf86Priv.h"

/* Backend API implemented by seatd-libseat.c */

int  seatd_libseat_init(Bool KeepTty_state);
void seatd_libseat_fini(void);
int  seatd_libseat_open_graphics(const char *path);
int  seatd_libseat_open_device(InputInfoPtr p, int *pfd, Bool *paused);
void seatd_libseat_close_device(InputInfoPtr p);
Bool seatd_libseat_controls_session(void);
int  seatd_libseat_switch_session(int session);

#endif /* SEATD_LIBSEAT_H */
