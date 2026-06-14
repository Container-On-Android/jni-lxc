package io.github.coap.lxc;

import android.os.RemoteException;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

import io.github.coap.ILxc;

public class LxcManager {
    private static final String TAG = "LxcManager";

    private final ILxc service;
    private final String defaultLxcPath;

    public LxcManager(ILxc service, String defaultLxcPath) {
        this.service = service;
        this.defaultLxcPath = defaultLxcPath;
    }

    public LxcManager(ILxc service) {
        this(service, LxcNative.LXC_PATH);
    }

    public String getVersion() {
        return LxcNative.getVersion();
    }

    public List<LxcContainer> listContainers(String lxcPath) {
        List<LxcContainer> containers = new ArrayList<>();
        try {
            String[] containerNames = service.listContainers(lxcPath);
            if (containerNames != null) {
                for (String name : containerNames) {
                    containers.add(new LxcContainer(name, lxcPath, service));
                }
            }
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to list containers: " + e.getMessage());
        }
        return containers;
    }

    public List<LxcContainer> listContainers() {
        return listContainers(defaultLxcPath);
    }

    public List<LxcContainer> listRunningContainers(String lxcPath) {
        List<LxcContainer> runningContainers = new ArrayList<>();
        List<LxcContainer> allContainers = listContainers(lxcPath);
        for (LxcContainer container : allContainers) {
            if (container.isRunning()) {
                runningContainers.add(container);
            }
        }
        return runningContainers;
    }

    public List<LxcContainer> listRunningContainers() {
        return listRunningContainers(defaultLxcPath);
    }

    public List<LxcContainer> listStoppedContainers(String lxcPath) {
        List<LxcContainer> stoppedContainers = new ArrayList<>();
        List<LxcContainer> allContainers = listContainers(lxcPath);
        for (LxcContainer container : allContainers) {
            if (!container.isRunning()) {
                stoppedContainers.add(container);
            }
        }
        return stoppedContainers;
    }

    public List<LxcContainer> listStoppedContainers() {
        return listStoppedContainers(defaultLxcPath);
    }

    public LxcContainer getContainer(String name, String lxcPath) {
        try {
            if (service.isDefined(name, lxcPath)) {
                return new LxcContainer(name, lxcPath, service);
            }
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to get container: " + e.getMessage());
        }
        return null;
    }

    public LxcContainer getContainer(String name) {
        return getContainer(name, defaultLxcPath);
    }

    public LxcContainer createContainer(String name, String lxcPath, String template, String bdevtype, int flags, String[] args) {
        try {
            if (service.create(name, lxcPath, template, bdevtype, flags, args)) {
                return new LxcContainer(name, lxcPath, service);
            }
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to create container: " + e.getMessage());
        }
        return null;
    }

    public LxcContainer createContainer(String name, String template, String bdevtype, int flags, String[] args) {
        return createContainer(name, defaultLxcPath, template, bdevtype, flags, args);
    }

    public boolean cloneContainer(String name, String lxcPath, String newname, String newlxcpath, int flags, String bdevtype) {
        try {
            return service.clone(name, lxcPath, newname, newlxcpath, flags, bdevtype);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to clone container: " + e.getMessage());
            return false;
        }
    }

    public boolean cloneContainer(String name, String newname, String newlxcpath, int flags, String bdevtype) {
        return cloneContainer(name, defaultLxcPath, newname, newlxcpath, flags, bdevtype);
    }

    public boolean configItemIsSupported(String key) {
        try {
            return service.configItemIsSupported(key);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to check if config item is supported: " + e.getMessage());
            return false;
        }
    }

    public boolean hasApiExtension(String extension) {
        try {
            return service.hasApiExtension(extension);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to check for API extension: " + e.getMessage());
            return false;
        }
    }

    public String getDefaultLxcPath() {
        return defaultLxcPath;
    }

    public boolean isServiceAvailable() {
        String version = getVersion();
        return version != null;
    }

    /**
     * Open a passive monitor on the default lxcpath. The returned
     * {@link LxcMonitor} blocks on {@link LxcMonitor#readNext()} until
     * the next container state-change event is published by lxc-monitord.
     */
    public LxcMonitor openMonitor() {
        return openMonitor(defaultLxcPath);
    }

    /**
     * Open a passive monitor on the given lxcpath.
     *
     * @return a {@link LxcMonitor} instance, or {@code null} on failure.
     */
    public LxcMonitor openMonitor(String lxcpath) {
        try {
            long handle = service.openMonitor(lxcpath);
            if (handle == 0) {
                Log.e(TAG, "Failed to open monitor for " + lxcpath);
                return null;
            }
            return new LxcMonitor(service, handle);
        } catch (RemoteException e) {
            Log.e(TAG, "Failed to open monitor: " + e.getMessage());
            return null;
        }
    }

    @Override
    public String toString() {
        return String.format("LxcManager{defaultPath='%s', version='%s'}",
                defaultLxcPath, getVersion());
    }
}
