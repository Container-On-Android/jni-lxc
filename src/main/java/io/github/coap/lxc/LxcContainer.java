package io.github.coap.lxc;

import android.os.RemoteException;
import android.util.Log;

import androidx.annotation.NonNull;

import io.github.coap.ILxc;

public class LxcContainer {
    private static final String TAG = "LxcContainer";

    private final String name;
    private final String lxcPath;
    private final ILxc service;

    public LxcContainer(String name, String lxcPath, ILxc service) {
        this.name = name;
        this.lxcPath = lxcPath;
        this.service = service;
    }

    public String getName() {
        return name;
    }

    public String getLxcPath() {
        return lxcPath;
    }

    public boolean isDefined() {
        try {
            return service.isDefined(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to check if container is defined: " + e.getMessage());
            return false;
        }
    }

    public boolean isRunning() {
        try {
            return service.isRunning(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to check if container is running: " + e.getMessage());
            return false;
        }
    }

    public String getState() {
        try {
            return service.getState(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get container state: " + e.getMessage());
            return "UNKNOWN";
        }
    }

    public boolean start(boolean useInit) {
        try {
            return service.startContainer(name, lxcPath, useInit);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to start container: " + e.getMessage());
            return false;
        }
    }

    public boolean start() {
        return start(false);
    }

    public boolean stop() {
        try {
            return service.stopContainer(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to stop container: " + e.getMessage());
            return false;
        }
    }

    public boolean freeze() {
        try {
            return service.freezeContainer(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to freeze container: " + e.getMessage());
            return false;
        }
    }

    public boolean unfreeze() {
        try {
            return service.unfreezeContainer(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to unfreeze container: " + e.getMessage());
            return false;
        }
    }

    public boolean destroy() {
        try {
            return service.destroyContainer(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to destroy container: " + e.getMessage());
            return false;
        }
    }

    public boolean reboot() {
        try {
            return service.reboot(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to reboot container: " + e.getMessage());
            return false;
        }
    }

    public boolean shutdown(int timeout) {
        try {
            return service.shutdown(name, lxcPath, timeout);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to shutdown container: " + e.getMessage());
            return false;
        }
    }

    public boolean wait(String state, int timeout) {
        try {
            return service.wait(name, lxcPath, state, timeout);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to wait for container state: " + e.getMessage());
            return false;
        }
    }

    public String getConfigItem(String key) {
        try {
            return service.getConfigItem(name, lxcPath, key);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get config item: " + e.getMessage());
            return null;
        }
    }

    public boolean setConfigItem(String key, String value) {
        try {
            return service.setConfigItem(name, lxcPath, key, value);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to set config item: " + e.getMessage());
            return false;
        }
    }

    public boolean clearConfig() {
        try {
            return service.clearConfig(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to clear config: " + e.getMessage());
            return false;
        }
    }

    public boolean clearConfigItem(String key) {
        try {
            return service.clearConfigItem(name, lxcPath, key);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to clear config item: " + e.getMessage());
            return false;
        }
    }

    public String getCgroupItem(String key) {
        try {
            return service.getCgroupItem(name, lxcPath, key);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get cgroup item: " + e.getMessage());
            return null;
        }
    }

    public boolean setCgroupItem(String key, String value) {
        try {
            return service.setCgroupItem(name, lxcPath, key, value);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to set cgroup item: " + e.getMessage());
            return false;
        }
    }

    public String getConfigPath() {
        try {
            return service.getConfigPath(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get config path: " + e.getMessage());
            return null;
        }
    }

    public boolean setConfigPath(String path) {
        try {
            return service.setConfigPath(name, lxcPath, path);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to set config path: " + e.getMessage());
            return false;
        }
    }

    public boolean saveConfig(String altFile) {
        try {
            return service.saveConfig(name, lxcPath, altFile);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to save config: " + e.getMessage());
            return false;
        }
    }

    public boolean saveConfig() {
        return saveConfig(null);
    }

    public boolean loadConfig(String altFile) {
        try {
            return service.loadConfig(name, lxcPath, altFile);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to load config: " + e.getMessage());
            return false;
        }
    }

    public boolean loadConfig() {
        return loadConfig(null);
    }

    public String configFileName() {
        try {
            return service.configFileName(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get config file name: " + e.getMessage());
            return null;
        }
    }

    public String getKeys(String key) {
        try {
            return service.getKeys(name, lxcPath, key);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get keys: " + e.getMessage());
            return null;
        }
    }

    public boolean mayControl() {
        try {
            return service.mayControl(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to check may control: " + e.getMessage());
            return false;
        }
    }

    public boolean setTimeout(int timeout) {
        try {
            return service.setTimeout(name, lxcPath, timeout);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to set timeout: " + e.getMessage());
            return false;
        }
    }

    public int createSnapshot() {
        try {
            return service.createSnapshot(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to create snapshot: " + e.getMessage());
            return -1;
        }
    }

    public int snapshotList() {
        try {
            return service.snapshotList(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to list snapshots: " + e.getMessage());
            return -1;
        }
    }

    public boolean snapshotDestroy(String snapname) {
        try {
            return service.snapshotDestroy(name, lxcPath, snapname);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to destroy snapshot: " + e.getMessage());
            return false;
        }
    }

    public boolean snapshotDestroyAll() {
        try {
            return service.snapshotDestroyAll(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to destroy all snapshots: " + e.getMessage());
            return false;
        }
    }

    public boolean snapshotRestore(String snapname, String newname) {
        try {
            return service.snapshotRestore(name, lxcPath, snapname, newname);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to restore snapshot: " + e.getMessage());
            return false;
        }
    }

    public boolean snapshotRestore(String snapname) {
        return snapshotRestore(snapname, null);
    }

    public String[] getInterfaces() {
        try {
            return service.getInterfaces(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get interfaces: " + e.getMessage());
            return new String[0];
        }
    }

    public String[] getIps(String interface_, String family, int scope) {
        try {
            return service.getIps(name, lxcPath, interface_, family, scope);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get IPs: " + e.getMessage());
            return new String[0];
        }
    }

    public String[] getIps() {
        return getIps(null, null, 0);
    }

    public boolean attachInterface(String dev, String dstDev) {
        try {
            return service.attachInterface(name, lxcPath, dev, dstDev);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to attach interface: " + e.getMessage());
            return false;
        }
    }

    public boolean detachInterface(String dev, String dstDev) {
        try {
            return service.detachInterface(name, lxcPath, dev, dstDev);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to detach interface: " + e.getMessage());
            return false;
        }
    }

    public boolean addDeviceNode(String srcPath, String destPath) {
        try {
            return service.addDeviceNode(name, lxcPath, srcPath, destPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to add device node: " + e.getMessage());
            return false;
        }
    }

    public boolean removeDeviceNode(String srcPath, String destPath) {
        try {
            return service.removeDeviceNode(name, lxcPath, srcPath, destPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to remove device node: " + e.getMessage());
            return false;
        }
    }

    public int getConsoleFd(int ttynum) {
        try {
            return service.getConsoleFd(name, lxcPath, ttynum);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get console fd: " + e.getMessage());
            return -1;
        }
    }

    public boolean console(int ttynum, int stdinfd, int stdoutfd, int stderrfd, int escape) {
        try {
            return service.console(name, lxcPath, ttynum, stdinfd, stdoutfd, stderrfd, escape);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to console: " + e.getMessage());
            return false;
        }
    }

    public boolean clone(String newname, String newlxcpath, int flags, String bdevtype) {
        try {
            return service.clone(name, lxcPath, newname, newlxcpath, flags, bdevtype);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to clone container: " + e.getMessage());
            return false;
        }
    }

    public boolean create(String t, String bdevtype, int flags, String[] argv) {
        try {
            return service.create(name, lxcPath, t, bdevtype, flags, argv);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to create container: " + e.getMessage());
            return false;
        }
    }

    public boolean rename(String newname) {
        try {
            return service.rename(name, lxcPath, newname);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to rename container: " + e.getMessage());
            return false;
        }
    }

    public boolean checkpoint(String directory, boolean stop, boolean verbose) {
        try {
            return service.checkpoint(name, lxcPath, directory, stop, verbose);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to checkpoint container: " + e.getMessage());
            return false;
        }
    }

    public boolean restore(String directory, boolean verbose) {
        try {
            return service.restore(name, lxcPath, directory, verbose);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to restore container: " + e.getMessage());
            return false;
        }
    }

    public boolean destroyWithSnapshots() {
        try {
            return service.destroyWithSnapshots(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to destroy container with snapshots: " + e.getMessage());
            return false;
        }
    }

    public int attachRunWait(boolean clearEnv, int namespaces, long personality, int uid, int gid, String[] argv, int attachFlags) {
        try {
            return service.attachRunWait(name, lxcPath, clearEnv, namespaces, personality, uid, gid, argv, attachFlags);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to attach and wait: " + e.getMessage());
            return -1;
        }
    }

    public int attachNoWait(boolean clearEnv, int namespaces, long personality, int uid, int gid, String[] argv, int attachFlags) {
        try {
            return service.attachNoWait(name, lxcPath, clearEnv, namespaces, personality, uid, gid, argv, attachFlags);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to attach: " + e.getMessage());
            return -1;
        }
    }

    public int attach(boolean clearEnv, int namespaces, long personality, int uid, int gid, int stdinfd, int stdoutfd, int stderrfd, int attachFlags) {
        try {
            return service.attach(name, lxcPath, clearEnv, namespaces, personality, uid, gid, stdinfd, stdoutfd, stderrfd, attachFlags);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to attach: " + e.getMessage());
            return -1;
        }
    }

    public int initPid() {
        try {
            return service.initPid(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get init pid: " + e.getMessage());
            return -1;
        }
    }

    public int initPidfd() {
        try {
            return service.initPidfd(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get init pidfd: " + e.getMessage());
            return -1;
        }
    }

    public int devptsFd() {
        try {
            return service.devptsFd(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get devpts fd: " + e.getMessage());
            return -1;
        }
    }

    public int seccompNotifyFd() {
        try {
            return service.seccompNotifyFd(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get seccomp notify fd: " + e.getMessage());
            return -1;
        }
    }

    public int seccompNotifyFdActive() {
        try {
            return service.seccompNotifyFdActive(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get seccomp notify fd active: " + e.getMessage());
            return -1;
        }
    }

    public boolean wantDaemonize(boolean state) {
        try {
            return service.wantDaemonize(name, lxcPath, state);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to set want daemonize: " + e.getMessage());
            return false;
        }
    }

    public boolean wantCloseAllFds(boolean state) {
        try {
            return service.wantCloseAllFds(name, lxcPath, state);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to set want close all fds: " + e.getMessage());
            return false;
        }
    }

    public int getErrorNum() {
        try {
            return service.getErrorNum(name, lxcPath);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get error num: " + e.getMessage());
            return -1;
        }
    }

    @NonNull
    @Override
    public String toString() {
        return String.format("LxcContainer{name='%s', path='%s', state='%s'}",
                           name, lxcPath, getState());
    }
}
