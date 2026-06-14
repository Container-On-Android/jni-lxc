package io.github.coap.lxc;

import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.NonNull;

import com.topjohnwu.superuser.ipc.RootService;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicLong;

import io.github.coap.ILxc;

public class LxcNative extends RootService {
    public static String LXC_PATH = "/data/share/var/lib/lxc";

    static {
        System.loadLibrary("lxc-wrapper");
    }

    public static String getVersion() {
        return nativeGetVersion();
    }

    @SuppressWarnings({"JniMissingFunction", "unused"})
    static class LxcIPC extends ILxc.Stub {
        private static final String TAG = "LxcIPC";

        private static final class MonitorEntry {
            final long nativeHandle;
            final BlockingQueue<String[]> queue = new LinkedBlockingQueue<>();
            final Thread readerThread;
            volatile boolean closed = false;

            MonitorEntry(long nativeHandle, long publicHandle) {
                this.nativeHandle = nativeHandle;
                this.readerThread = new Thread(() -> {
                    try {
                        while (!closed) {
                            String[] event = nativeReadMonitorEvent(nativeHandle);
                            if (event == null || closed) {
                                break;
                            }
                            queue.offer(event);
                        }
                    } catch (Throwable t) {
                        Log.e(TAG, "Monitor reader terminated: " + t.getMessage());
                    }
                }, "LxcMonitorReader-" + publicHandle);
                this.readerThread.setDaemon(true);
                this.readerThread.start();
            }
        }

        private final ConcurrentHashMap<Long, MonitorEntry> monitorEntries = new ConcurrentHashMap<>();
        private final AtomicLong nextHandle = new AtomicLong(1);

        @Override
        public int getUid() {
            return 0;
        }

        @Override
        public String[] listContainers(String lxcpath) {
            return nativeListContainers(lxcpath);
        }

        @Override
        public boolean isDefined(String name, String lxcpath) {
            return nativeIsDefined(name, lxcpath);
        }

        @Override
        public boolean isRunning(String name, String lxcpath) {
            return nativeIsRunning(name, lxcpath);
        }

        @Override
        public String getState(String name, String lxcpath) {
            return nativeGetState(name, lxcpath);
        }

        @Override
        public boolean startContainer(String name, String lxcpath, boolean useInit) {
            return nativeStartContainer(name, lxcpath, useInit);
        }

        @Override
        public boolean stopContainer(String name, String lxcpath) {
            return nativeStopContainer(name, lxcpath);
        }

        @Override
        public boolean freezeContainer(String name, String lxcpath) {
            return nativeFreezeContainer(name, lxcpath);
        }

        @Override
        public boolean unfreezeContainer(String name, String lxcpath) {
            return nativeUnfreezeContainer(name, lxcpath);
        }

        @Override
        public boolean destroyContainer(String name, String lxcpath) {
            return nativeDestroyContainer(name, lxcpath);
        }

        @Override
        public boolean reboot(String name, String lxcpath) {
            return nativeReboot(name, lxcpath);
        }

        @Override
        public boolean shutdown(String name, String lxcpath, int timeout) {
            return nativeShutdown(name, lxcpath, timeout);
        }

        @Override
        public boolean wait(String name, String lxcpath, String state, int timeout) {
            return nativeWait(name, lxcpath, state, timeout);
        }

        @Override
        public String getConfigItem(String name, String lxcpath, String key) {
            return nativeGetConfigItem(name, lxcpath, key);
        }

        @Override
        public boolean setConfigItem(String name, String lxcpath, String key, String value) {
            return nativeSetConfigItem(name, lxcpath, key, value);
        }

        @Override
        public boolean clearConfig(String name, String lxcpath) {
            return nativeClearConfig(name, lxcpath);
        }

        @Override
        public boolean clearConfigItem(String name, String lxcpath, String key) {
            return nativeClearConfigItem(name, lxcpath, key);
        }

        @Override
        public String getCgroupItem(String name, String lxcpath, String key) {
            return nativeGetCgroupItem(name, lxcpath, key);
        }

        @Override
        public boolean setCgroupItem(String name, String lxcpath, String key, String value) {
            return nativeSetCgroupItem(name, lxcpath, key, value);
        }

        @Override
        public String getConfigPath(String name, String lxcpath) {
            return nativeGetConfigPath(name, lxcpath);
        }

        @Override
        public boolean setConfigPath(String name, String lxcpath, String path) {
            return nativeSetConfigPath(name, lxcpath, path);
        }

        @Override
        public boolean saveConfig(String name, String lxcpath, String altFile) {
            return nativeSaveConfig(name, lxcpath, altFile);
        }

        @Override
        public boolean loadConfig(String name, String lxcpath, String altFile) {
            return nativeLoadConfig(name, lxcpath, altFile);
        }

