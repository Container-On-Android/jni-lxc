package io.github.coap;

import android.annotation.SuppressLint;
import android.content.ComponentName;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import com.topjohnwu.superuser.Shell;
import io.github.coap.databinding.ActivityMainBinding;
import io.github.coap.lxc.LxcNative;
import io.github.coap.lxc.LxcManager;
import io.github.coap.lxc.LxcContainer;
import android.content.ServiceConnection;
import com.topjohnwu.superuser.ipc.RootService;
import java.util.List;

@SuppressLint("SetTextI18n")
public class MainActivity extends AppCompatActivity {
    private ActivityMainBinding binding;

    private AIDLConnection aidlConn;
    private int uid = -1;
    private IService lxcService;
    private LxcManager lxcManager;
    private LxcContainer currentContainer;

    class AIDLConnection implements ServiceConnection {
        
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            lxcService = IService.Stub.asInterface(service);
            try {
                Log.d("AIDL", "UID " + lxcService.getUid());
                uid = lxcService.getUid();
                
                // Initialize LXC Manager
                lxcManager = new LxcManager(lxcService);
                
                runOnUiThread(() -> {
                    binding.getuid.setText("UID: " + uid);
                    binding.version.setText("LXC Version: " + LxcNative.getVersion());
                    binding.serviceStatus.setText("LXC Service: Connected");

                    // Start LXC feature demo
                    demoLxcFeatures();
                });
            } catch(RemoteException err) {
                Log.e("AIDL", "AIDL SERVICE FAILED", err);
                runOnUiThread(() -> binding.serviceStatus.setText("LXC Service: Failed"));
            }
        }
        
