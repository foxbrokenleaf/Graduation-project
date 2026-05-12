package org.fbl.homeaquariumairpumpintelligentcontrol;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private static final String TARGET_MAC = "26:03:02:01:C2:75";
    private static final UUID SPP_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private Button btnConnect;
    private TextView bleStatus, pumpWork, pumpFault, tvLog;

    private BluetoothAdapter bluetoothAdapter;
    private BluetoothDevice targetDevice;
    private BluetoothSocket bluetoothSocket;
    private InputStream inputStream;
    private OutputStream outputStream;
    private Thread readThread;
    private boolean isConnected = false;
    private boolean isReading = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btnConnect = findViewById(R.id.btn_connect);
        bleStatus  = findViewById(R.id.ble_status);
        pumpWork   = findViewById(R.id.pump_work);
        pumpFault  = findViewById(R.id.pump_fault);
        tvLog      = findViewById(R.id.tv_log);

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        targetDevice = bluetoothAdapter.getRemoteDevice(TARGET_MAC);

        btnConnect.setOnClickListener(v -> {
            if (!isConnected) {
                checkPermissions();
                connectSPP();
            } else {
                disconnectSPP();
            }
        });
    }

    private void checkPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT}, 1001);
        } else {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, 1001);
        }
    }

    private void connectSPP() {
        if (bluetoothAdapter == null || !bluetoothAdapter.isEnabled()) {
            Toast.makeText(this, "请先开启手机蓝牙", Toast.LENGTH_SHORT).show();
            return;
        }

        bleStatus.setText("正在连接串口...");
        btnConnect.setText("连接中...");

        new Thread(() -> {
            try {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    return;
                }
                bluetoothSocket = targetDevice.createRfcommSocketToServiceRecord(SPP_UUID);
                bluetoothAdapter.cancelDiscovery();
                bluetoothSocket.connect();

                inputStream = bluetoothSocket.getInputStream();
                outputStream = bluetoothSocket.getOutputStream();

                isConnected = true;
                runOnUiThread(() -> {
                    bleStatus.setText("已连接串口 JDY-31-SPP");
                    bleStatus.setTextColor(0xFF388E3C);
                    btnConnect.setText("断开连接");
                    addLog("【串口连接成功】");
                });

                startReadData();

            } catch (IOException e) {
                runOnUiThread(() -> {
                    addLog("【串口连接失败】");
                    bleStatus.setText("连接失败");
                    bleStatus.setTextColor(0xFFD32F2F);
                    btnConnect.setText("一键连接蓝牙");
                });
                closeAll();
            }
        }).start();
    }

    private void startReadData() {
        isReading = true;
        readThread = new Thread(() -> {
            byte[] buffer = new byte[1024];
            int len;
            while (isReading) {
                try {
                    if (inputStream == null) break;
                    len = inputStream.read(buffer);
                    if (len > 0) {
                        String rec = new String(buffer, 0, len).trim();
                        runOnUiThread(() -> {
                            addLog("接收：" + rec);
                            parseBluetoothData(rec);
                        });
                    }
                } catch (IOException e) {
                    break;
                }
            }
        });
        readThread.start();
    }

    private void parseBluetoothData(String data) {
        try {
            if (data.contains("[BT-01]")) {
                if (data.contains("Open")) {
                    pumpWork.setText("开启");
                    pumpWork.setTextColor(0xFF0288D1);
                } else if (data.contains("Close")) {
                    pumpWork.setText("关闭");
                    pumpWork.setTextColor(0xFF757575);
                }
            }

            if (data.contains("[BT-02]")) {
                if (data.contains("Ok")) {
                    pumpFault.setText("正常");
                    pumpFault.setTextColor(0xFF388E3C);
                } else if (data.contains("Error")) {
                    pumpFault.setText("故障");
                    pumpFault.setTextColor(0xFFD32F2F);
                }
            }
        } catch (Exception e) {}
    }

    private void addLog(String text) {
        String time = new SimpleDateFormat("HH:mm:ss", Locale.getDefault()).format(new Date());
        String log = "[" + time + "] " + text + "\n";
        tvLog.append(log);

        // 自动滚动到底部，显示最新内容
        final ScrollView scrollView = (ScrollView) tvLog.getParent();
        scrollView.post(new Runnable() {
            @Override
            public void run() {
                scrollView.scrollTo(0, tvLog.getBottom());
            }
        });
    }

    private void disconnectSPP() {
        isReading = false;
        isConnected = false;
        closeAll();
        runOnUiThread(() -> {
            bleStatus.setText("已断开");
            bleStatus.setTextColor(0xFFD32F2F);
            btnConnect.setText("一键连接蓝牙");
            addLog("【串口已断开】");
        });
    }

    private void closeAll() {
        try {
            if (inputStream != null) inputStream.close();
            if (outputStream != null) outputStream.close();
            if (bluetoothSocket != null) bluetoothSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        inputStream = null;
        outputStream = null;
        bluetoothSocket = null;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        disconnectSPP();
    }
}