        @Override
        public String configFileName(String name, String lxcpath) {
            return nativeConfigFileName(name, lxcpath);
        }

        @Override
        public String getKeys(String name, String lxcpath, String key) {
            return nativeGetKeys(name, lxcpath, key);
        }

        @Override
        public boolean mayControl(String name, String lxcpath) {
            return nativeMayControl(name, lxcpath);
        }

        @Override
        public boolean setTimeout(String name, String lxcpath, int timeout) {
            return nativeSetTimeout(name, lxcpath, timeout);
        }

        @Override
        public int createSnapshot(String name, String lxcpath) {
            return nativeCreateSnapshot(name, lxcpath);
        }

        @Override
        public int snapshotList(String name, String lxcpath) {
            return nativeSnapshotList(name, lxcpath);
        }

        @Override
        public boolean snapshotDestroy(String name, String lxcpath, String snapname) {
            return nativeSnapshotDestroy(name, lxcpath, snapname);
        }

        @Override
        public boolean snapshotDestroyAll(String name, String lxcpath) {
            return nativeSnapshotDestroyAll(name, lxcpath);
        }

        @Override
        public boolean snapshotRestore(String name, String lxcpath, String snapname, String newname) {
            return nativeSnapshotRestore(name, lxcpath, snapname, newname);
        }

        @Override
        public String[] getInterfaces(String name, String lxcpath) {
            return nativeGetInterfaces(name, lxcpath);
        }

        @Override
        public String[] getIps(String name, String lxcpath, String interface_, String family, int scope) {
            return nativeGetIps(name, lxcpath, interface_, family, scope);
        }

        @Override
        public boolean attachInterface(String name, String lxcpath, String dev, String dstDev) {
            return nativeAttachInterface(name, lxcpath, dev, dstDev);
        }

        @Override
        public boolean detachInterface(String name, String lxcpath, String dev, String dstDev) {
            return nativeDetachInterface(name, lxcpath, dev, dstDev);
        }

        @Override
        public boolean addDeviceNode(String name, String lxcpath, String srcPath, String destPath) {
            return nativeAddDeviceNode(name, lxcpath, srcPath, destPath);
        }

        @Override
        public boolean removeDeviceNode(String name, String lxcpath, String srcPath, String destPath) {
            return nativeRemoveDeviceNode(name, lxcpath, srcPath, destPath);
        }

        @Override
        public int getConsoleFd(String name, String lxcpath, int ttynum) {
            return nativeGetConsoleFd(name, lxcpath, ttynum);
        }

        @Override
        public boolean console(String name, String lxcpath, int ttynum, int stdinfd, int stdoutfd, int stderrfd, int escape) {
            return nativeConsole(name, lxcpath, ttynum, stdinfd, stdoutfd, stderrfd, escape);
        }

        @Override
        public boolean clone(String name, String lxcpath, String newname, String newlxcpath, int flags, String bdevtype) {
            return nativeClone(name, lxcpath, newname, newlxcpath, flags, bdevtype);
        }

        @Override
        public boolean create(String name, String lxcpath, String t, String bdevtype, int flags, String[] argv) {
            return nativeCreate(name, lxcpath, t, bdevtype, flags, argv);
        }

        @Override
        public boolean rename(String name, String lxcpath, String newname) {
            return nativeRename(name, lxcpath, newname);
        }

        @Override
        public boolean checkpoint(String name, String lxcpath, String directory, boolean stop, boolean verbose) {
            return nativeCheckpoint(name, lxcpath, directory, stop, verbose);
        }

        @Override
        public boolean restore(String name, String lxcpath, String directory, boolean verbose) {
            return nativeRestore(name, lxcpath, directory, verbose);
        }

        @Override
        public boolean destroyWithSnapshots(String name, String lxcpath) {
            return nativeDestroyWithSnapshots(name, lxcpath);
        }

        @Override
        public int attachRunWait(String name, String lxcpath, boolean clearEnv, int namespaces, long personality, int uid, int gid, String[] argv, int attachFlags) {
            return nativeAttachRunWait(name, lxcpath, clearEnv, namespaces, personality, uid, gid, argv, attachFlags);
        }

        @Override
        public int attachNoWait(String name, String lxcpath, boolean clearEnv, int namespaces, long personality, int uid, int gid, String[] argv, int attachFlags) {
            return nativeAttachNoWait(name, lxcpath, clearEnv, namespaces, personality, uid, gid, argv, attachFlags);
        }

        @Override
        public int attach(String name, String lxcpath, boolean clearEnv, int namespaces, long personality, int uid, int gid, int stdinfd, int stdoutfd, int stderrfd, int attachFlags) {
            return nativeAttach(name, lxcpath, clearEnv, namespaces, personality, uid, gid, stdinfd, stdoutfd, stderrfd, attachFlags);
        }