        public void onServiceDisconnected(ComponentName name) {
            aidlConn = null;
            lxcService = null;
            lxcManager = null;
            currentContainer = null;
            runOnUiThread(() -> binding.serviceStatus.setText("LXC Service: Disconnected"));
        }
    }
    
    static {
        Shell.enableVerboseLogging = BuildConfig.DEBUG;
        Shell.setDefaultBuilder(Shell.Builder.create()
            .setFlags(Shell.FLAG_MOUNT_MASTER)
            .setTimeout(10));
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Inflate and get instance of binding
        binding = ActivityMainBinding.inflate(getLayoutInflater());

        // set content view to binding's root
        setContentView(binding.getRoot());

        // Initialize UI state
        binding.serviceStatus.setText("LXC Service: Connecting...");
        binding.version.setText("LXC Version: Checking...");
        binding.getuid.setText("UID: Checking...");
        
        // Set up button click listeners
        setupButtonListeners();
        
        // Bind AIDL service
        Intent intent = new Intent(this, LxcNative.class);
        aidlConn = new AIDLConnection();
        RootService.bind(intent, aidlConn);
    }
    
    /**
     * Set up button click event listeners
     */
    private void setupButtonListeners() {
        // Set refresh button click event
        binding.refreshButton.setOnClickListener(v -> {
            if (lxcManager != null && lxcManager.isServiceAvailable()) {
                demoLxcFeatures();
            } else {
                binding.demoOutput.setText("LXC service not available, please wait for connection...");
            }
        });
        
        // Set select container button click event
        binding.selectContainerButton.setOnClickListener(v -> {
            String containerName = binding.containerNameInput.getText().toString().trim();
            if (containerName.isEmpty()) {
                Toast.makeText(this, "Please enter container name", Toast.LENGTH_SHORT).show();
                return;
            }
            
            selectContainer(containerName);
        });
        
        // Set start button click event
        binding.startButton.setOnClickListener(v -> {
            if (currentContainer != null) {
                startContainer();
            } else {
                Toast.makeText(this, "Please select a container first", Toast.LENGTH_SHORT).show();
            }
        });
        
        // Set stop button click event
        binding.stopButton.setOnClickListener(v -> {
            if (currentContainer != null) {
                stopContainer();
            } else {
                Toast.makeText(this, "Please select a container first", Toast.LENGTH_SHORT).show();
            }
        });
        
        // Set freeze button click event
        binding.freezeButton.setOnClickListener(v -> {
            if (currentContainer != null) {
                freezeContainer();
            } else {
                Toast.makeText(this, "Please select a container first", Toast.LENGTH_SHORT).show();
            }
        });
        
        // Set unfreeze button click event
        binding.unfreezeButton.setOnClickListener(v -> {
            if (currentContainer != null) {
                unfreezeContainer();
            } else {
                Toast.makeText(this, "Please select a container first", Toast.LENGTH_SHORT).show();
            }
        });
        
        // Set destroy button click event
        binding.destroyButton.setOnClickListener(v -> {
            if (currentContainer != null) {
                destroyContainer();
            } else {
                Toast.makeText(this, "Please select a container first", Toast.LENGTH_SHORT).show();
            }
        });
        
        // Set snapshot button click event
        binding.snapshotButton.setOnClickListener(v -> {
            if (currentContainer != null) {
                createSnapshot();
            } else {
                Toast.makeText(this, "Please select a container first", Toast.LENGTH_SHORT).show();
            }
        });
        
        // Set attach button click event
        binding.attachButton.setOnClickListener(v -> {
            if (currentContainer != null) {
                attachToContainer();
            } else {
                Toast.makeText(this, "Please select a container first", Toast.LENGTH_SHORT).show();
            }
        });
        
        // Set console button click event
        binding.consoleButton.setOnClickListener(v -> {
            if (currentContainer != null) {
                openConsole();
            } else {
                Toast.makeText(this, "Please select a container first", Toast.LENGTH_SHORT).show();
            }
        });
        
        // Set copy button click event
        binding.copyButton.setOnClickListener(v -> {
            if (currentContainer != null) {
                copyContainer();
            } else {
                Toast.makeText(this, "Please select a container first", Toast.LENGTH_SHORT).show();
            }
        });
    }
    
    /**
     * Select container
     */
    private void selectContainer(String containerName) {
        new Thread(() -> {
            try {
                if (lxcManager == null || !lxcManager.isServiceAvailable()) {
                    runOnUiThread(() -> Toast.makeText(this, "LXC service not available", Toast.LENGTH_SHORT).show());
                    return;
                }
                
                LxcContainer container = lxcManager.getContainer(containerName);
                if (container != null && container.isDefined()) {
                    currentContainer = container;
                    runOnUiThread(() -> {
                        Toast.makeText(this, "Container selected: " + containerName, Toast.LENGTH_SHORT).show();
                        updateContainerInfo();
                    });
                } else {
                    runOnUiThread(() -> Toast.makeText(this, "Container does not exist or is not defined: " + containerName, Toast.LENGTH_SHORT).show());
                }
            } catch (Exception e) {
                Log.e("LXC", "Failed to select container", e);
                runOnUiThread(() -> Toast.makeText(this, "Failed to select container: " + e.getMessage(), Toast.LENGTH_SHORT).show());
            }
        }).start();
    }
    
    /**
     * Update container information display
     */
    private void updateContainerInfo() {
        if (currentContainer == null) return;
        
        StringBuilder info = new StringBuilder();
        info.append("=== Current Container Information ===\n");
        info.append("Name: ").append(currentContainer.getName()).append("\n");
        info.append("State: ").append(currentContainer.getState()).append("\n");
        info.append("Running: ").append(currentContainer.isRunning()).append("\n");
        info.append("Defined: ").append(currentContainer.isDefined()).append("\n");
        
        String[] interfaces = currentContainer.getInterfaces();
        if (interfaces != null && interfaces.length > 0) {
            info.append("Network Interfaces: ").append(String.join(", ", interfaces)).append("\n");
        }
        
        binding.demoOutput.setText(info.toString());
    }
    
    /**
     * Start container
     */
    private void startContainer() {
        new Thread(() -> {
            try {
                boolean success = currentContainer.start();
                runOnUiThread(() -> {
                    if (success) {
                        Toast.makeText(this, "Container started successfully", Toast.LENGTH_SHORT).show();
                        updateContainerInfo();
                    } else {
                        Toast.makeText(this, "Failed to start container", Toast.LENGTH_SHORT).show();
                    }
                });
            } catch (Exception e) {
                Log.e("LXC", "Failed to start container", e);
                runOnUiThread(() -> Toast.makeText(this, "Failed to start container: " + e.getMessage(), Toast.LENGTH_SHORT).show());
            }
        }).start();
    }
    
    /**
     * Stop container
     */
    private void stopContainer() {
        new Thread(() -> {
            try {
                boolean success = currentContainer.stop();
                runOnUiThread(() -> {
                    if (success) {
                        Toast.makeText(this, "Container stopped successfully", Toast.LENGTH_SHORT).show();
                        updateContainerInfo();
                    } else {
                        Toast.makeText(this, "Failed to stop container", Toast.LENGTH_SHORT).show();
                    }
                });
            } catch (Exception e) {
                Log.e("LXC", "Failed to stop container", e);
                runOnUiThread(() -> Toast.makeText(this, "Failed to stop container: " + e.getMessage(), Toast.LENGTH_SHORT).show());
            }
        }).start();
    }
    
    /**
     * Freeze container
     */
    private void freezeContainer() {
        new Thread(() -> {
            try {
                boolean success = currentContainer.freeze();
                runOnUiThread(() -> {
                    if (success) {
                        Toast.makeText(this, "Container frozen successfully", Toast.LENGTH_SHORT).show();
                        updateContainerInfo();
                    } else {
                        Toast.makeText(this, "Failed to freeze container", Toast.LENGTH_SHORT).show();
                    }
                });
            } catch (Exception e) {
                Log.e("LXC", "Failed to freeze container", e);
                runOnUiThread(() -> Toast.makeText(this, "Failed to freeze container: " + e.getMessage(), Toast.LENGTH_SHORT).show());
            }
        }).start();
    }
    
    /**
     * Unfreeze container
     */
    private void unfreezeContainer() {
        new Thread(() -> {
            try {
                boolean success = currentContainer.unfreeze();
                runOnUiThread(() -> {
                    if (success) {
                        Toast.makeText(this, "Container unfrozen successfully", Toast.LENGTH_SHORT).show();
                        updateContainerInfo();
                    } else {
                        Toast.makeText(this, "Failed to unfreeze container", Toast.LENGTH_SHORT).show();
                    }
                });
            } catch (Exception e) {
                Log.e("LXC", "Failed to unfreeze container", e);
                runOnUiThread(() -> Toast.makeText(this, "Failed to unfreeze container: " + e.getMessage(), Toast.LENGTH_SHORT).show());
            }
        }).start();
    }
    
    /**
     * Destroy container
     */
    private void destroyContainer() {
        new Thread(() -> {
            try {
                boolean success = currentContainer.destroy();
                runOnUiThread(() -> {
                    if (success) {
                        Toast.makeText(this, "Container destroyed successfully", Toast.LENGTH_SHORT).show();
                        currentContainer = null;
                        binding.containerNameInput.setText("");
                        binding.demoOutput.setText("Container destroyed");
                    } else {
                        Toast.makeText(this, "Failed to destroy container", Toast.LENGTH_SHORT).show();
                    }
                });
            } catch (Exception e) {
                Log.e("LXC", "Failed to destroy container", e);
                runOnUiThread(() -> Toast.makeText(this, "Failed to destroy container: " + e.getMessage(), Toast.LENGTH_SHORT).show());
            }
        }).start();
    }
    
    /**
     * Create snapshot
     */
    private void createSnapshot() {
        new Thread(() -> {
            try {
                int snapshotId = currentContainer.createSnapshot();
                runOnUiThread(() -> {
                    if (snapshotId >= 0) {
                        Toast.makeText(this, "Snapshot created successfully, ID: " + snapshotId, Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(this, "Failed to create snapshot", Toast.LENGTH_SHORT).show();
                    }
                });
            } catch (Exception e) {
                Log.e("LXC", "Failed to create snapshot", e);
                runOnUiThread(() -> Toast.makeText(this, "Failed to create snapshot: " + e.getMessage(), Toast.LENGTH_SHORT).show());
            }
        }).start();
    }
    
    /**
     * Attach to container
     */
    private void attachToContainer() {
        runOnUiThread(() -> Toast.makeText(this, "Attach functionality not implemented yet", Toast.LENGTH_SHORT).show());
    }
    
    /**
     * Open console
     */
    private void openConsole() {
        runOnUiThread(() -> Toast.makeText(this, "Console functionality not implemented yet", Toast.LENGTH_SHORT).show());
    }
    
    /**
     * Copy container
     */
    private void copyContainer() {
        runOnUiThread(() -> Toast.makeText(this, "Copy functionality not implemented yet", Toast.LENGTH_SHORT).show());
    }
    
    /**
     * Demo LXC features
     */
    private void demoLxcFeatures() {
        new Thread(() -> {
            try {
                if (lxcManager == null || !lxcManager.isServiceAvailable()) {
                    runOnUiThread(() -> binding.demoOutput.setText("LXC service not available"));
                    return;
                }
                
                StringBuilder output = new StringBuilder();
                output.append("=== LXC Feature Demo ===\n\n");
                output.append("LXC Version: ").append(LxcNative.getVersion()).append("\n\n");

                List<LxcContainer> containers = lxcManager.listContainers();
                output.append("Containers found: ").append(containers.size()).append("\n");

                for (LxcContainer container : containers) {
                    output.append("\nContainer: ").append(container.getName()).append("\n");
                    output.append("  State: ").append(container.getState()).append("\n");
                    output.append("  Running: ").append(container.isRunning()).append("\n");
                    output.append("  Defined: ").append(container.isDefined()).append("\n");

                    String[] interfaces = container.getInterfaces();
                    if (interfaces != null && interfaces.length > 0) {
                        output.append("  Interfaces: ").append(String.join(", ", interfaces)).append("\n");
                    }

                    String arch = container.getConfigItem("lxc.arch");
                    if (arch != null) {
                        output.append("  Architecture: ").append(arch).append("\n");
                    }
                }

                List<LxcContainer> runningContainers = lxcManager.listRunningContainers();
                List<LxcContainer> stoppedContainers = lxcManager.listStoppedContainers();

                output.append("\n=== Summary ===\n");
                output.append("Total containers: ").append(containers.size()).append("\n");
                output.append("Running containers: ").append(runningContainers.size()).append("\n");
                output.append("Stopped containers: ").append(stoppedContainers.size()).append("\n");

                final String finalOutput = output.toString();
                runOnUiThread(() -> binding.demoOutput.setText(finalOutput));
                
            } catch (Exception e) {
                Log.e("LXC Demo", "Error in LXC demo", e);
                runOnUiThread(() -> binding.demoOutput.setText("Error: " + e.getMessage()));
            }
        }).start();
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (aidlConn != null) {
            RootService.unbind(aidlConn);
        }
        this.binding = null;
    }
}