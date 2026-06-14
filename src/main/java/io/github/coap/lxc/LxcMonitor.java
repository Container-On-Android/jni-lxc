package io.github.coap.lxc;

import android.os.RemoteException;
import android.util.Log;

import io.github.coap.ILxc;

/**
 * Passive monitor for LXC container state changes. Backed by the
 * {@code lxc_monitor_open} / {@code lxc_monitor_read} API which receives
 * state-change events pushed by {@code lxc-monitord} over a Unix domain
 * socket — no polling required.
 *
 * The wire format mirrors liblxc's {@code struct lxc_msg}:
 * {@code int type, int value, char name[4096]}. monitord forwards the
 * full struct, so name and state are reliable here.
 */
public class LxcMonitor {

    /** Message types as defined by lxc_msg_type_t in lxc/monitor.h. */
    public static final int TYPE_STATE = 0;
    public static final int TYPE_EXIT_CODE = 1;

    /** A single event delivered by lxc-monitord. */
    public static final class Event {
        public final int type;
        public final String name;
        public final String state;
        public final int value;

        public Event(int type, String name, String state, int value) {
            this.type = type;
            this.name = name;
            this.state = state;
            this.value = value;
        }

        @Override
        public String toString() {
            return String.format("LxcEvent{type=%d, name='%s', state='%s', value=%d}",
                    type, name, state, value);
        }
    }

    private static final String TAG = "LxcMonitor";

    private final ILxc service;
    private final long handle;
    private volatile boolean closed = false;

    /* package */ LxcMonitor(ILxc service, long handle) {
        this.service = service;
        this.handle = handle;
    }

    public long getHandle() {
        return handle;
    }

    /**
     * Blocks until the next monitor event is available.
     *
     * @return the event, or {@code null} if the monitor is closed.
     */
    public Event readNext() {
        if (closed) {
            return null;
        }
        try {
            String[] raw = service.readMonitorEvent(handle);
            if (raw == null || raw.length < 4) {
                return null;
            }
            int type = parseIntSafe(raw[0]);
            String name = raw[1] != null ? raw[1] : "";
            String state = raw[2] != null ? raw[2] : "";
            int value = parseIntSafe(raw[3]);
            return new Event(type, name, state, value);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to read monitor event: " + e.getMessage());
            return null;
        }
    }

    private static int parseIntSafe(String s) {
        if (s == null) return 0;
        try {
            return Integer.parseInt(s);
        } catch (NumberFormatException e) {
            return 0;
        }
    }

    public void close() {
        if (closed) {
            return;
        }
        closed = true;
        try {
            service.closeMonitor(handle);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to close monitor: " + e.getMessage());
        }
    }
}