        @Override
        public int initPid(String name, String lxcpath) {
            return nativeInitPid(name, lxcpath);
        }

        @Override
        public int initPidfd(String name, String lxcpath) {
            return nativeInitPidfd(name, lxcpath);
        }

        @Override
        public int devptsFd(String name, String lxcpath) {
            return nativeDevptsFd(name, lxcpath);
        }

        @Override
        public int seccompNotifyFd(String name, String lxcpath) {
            return nativeSeccompNotifyFd(name, lxcpath);
        }

        @Override
        public int seccompNotifyFdActive(String name, String lxcpath) {
            return nativeSeccompNotifyFdActive(name, lxcpath);
        }

        @Override
        public boolean wantDaemonize(String name, String lxcpath, boolean state) {
            return nativeWantDaemonize(name, lxcpath, state);
        }

        @Override
        public boolean wantCloseAllFds(String name, String lxcpath, boolean state) {
            return nativeWantCloseAllFds(name, lxcpath, state);
        }

        @Override
        public int getErrorNum(String name, String lxcpath) {
            return nativeGetErrorNum(name, lxcpath);
        }

        @Override
        public boolean configItemIsSupported(String key) {
            return nativeConfigItemIsSupported(key);
        }

        @Override
        public boolean hasApiExtension(String extension) {
            return nativeHasApiExtension(extension);
        }

        @Override
        public int ensureMonitord(String lxcpath) {
            String path = (lxcpath == null || lxcpath.isEmpty()) ? LXC_PATH : lxcpath;
            int ret = nativeEnsureMonitord(path);
            Log.d(TAG, "ensureMonitord path=" + path + " ret=" + ret);
            return ret;
        }

        @Override
        public long openMonitor(String lxcpath) {
            String path = (lxcpath == null || lxcpath.isEmpty()) ? LXC_PATH : lxcpath;
            long nativeHandle = nativeOpenMonitor(path);
            if (nativeHandle == 0) {
                Log.e(TAG, "Failed to open LXC monitor for " + path);
                return 0;
            }
            long handle = nextHandle.getAndIncrement();
            MonitorEntry entry = new MonitorEntry(nativeHandle, handle);
            monitorEntries.put(handle, entry);
            Log.d(TAG, "Opened LXC monitor handle=" + handle);
            return handle;
        }

        @Override
        public int closeMonitor(long handle) {
            MonitorEntry entry = monitorEntries.remove(handle);
            if (entry == null) {
                return -1;
            }
            entry.closed = true;
            int ret = nativeCloseMonitor(entry.nativeHandle);
            entry.readerThread.interrupt();
            // Unblock any pending readMonitorEvent with a sentinel value.
            entry.queue.offer(new String[0]);
            Log.d(TAG, "Closed LXC monitor handle=" + handle + " ret=" + ret);
            return ret;
        }

        @Override
        public String[] readMonitorEvent(long handle) {
            MonitorEntry entry = monitorEntries.get(handle);
            if (entry == null) {
                return null;
            }
            try {
                String[] event = entry.queue.take();
                if (event.length == 0) {
                    return null; // sentinel pushed on close
                }
                return event;
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                return null;
            }
        }

        /* package */ void closeAllMonitors() {
            // Snapshot the key set to avoid concurrent modification while
            // closeMonitor() removes entries from the map.
            Long[] handles = monitorEntries.keySet().toArray(new Long[0]);
            for (Long handle : handles) {
                try {
                    closeMonitor(handle);
                } catch (Throwable t) {
                    Log.e(TAG, "closeAllMonitors error: " + t.getMessage());
                }
            }
        }
    }

    private final LxcIPC ipc = new LxcIPC();

