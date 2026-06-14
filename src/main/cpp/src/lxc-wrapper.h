/*
 * lxc-wrapper.h
 *
 * Layout mirrors the public LXC wire protocol for struct lxc_msg so the
 * JNI can exchange messages with liblxc's lxc_monitor_read without
 * pulling in lxc/monitor.h. liblxc's monitord forwards the full struct
 * (type + value + name[]) to subscribed clients, so a fixed-size name
 * array is enough — the CLI tool lxc-monitor uses the same layout.
 *
 * The custom implementations of lxc_monitor_open/close/read and
 * lxc_state2str are declared with __attribute__((weak)) so that they
 * coexist with the versions bundled in liblxc.a — the linker picks the
 * strong symbol from the static library and the weak fallback is
 * unused.
 */

#ifndef LXC_WRAPPER_H
#define LXC_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#define LXC_MAX_NAME_LEN 4096

typedef enum lxc_msg_type {
    lxc_msg_state,
    lxc_msg_exit_code,
} lxc_msg_type_t;

struct lxc_msg {
    lxc_msg_type_t type;
    char name[LXC_MAX_NAME_LEN];
    int value;
};

struct lxc_monitor; /* opaque */

__attribute__((weak)) struct lxc_monitor *lxc_monitor_open(const char *lxcpath);
__attribute__((weak)) int lxc_monitor_close(struct lxc_monitor *mon);
__attribute__((weak)) int lxc_monitor_read(struct lxc_monitor *mon, struct lxc_msg *msg);
__attribute__((weak)) const char *lxc_state2str(int state);

#ifdef __cplusplus
}
#endif

#endif /* LXC_WRAPPER_H */
