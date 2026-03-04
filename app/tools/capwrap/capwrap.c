/*
 * capwrap — capability wrapper for running zephyr.exe without sudo.
 *
 * The native_sim zephyr.exe needs CAP_NET_ADMIN and CAP_NET_RAW to open
 * HCI sockets for BLE.  Normally this requires running as root (sudo),
 * and setcap on zephyr.exe itself is lost every time it is recompiled.
 *
 * This small wrapper is granted the file capabilities once:
 *     sudo setcap cap_net_admin,cap_net_raw,cap_setpcap+ep capwrap
 *
 * It then raises the caps into the ambient set and exec's zephyr.exe
 * (or any other program) so the child inherits them — without being
 * setuid or having file caps itself.
 *
 * Build:
 *     gcc -O2 -o capwrap capwrap.c -lcap
 *
 * One-time setup (after building):
 *     sudo setcap cap_net_admin,cap_net_raw,cap_setpcap+ep capwrap
 *
 * Usage:
 *     ./capwrap /path/to/zephyr.exe --bt-dev=hci0
 *
 * Verify capabilities propagate:
 *     ./capwrap /bin/sh -c \
 *       'cat /proc/$$/status | grep -E "^Cap(Eff|Bnd|Amb)"'
 *
 * Notes:
 *   - cap_setpcap on the wrapper allows adjusting the inheritable set,
 *     which is required for raising ambient caps on some kernels.
 *   - Ambient caps propagate across execve only to non-privileged targets.
 *     Do NOT mark zephyr.exe setuid or give it file capabilities.
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/capability.h>
#include <errno.h>

static int raise_ambient(cap_value_t cap)
{
    if (prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, cap, 0, 0) == -1) {
        perror("prctl(PR_CAP_AMBIENT_RAISE)");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: capwrap /path/to/zephyr.exe [args...]\n");
        return 2;
    }

    cap_value_t list[2] = { CAP_NET_ADMIN, CAP_NET_RAW };

    /* Put caps in inheritable, permitted, effective */
    cap_t caps = cap_get_proc();
    if (!caps) {
        perror("cap_get_proc");
        return 1;
    }
    if (cap_set_flag(caps, CAP_INHERITABLE, 2, list, CAP_SET) == -1) {
        perror("cap_set_flag inheritable");
        return 1;
    }
    if (cap_set_flag(caps, CAP_PERMITTED, 2, list, CAP_SET) == -1) {
        perror("cap_set_flag permitted");
        return 1;
    }
    if (cap_set_flag(caps, CAP_EFFECTIVE, 2, list, CAP_SET) == -1) {
        perror("cap_set_flag effective");
        return 1;
    }
    if (cap_set_proc(caps) == -1) {
        perror("cap_set_proc");
        return 1;
    }
    cap_free(caps);

    /* Raise ambient so caps survive execve to zephyr.exe */
    if (raise_ambient(CAP_NET_ADMIN) == -1) {
        return 1;
    }
    if (raise_ambient(CAP_NET_RAW) == -1) {
        return 1;
    }

    execvp(argv[1], &argv[1]);
    perror("execvp");
    return 1;
}