    @SuppressWarnings("JniMissingFunction")
    private static native String nativeGetVersion();
    private static native String[] nativeListContainers(String lxcpath);
    private static native boolean nativeIsDefined(String name, String lxcpath);
    private static native boolean nativeIsRunning(String name, String lxcpath);
    private static native String nativeGetState(String name, String lxcpath);
    private static native boolean nativeStartContainer(String name, String lxcpath, boolean useInit);
    private static native boolean nativeStopContainer(String name, String lxcpath);
    private static native boolean nativeFreezeContainer(String name, String lxcpath);
    private static native boolean nativeUnfreezeContainer(String name, String lxcpath);
    private static native boolean nativeDestroyContainer(String name, String lxcpath);
    private static native boolean nativeReboot(String name, String lxcpath);
    private static native boolean nativeShutdown(String name, String lxcpath, int timeout);
    private static native boolean nativeWait(String name, String lxcpath, String state, int timeout);
    private static native String nativeGetConfigItem(String name, String lxcpath, String key);
    private static native boolean nativeSetConfigItem(String name, String lxcpath, String key, String value);
    private static native boolean nativeClearConfig(String name, String lxcpath);
    private static native boolean nativeClearConfigItem(String name, String lxcpath, String key);
    private static native String nativeGetCgroupItem(String name, String lxcpath, String key);
    private static native boolean nativeSetCgroupItem(String name, String lxcpath, String key, String value);
    private static native String nativeGetConfigPath(String name, String lxcpath);
    private static native boolean nativeSetConfigPath(String name, String lxcpath, String path);
    private static native boolean nativeSaveConfig(String name, String lxcpath, String altFile);
    private static native boolean nativeLoadConfig(String name, String lxcpath, String altFile);
    private static native String nativeConfigFileName(String name, String lxcpath);
    private static native String nativeGetKeys(String name, String lxcpath, String key);
    private static native boolean nativeMayControl(String name, String lxcpath);
    private static native boolean nativeSetTimeout(String name, String lxcpath, int timeout);
    private static native int nativeCreateSnapshot(String name, String lxcpath);
    private static native int nativeSnapshotList(String name, String lxcpath);
    private static native boolean nativeSnapshotDestroy(String name, String lxcpath, String snapname);
    private static native boolean nativeSnapshotDestroyAll(String name, String lxcpath);
    private static native boolean nativeSnapshotRestore(String name, String lxcpath, String snapname, String newname);
    private static native String[] nativeGetInterfaces(String name, String lxcpath);
    private static native String[] nativeGetIps(String name, String lxcpath, String interface_, String family, int scope);
    private static native boolean nativeAttachInterface(String name, String lxcpath, String dev, String dstDev);
    private static native boolean nativeDetachInterface(String name, String lxcpath, String dev, String dstDev);
    private static native boolean nativeAddDeviceNode(String name, String lxcpath, String srcPath, String destPath);
    private static native boolean nativeRemoveDeviceNode(String name, String lxcpath, String srcPath, String destPath);
    private static native int nativeGetConsoleFd(String name, String lxcpath, int ttynum);
    private static native boolean nativeConsole(String name, String lxcpath, int ttynum, int stdinfd, int stdoutfd, int stderrfd, int escape);
    private static native boolean nativeClone(String name, String lxcpath, String newname, String newlxcpath, int flags, String bdevtype);
    private static native boolean nativeCreate(String name, String lxcpath, String t, String bdevtype, int flags, String[] argv);
    private static native boolean nativeRename(String name, String lxcpath, String newname);
    private static native boolean nativeCheckpoint(String name, String lxcpath, String directory, boolean stop, boolean verbose);
    private static native boolean nativeRestore(String name, String lxcpath, String directory, boolean verbose);
    private static native boolean nativeDestroyWithSnapshots(String name, String lxcpath);
    private static native int nativeAttachRunWait(String name, String lxcpath, boolean clearEnv, int namespaces, long personality, int uid, int gid, String[] argv, int attachFlags);
    private static native int nativeAttachNoWait(String name, String lxcpath, boolean clearEnv, int namespaces, long personality, int uid, int gid, String[] argv, int attachFlags);
    private static native int nativeAttach(String name, String lxcpath, boolean clearEnv, int namespaces, long personality, int uid, int gid, int stdinfd, int stdoutfd, int stderrfd, int attachFlags);
    private static native int nativeInitPid(String name, String lxcpath);
    private static native int nativeInitPidfd(String name, String lxcpath);
    private static native int nativeDevptsFd(String name, String lxcpath);
    private static native int nativeSeccompNotifyFd(String name, String lxcpath);
    private static native int nativeSeccompNotifyFdActive(String name, String lxcpath);
    private static native boolean nativeWantDaemonize(String name, String lxcpath, boolean state);
    private static native boolean nativeWantCloseAllFds(String name, String lxcpath, boolean state);
    private static native int nativeGetErrorNum(String name, String lxcpath);
    private static native boolean nativeConfigItemIsSupported(String key);
    private static native boolean nativeHasApiExtension(String extension);
    private static native long nativeOpenMonitor(String lxcpath);
    private static native int nativeCloseMonitor(long handle);
    private static native String[] nativeReadMonitorEvent(long handle);
    private static native int nativeEnsureMonitord(String lxcpath);

    @Override
    public void onCreate() {
        Log.d("LXC", "LXC Service Created");
    }

    @Override
    public void onRebind(@NonNull Intent intent) {
        super.onRebind(intent);
        Log.d("LXC", "LXC Service Rebound");
    }

    @Override
    public IBinder onBind(@NonNull Intent intent) {
        Log.d("LXC", "LXC Service Bound");
        return ipc;
    }

    @Override
    public boolean onUnbind(@NonNull Intent intent) {
        Log.d("LXC", "LXC Service Unbound");
        return true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        ipc.closeAllMonitors();
        Log.d("LXC", "LXC Service Destroyed");
    }
}
