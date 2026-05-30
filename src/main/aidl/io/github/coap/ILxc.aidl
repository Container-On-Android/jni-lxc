package io.github.coap;

interface ILxc {

    int getUid();

    // Container lifecycle
    String[] listContainers(String lxcpath);
    boolean isDefined(String name, String lxcpath);
    boolean isRunning(String name, String lxcpath);
    String getState(String name, String lxcpath);
    boolean startContainer(String name, String lxcpath, boolean useInit);
    boolean stopContainer(String name, String lxcpath);
    boolean freezeContainer(String name, String lxcpath);
    boolean unfreezeContainer(String name, String lxcpath);
    boolean destroyContainer(String name, String lxcpath);
    boolean reboot(String name, String lxcpath);
    boolean shutdown(String name, String lxcpath, int timeout);
    boolean wait(String name, String lxcpath, String state, int timeout);

    // Configuration
    String getConfigItem(String name, String lxcpath, String key);
    boolean setConfigItem(String name, String lxcpath, String key, String value);
    boolean clearConfig(String name, String lxcpath);
    boolean clearConfigItem(String name, String lxcpath, String key);
    String getCgroupItem(String name, String lxcpath, String key);
    boolean setCgroupItem(String name, String lxcpath, String key, String value);
    String getConfigPath(String name, String lxcpath);
    boolean setConfigPath(String name, String lxcpath, String path);
    boolean saveConfig(String name, String lxcpath, String altFile);
    boolean loadConfig(String name, String lxcpath, String altFile);
    String configFileName(String name, String lxcpath);
    String getKeys(String name, String lxcpath, String key);
    boolean mayControl(String name, String lxcpath);
    boolean setTimeout(String name, String lxcpath, int timeout);

    // Snapshots
    int createSnapshot(String name, String lxcpath);
    int snapshotList(String name, String lxcpath);
    boolean snapshotDestroy(String name, String lxcpath, String snapname);
    boolean snapshotDestroyAll(String name, String lxcpath);
    boolean snapshotRestore(String name, String lxcpath, String snapname, String newname);

    // Network
    String[] getInterfaces(String name, String lxcpath);
    String[] getIps(String name, String lxcpath, String interface_, String family, int scope);
    boolean attachInterface(String name, String lxcpath, String dev, String dstDev);
    boolean detachInterface(String name, String lxcpath, String dev, String dstDev);

    // Device nodes
    boolean addDeviceNode(String name, String lxcpath, String srcPath, String destPath);
    boolean removeDeviceNode(String name, String lxcpath, String srcPath, String destPath);

    // Console
    int getConsoleFd(String name, String lxcpath, int ttynum);
    boolean console(String name, String lxcpath, int ttynum, int stdinfd, int stdoutfd, int stderrfd, int escape);

    // Container creation/destruction
    boolean clone(String name, String lxcpath, String newname, String newlxcpath, int flags, String bdevtype);
    boolean create(String name, String lxcpath, String t, String bdevtype, int flags, in String[] argv);
    boolean rename(String name, String lxcpath, String newname);
    boolean checkpoint(String name, String lxcpath, String directory, boolean stop, boolean verbose);
    boolean restore(String name, String lxcpath, String directory, boolean verbose);
    boolean destroyWithSnapshots(String name, String lxcpath);

    // Attach
    int attachRunWait(String name, String lxcpath, boolean clearEnv, int namespaces, long personality, int uid, int gid, in String[] argv, int attachFlags);
    int attachNoWait(String name, String lxcpath, boolean clearEnv, int namespaces, long personality, int uid, int gid, in String[] argv, int attachFlags);
    int attach(String name, String lxcpath, boolean clearEnv, int namespaces, long personality, int uid, int gid, int stdinfd, int stdoutfd, int stderrfd, int attachFlags);

    // Process info
    int initPid(String name, String lxcpath);
    int initPidfd(String name, String lxcpath);
    int devptsFd(String name, String lxcpath);

    // Seccomp
    int seccompNotifyFd(String name, String lxcpath);
    int seccompNotifyFdActive(String name, String lxcpath);

    // Daemonize options
    boolean wantDaemonize(String name, String lxcpath, boolean state);
    boolean wantCloseAllFds(String name, String lxcpath, boolean state);

    // Error handling
    int getErrorNum(String name, String lxcpath);

    // API checks
    boolean configItemIsSupported(String key);
    boolean hasApiExtension(String extension);
}
