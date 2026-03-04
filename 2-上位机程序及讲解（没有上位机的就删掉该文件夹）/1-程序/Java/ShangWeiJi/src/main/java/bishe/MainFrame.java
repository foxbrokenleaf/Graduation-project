package bishe;

import com.fazecast.jSerialComm.SerialPort;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;

public class MainFrame implements Runnable {
    private JComboBox cb_SerialSelect;
    private JButton btn_SerialConnect;
    private JTextField tf_NetworkIPAddress;
    private JTextField tf_NetworkPort;
    private JButton btn_NetworkConnect;
    private JButton btn_ControlledDriverStatusToggle1;
    private JButton btn_ControlledDriverStatusToggle2;
    private JButton btn_ControlledDriverStatusToggle3;
    private JButton btn_ControlledDriverStatusToggle4;
    private JButton btn_ControlledDriverStatusToggle5;
    private JButton btn_ControlledDriverStatusToggle6;
    private JButton btn_ControlledDriverStatusToggle7;
    private JTextArea serialLogTextArea;
    private JTextArea networkLogTextArea;
    private JPanel p_SerialConnectSetting;
    private JPanel p_NetworkConnectSetting;
    private JLabel l_NetworkName;
    private JLabel l_SerialName;
    private JLabel l_SerialLog;
    private JLabel l_NetworkLog;
    private JPanel p_Log;
    private JScrollPane sp_SerialLog;
    private JScrollPane jp_NetworkLog;
    private JPanel p_SensorValue;
    private JLabel l_SensorName1;
    private JLabel l_SensorValue1;
    private JLabel l_SensorUnit1;
    private JLabel l_SensorName2;
    private JLabel l_SensorName3;
    private JLabel l_SensorName4;
    private JLabel l_SensorName5;
    private JLabel l_SensorValue2;
    private JLabel l_SensorValue3;
    private JLabel l_SensorValue4;
    private JLabel l_SensorValue5;
    private JLabel l_SensorValue6;
    private JLabel l_SensorName6;
    private JLabel l_SensorUnit2;
    private JLabel l_SensorUnit3;
    private JLabel l_SensorUnit4;
    private JLabel l_SensorUnit5;
    private JLabel l_SensorUnit6;
    private JButton btn_SensorRead1;
    private JButton btn_SensorRead2;
    private JButton btn_SensorRead3;
    private JButton btn_SensorRead4;
    private JButton btn_SensorRead5;
    private JButton btn_SensorRead6;
    private JPanel p_SensorWarnValue;
    private JPanel p_ControlledDriver;
    private JLabel l_SensorWarnName1;
    private JLabel l_SensorWarnName2;
    private JLabel l_SensorWarnName3;
    private JLabel l_SensorWarnName4;
    private JLabel l_SensorWarnName5;
    private JLabel l_SensorWarnName6;
    private JTextField l_SensorWarnValue1;
    private JTextField l_SensorWarnValue2;
    private JTextField l_SensorWarnValue3;
    private JTextField l_SensorWarnValue4;
    private JTextField l_SensorWarnValue5;
    private JTextField l_SensorWarnValue6;
    private JLabel l_SensorWarnUnit1;
    private JLabel l_SensorWarnUnit2;
    private JLabel l_SensorWarnUnit3;
    private JLabel l_SensorWarnUnit4;
    private JLabel l_SensorWarnUnit5;
    private JLabel l_SensorWarnUnit6;
    private JButton btn_SensorWarnSetting1;
    private JButton btn_SensorWarnSetting2;
    private JButton btn_SensorWarnSetting3;
    private JButton btn_SensorWarnSetting4;
    private JButton btn_SensorWarnSetting5;
    private JButton btn_SensorWarnSetting6;
    private JLabel l_ControlledDriverName1;
    private JLabel l_ControlledDriverName2;
    private JLabel l_ControlledDriverName3;
    private JLabel l_ControlledDriverName4;
    private JLabel l_ControlledDriverName5;
    private JLabel l_ControlledDriverName6;
    private JLabel l_ControlledDriverName7;
    private JPanel p_Main;
    private JButton btn_ControlledMCURunMode;
    public JComboBox cb_SerialBaudRate;
    private JLabel l_SensorName7;
    private JLabel l_SensorValue7;
    private JLabel l_SensorUnit7;
    private JButton btn_SensorRead7;
    private JPanel SensorValuePanel_1;
    private JPanel SensorValuePanel_2;
    private JPanel SensorValuePanel_3;
    private JPanel SensorValuePanel_4;
    private JPanel SensorValuePanel_5;
    private JPanel SensorValuePanel_6;
    private JPanel SensorValuePanel_7;
    private JLabel l_SensorWarnName7;
    private JTextField l_SensorWarnValue7;
    private JLabel l_SensorWarnUnit7;
    private JButton btn_SensorWarnSetting7;
    private JPanel SensorWarnValuePanel_1;
    private JPanel SensorWarnValuePanel_2;
    private JPanel SensorWarnValuePanel_3;
    private JPanel SensorWarnValuePanel_4;
    private JPanel SensorWarnValuePanel_5;
    private JPanel SensorWarnValuePanel_6;
    private JPanel SensorWarnValuePanel_7;
    private JPanel RunMode;
    private JPanel ControlledDriverPanel_1;
    private JPanel ControlledDriverPanel_2;
    private JPanel ControlledDriverPanel_3;
    private JPanel ControlledDriverPanel_4;
    private JPanel ControlledDriverPanel_5;
    private JPanel ControlledDriverPanel_6;
    private JPanel ControlledDriverPanel_7;
    private JPanel p_SerialLog;
    private JPanel p_NetworkLog;
    private JPanel ControlledDriverPanel_8;
    private JLabel l_ControlledDriverName8;
    private JButton btn_ControlledDriverStatusToggle8;

    SerialServer serialServer;
    NetworkServer networkServer;

    private static MainFrame instance = null;

    String[] tmp_SensorNames = new String[7];
    String[] tmp_SensorUnit = new String[7];
    String[] tmp_ControlledDrivers = new String[8];

    private Thread thread;
    private String ThreadName;

    private String FilePath = "HistoryData.csv";
    private String str_TableHead = "时间, 传感器, 数值, 单位";
    private BufferedReader bufferedReader;

    public static void main(String[] args) {
        JFrame frame = new JFrame("MainFrame");
        frame.setContentPane(new MainFrame().p_Main);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

    private String getTime() {
        long currentTimeMillis = System.currentTimeMillis();
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        Date date = new Date(currentTimeMillis);
        return format.format(date);
    }

    public void run() {
        while (true) {
            SaveData2File();
            try {
                Thread.sleep(10000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }

    private void SaveData2File() {

        try {
            bufferedReader = new BufferedReader(new FileReader(FilePath));
        } catch (FileNotFoundException err) {
            try {
                BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(FilePath));
                bufferedWriter.close();
                bufferedReader = new BufferedReader(new FileReader(FilePath));
            } catch (IOException io_err) {
                io_err.printStackTrace();
            }
        } catch (IOException err) {
            err.printStackTrace();
        }
        try {

            String line = "";
            line = bufferedReader.readLine();
            bufferedReader.close();

            if (line != null && line.equals(str_TableHead)) {
//                System.out.println("存在表头！");
                BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(FilePath, true));
                //"Time, Sensor, Value, Unit";
                String str_AppendData = "";
                Integer tmpInt[] = new Integer[]{FrameData.SensorValue_3, FrameData.SensorValue_4, FrameData.SensorValue_7, FrameData.SensorValue_1, FrameData.SensorValue_6};
                for (int i = 0; i < 5; i++) {
                    str_AppendData = "\r\n%s, %s, %d, %s".formatted(this.getTime(), tmp_SensorNames[i], tmpInt[i], tmp_SensorUnit[i]);
                    for (int j = 0; j < str_AppendData.length(); j++) {
                        bufferedWriter.append(str_AppendData.toCharArray()[j]);
                    }
                }

//                System.out.println(str_AppendData);
                bufferedWriter.flush();
                bufferedWriter.close();
            } else {
//                System.out.println("不存在表头，添加表头！");
                BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(FilePath));
                bufferedWriter.write(str_TableHead);
                bufferedWriter.flush();
                bufferedWriter.close();
            }
        } catch (IOException err) {
            err.printStackTrace();
        }
    }

    private void MainFrameSettingComponentName() {
        //  设置有关组件的文本
        //  串口
        String SerialCom = "COM8";
        cb_SerialSelect.setSelectedItem(SerialCom);
        //  网络传输
        String ip = "192.168.137.77";
        String port = "8080";
        tf_NetworkIPAddress.setText(ip);
        tf_NetworkPort.setText(port);
        //  传感器

        tmp_SensorNames[0] = "温度";
        tmp_SensorNames[1] = "湿度";
        tmp_SensorNames[2] = "二氧化碳";
        tmp_SensorNames[3] = "火焰强度";
        tmp_SensorNames[4] = "亮度";
//        tmp_SensorNames[5] = "红外感应";
//        tmp_SensorNames[6] = "亮度";
        //  单位

        tmp_SensorUnit[0] = "℃";
        tmp_SensorUnit[1] = "rh%";
        tmp_SensorUnit[2] = "ppm";
        tmp_SensorUnit[3] = "level";
        tmp_SensorUnit[4] = "lux";
//        tmp_SensorUnit[5] = "";
//        tmp_SensorUnit[6] = "";
        // 被控设备

        tmp_ControlledDrivers[0] = "排风";
        tmp_ControlledDrivers[1] = "灭火";
        tmp_ControlledDrivers[2] = "除湿";
        tmp_ControlledDrivers[3] = "降温";
        tmp_ControlledDrivers[4] = "窗户";
        tmp_ControlledDrivers[5] = "蜂鸣器";
        tmp_ControlledDrivers[6] = "语音播报";
        tmp_ControlledDrivers[7] = "LED";
        //串口布局
        p_SerialConnectSetting.setVisible(true);
        p_SerialLog.setVisible(true);
        //网路布局
        p_NetworkConnectSetting.setVisible(false);
        p_NetworkLog.setVisible(false);
        //  传感器当前值
        l_SensorName1.setText(tmp_SensorNames[0]);
        l_SensorName2.setText(tmp_SensorNames[1]);
        l_SensorName3.setText(tmp_SensorNames[2]);
        l_SensorName4.setText(tmp_SensorNames[3]);
        l_SensorName5.setText(tmp_SensorNames[4]);
        l_SensorName6.setText(tmp_SensorNames[5]);
        //  传感器单位
        l_SensorUnit1.setText(tmp_SensorUnit[0]);
        l_SensorUnit2.setText(tmp_SensorUnit[1]);
        l_SensorUnit3.setText(tmp_SensorUnit[2]);
        l_SensorUnit4.setText(tmp_SensorUnit[3]);
        l_SensorUnit5.setText(tmp_SensorUnit[4]);
        l_SensorUnit6.setText(tmp_SensorUnit[5]);
        //  传感器报警值
        l_SensorWarnName1.setText(tmp_SensorNames[0]);
        l_SensorWarnName2.setText(tmp_SensorNames[1]);
        l_SensorWarnName3.setText(tmp_SensorNames[2]);
        l_SensorWarnName4.setText(tmp_SensorNames[3]);
        l_SensorWarnName5.setText(tmp_SensorNames[4]);
        l_SensorWarnName6.setText(tmp_SensorNames[5]);
        // 传感器阈值单位
        l_SensorWarnUnit1.setText(tmp_SensorUnit[0]);
        l_SensorWarnUnit2.setText(tmp_SensorUnit[1]);
        l_SensorWarnUnit3.setText(tmp_SensorUnit[2]);
        l_SensorWarnUnit4.setText(tmp_SensorUnit[3]);
        l_SensorWarnUnit5.setText(tmp_SensorUnit[4]);
        l_SensorWarnUnit6.setText(tmp_SensorUnit[5]);
        //  被控设备
        l_ControlledDriverName1.setText(tmp_ControlledDrivers[0]);
        l_ControlledDriverName2.setText(tmp_ControlledDrivers[1]);
        l_ControlledDriverName3.setText(tmp_ControlledDrivers[2]);
        l_ControlledDriverName4.setText(tmp_ControlledDrivers[3]);
        l_ControlledDriverName5.setText(tmp_ControlledDrivers[4]);
        l_ControlledDriverName6.setText(tmp_ControlledDrivers[5]);
        l_ControlledDriverName7.setText(tmp_ControlledDrivers[6]);
        l_ControlledDriverName8.setText(tmp_ControlledDrivers[7]);
        //
        //传感器值布局
        SensorValuePanel_1.setVisible(true);
        SensorValuePanel_2.setVisible(true);
        SensorValuePanel_3.setVisible(true);
        SensorValuePanel_4.setVisible(true);
        SensorValuePanel_5.setVisible(true);
        SensorValuePanel_6.setVisible(false);
        SensorValuePanel_7.setVisible(false);
        //阈值布局
        SensorWarnValuePanel_1.setVisible(true);
        SensorWarnValuePanel_2.setVisible(true);
        SensorWarnValuePanel_3.setVisible(true);
        SensorWarnValuePanel_4.setVisible(true);
        SensorWarnValuePanel_5.setVisible(true);
        SensorWarnValuePanel_6.setVisible(false);
        SensorWarnValuePanel_7.setVisible(false);
        //控制布局
        ControlledDriverPanel_1.setVisible(true);
        ControlledDriverPanel_2.setVisible(true);
        ControlledDriverPanel_3.setVisible(true);
        ControlledDriverPanel_4.setVisible(true);
        ControlledDriverPanel_5.setVisible(true);
        ControlledDriverPanel_6.setVisible(true);
        ControlledDriverPanel_7.setVisible(true);
    }

    private void UsefullContronlledDriverButtonStatus(boolean status) {
        btn_ControlledDriverStatusToggle1.setEnabled(status);
        btn_ControlledDriverStatusToggle2.setEnabled(status);
        btn_ControlledDriverStatusToggle3.setEnabled(status);
        btn_ControlledDriverStatusToggle4.setEnabled(status);
        btn_ControlledDriverStatusToggle5.setEnabled(status);
        btn_ControlledDriverStatusToggle6.setEnabled(status);
        btn_ControlledDriverStatusToggle7.setEnabled(status);
        btn_ControlledDriverStatusToggle8.setEnabled(status);
    }

    private void MainFrameControlledDriverToggleButtonTextStatus(JButton btn_tmp, byte[] v) {
        byte[] tmp = new byte[]{(byte) 0x55, v[1], (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x55, (byte) 0xAA};
        if (btn_tmp.getText().equals("关闭")) {
            v[3] = (byte) 0x01;
            tmp[3] = (byte) 0x01;
            btn_tmp.setText("开启");
        } else if (btn_tmp.getText().equals("开启")) {
            v[3] = (byte) 0x00;
            tmp[3] = (byte) 0x00;
            btn_tmp.setText("关闭");
        }
        if (btn_tmp.getText().equals("自控")) {
            v[3] = (byte) 0x01;
            tmp[3] = (byte) 0x01;
            UsefullContronlledDriverButtonStatus(true);
            btn_tmp.setText("手动");
        } else if (btn_tmp.getText().equals("手动")) {
            v[3] = (byte) 0x00;
            tmp[3] = (byte) 0x00;
            UsefullContronlledDriverButtonStatus(false);
            btn_tmp.setText("自控");
        }
        try {
            networkServer.SendFrame(v);
            serialServer.SendFrame(tmp);
            System.out.println();
        } catch (IOException er) {
            er.printStackTrace();
        }
    }

    private void MainFrameSerialPortList() {
        cb_SerialSelect.removeAllItems();
        SerialPort[] serialPorts = serialServer.getSerialPorts();
        for (SerialPort port : serialPorts) {
            cb_SerialSelect.addItem(port.getSystemPortName());
        }
    }

    public static MainFrame getHandler() {
        if (instance == null) {
            instance = new MainFrame();
        }
        return instance;
    }

    public void UpdateSensorValue() {

        FrameData.SensorValue_1 = Byte.toUnsignedInt(FrameData.SENSOR_1[2]) * 256 + Byte.toUnsignedInt(FrameData.SENSOR_1[3]);
        //
        FrameData.SensorValue_3 = (int) FrameData.SENSOR_3[3];
        FrameData.SensorValue_4 = (int) FrameData.SENSOR_4[3];
        //
        FrameData.SensorValue_6 = Byte.toUnsignedInt(FrameData.SENSOR_6[2]) * 256 + Byte.toUnsignedInt(FrameData.SENSOR_6[3]);
        FrameData.SensorValue_7 = Byte.toUnsignedInt(FrameData.SENSOR_7[2]) * 256 + Byte.toUnsignedInt(FrameData.SENSOR_7[3]);

        this.l_SensorValue1.setText(String.valueOf(FrameData.SensorValue_3));
        this.l_SensorValue2.setText(String.valueOf(FrameData.SensorValue_4));
        this.l_SensorValue3.setText(String.valueOf(FrameData.SensorValue_7));
        this.l_SensorValue4.setText(String.valueOf(FrameData.SensorValue_1));
        this.l_SensorValue5.setText(String.valueOf(FrameData.SensorValue_6));

        if (FrameData.SensorValue_7 > Integer.parseInt(l_SensorWarnValue3.getText()))
            JOptionPane.showMessageDialog(null, "二氧化碳浓度过高！", "⚠警告⚠", JOptionPane.WARNING_MESSAGE);
        if (FrameData.SensorValue_1 > Integer.parseInt(l_SensorWarnValue4.getText()))
            JOptionPane.showMessageDialog(null, "火焰异常！", "⚠警告⚠", JOptionPane.WARNING_MESSAGE);

    }

    public void GetData() {
        try {
//            networkServer.SendFrame(FrameData.SENSOR_3);
//            serialServer.SendFrame(FrameData.SENSOR_3);
//            networkServer.SendFrame(FrameData.SENSOR_4);
//            serialServer.SendFrame(FrameData.SENSOR_4);
//            networkServer.SendFrame(FrameData.SENSOR_7);
//            serialServer.SendFrame(FrameData.SENSOR_7);
            networkServer.SendFrame(FrameData.SENSOR_1);
            serialServer.SendFrame(FrameData.SENSOR_1);
//            networkServer.SendFrame(FrameData.SENSOR_6);
//            serialServer.SendFrame(FrameData.SENSOR_6);
        } catch (IOException err) {
            err.printStackTrace();
        }
    }

    public MainFrame() {

        instance = this;

        this.serialServer = new SerialServer(this.serialLogTextArea);
//        this.serialServer.SetSerialLog(serialLogTextArea);
        try {
            this.networkServer = new NetworkServer(this.networkLogTextArea);
            this.networkServer.start();
        } catch (IOException e) {
            e.printStackTrace();
        }

        this.ThreadName = "MainFrameReadSensorValue";
        this.thread = new Thread(this, this.ThreadName);
        this.thread.start();

        this.MainFrameSettingComponentName();
        this.UsefullContronlledDriverButtonStatus(false);


        btn_SerialConnect.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (btn_SerialConnect.getText().equals("打开")) {
                    btn_SerialConnect.setText("关闭");
                    serialServer.SerialSelect(cb_SerialSelect.getSelectedItem().toString());
                    serialServer.SerialOpen();
                } else {
                    btn_SerialConnect.setText("打开");
                    serialServer.SerialClose();
                }


            }
        });
        cb_SerialSelect.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                super.mouseClicked(e);
                if (e.getButton() == MouseEvent.BUTTON1) {
                    MainFrameSerialPortList();
                }
            }
        });
        btn_NetworkConnect.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SettingTargetIPAddressAndPort(tf_NetworkIPAddress.getText(), tf_NetworkPort.getText());
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorRead1.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SendFrame(FrameData.SENSOR_3);
                    serialServer.SendFrame(FrameData.SENSOR_3);
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorRead2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SendFrame(FrameData.SENSOR_4);
                    serialServer.SendFrame(FrameData.SENSOR_4);
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorRead3.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SendFrame(FrameData.SENSOR_7);
                    serialServer.SendFrame(FrameData.SENSOR_7);
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorRead4.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SendFrame(FrameData.SENSOR_1);
                    serialServer.SendFrame(FrameData.SENSOR_1);
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorRead5.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SendFrame(FrameData.SENSOR_6);
                    serialServer.SendFrame(FrameData.SENSOR_6);
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorRead6.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SendFrame(FrameData.SENSOR_5);
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting1.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    if (Integer.parseInt(l_SensorWarnValue1.getText()) <= 50 && Integer.parseInt(l_SensorWarnValue1.getText()) >= 0) {
                        l_SensorWarnValue1.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue1.getText())));
                        FrameData.SET_SENSOR_WARN_3[2] = (byte) (Integer.parseInt(l_SensorWarnValue1.getText()) >> 8);
                        FrameData.SET_SENSOR_WARN_3[3] = (byte) (Integer.parseInt(l_SensorWarnValue1.getText()) & 0xFF);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_3);
                        serialServer.SendFrame(FrameData.SET_SENSOR_WARN_3);
                    } else l_SensorWarnValue1.setText("25");
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    if (Integer.parseInt(l_SensorWarnValue2.getText()) <= 80 && Integer.parseInt(l_SensorWarnValue2.getText()) >= 4) {
                        l_SensorWarnValue2.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue2.getText())));
                        FrameData.SET_SENSOR_WARN_4[2] = (byte) (Integer.parseInt(l_SensorWarnValue2.getText()) >> 8);
                        FrameData.SET_SENSOR_WARN_4[3] = (byte) (Integer.parseInt(l_SensorWarnValue2.getText()) & 0xFF);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_4);
                        serialServer.SendFrame(FrameData.SET_SENSOR_WARN_4);
                    } else l_SensorWarnValue2.setText("%d".formatted((int) (76 / 2)));
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting3.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    if (Integer.parseInt(l_SensorWarnValue3.getText()) <= 5000 && Integer.parseInt(l_SensorWarnValue3.getText()) >= 0) {
                        l_SensorWarnValue3.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue3.getText())));
                        FrameData.SET_SENSOR_WARN_7[2] = (byte) (Integer.parseInt(l_SensorWarnValue3.getText()) >> 8);
                        FrameData.SET_SENSOR_WARN_7[3] = (byte) (Integer.parseInt(l_SensorWarnValue3.getText()) & 0xFF);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_7);
                        serialServer.SendFrame(FrameData.SET_SENSOR_WARN_7);
                    } else l_SensorWarnValue3.setText("%d".formatted((int) (5000 / 2)));
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting4.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    if (Integer.parseInt(l_SensorWarnValue4.getText()) <= 4096 && Integer.parseInt(l_SensorWarnValue4.getText()) >= 0) {
                        l_SensorWarnValue4.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue4.getText())));
                        FrameData.SET_SENSOR_WARN_1[2] = (byte) (Integer.valueOf(l_SensorWarnValue4.getText()) >> 8);
                        FrameData.SET_SENSOR_WARN_1[3] = (byte) (Integer.valueOf(l_SensorWarnValue4.getText()) & 0xFF);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_1);
                        serialServer.SendFrame(FrameData.SET_SENSOR_WARN_1);
                    } else l_SensorWarnValue4.setText("%d".formatted((int) (4096 / 2)));
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting5.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    if (Integer.parseInt(l_SensorWarnValue5.getText()) <= 10001 && Integer.parseInt(l_SensorWarnValue5.getText()) >= 10) {
                        l_SensorWarnValue5.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue5.getText())));
                        FrameData.SET_SENSOR_WARN_6[2] = (byte) (Integer.valueOf(l_SensorWarnValue5.getText()) >> 8);
                        FrameData.SET_SENSOR_WARN_6[3] = (byte) (Integer.valueOf(l_SensorWarnValue5.getText()) & 0xFF);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_6);
                        serialServer.SendFrame(FrameData.SET_SENSOR_WARN_6);
                    } else l_SensorWarnValue5.setText("%d".formatted((int) (10001 / 2)));
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting6.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

            }
        });
        btn_ControlledDriverStatusToggle1.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledDriverStatusToggle1, FrameData.CONTROLLED_DRIVER_2);
            }
        });
        btn_ControlledDriverStatusToggle2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledDriverStatusToggle2, FrameData.CONTROLLED_DRIVER_1);
            }
        });
        btn_ControlledDriverStatusToggle3.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledDriverStatusToggle3, FrameData.CONTROLLED_DRIVER_5);
            }
        });
        btn_ControlledDriverStatusToggle4.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledDriverStatusToggle4, FrameData.CONTROLLED_DRIVER_4);
            }
        });
        btn_ControlledDriverStatusToggle5.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledDriverStatusToggle5, FrameData.CONTROLLED_DRIVER_3);
            }
        });
        btn_ControlledDriverStatusToggle6.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledDriverStatusToggle6, FrameData.CONTROLLED_DRIVER_6);
            }
        });
        btn_ControlledDriverStatusToggle7.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledDriverStatusToggle7, FrameData.CONTROLLED_DRIVER_7);
            }
        });

        btn_ControlledMCURunMode.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledMCURunMode, FrameData.CONTROLLED_DRIVER_8);
            }
        });
        btn_ControlledDriverStatusToggle8.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledDriverStatusToggle8, FrameData.CONTROLLED_DRIVER_9);
            }
        });
    }

    {
// GUI initializer generated by IntelliJ IDEA GUI Designer
// >>> IMPORTANT!! <<<
// DO NOT EDIT OR ADD ANY CODE HERE!
        $$$setupUI$$$();
    }

    /**
     * Method generated by IntelliJ IDEA GUI Designer
     * >>> IMPORTANT!! <<<
     * DO NOT edit this method OR call it in your code!
     *
     * @noinspection ALL
     */
    private void $$$setupUI$$$() {
        p_Main = new JPanel();
        p_Main.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(6, 1, new Insets(10, 10, 10, 10), -1, -1));
        p_ControlledDriver = new JPanel();
        p_ControlledDriver.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(3, 7, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_ControlledDriver, new com.intellij.uiDesigner.core.GridConstraints(5, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        p_ControlledDriver.setBorder(BorderFactory.createTitledBorder(null, "控制设备", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, null, null));
        ControlledDriverPanel_1 = new JPanel();
        ControlledDriverPanel_1.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_ControlledDriver.add(ControlledDriverPanel_1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_ControlledDriverName1 = new JLabel();
        l_ControlledDriverName1.setText("设备1");
        ControlledDriverPanel_1.add(l_ControlledDriverName1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle1 = new JButton();
        btn_ControlledDriverStatusToggle1.setText("关闭");
        ControlledDriverPanel_1.add(btn_ControlledDriverStatusToggle1, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        ControlledDriverPanel_2 = new JPanel();
        ControlledDriverPanel_2.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_ControlledDriver.add(ControlledDriverPanel_2, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_ControlledDriverName2 = new JLabel();
        l_ControlledDriverName2.setText("设备2");
        ControlledDriverPanel_2.add(l_ControlledDriverName2, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle2 = new JButton();
        btn_ControlledDriverStatusToggle2.setText("关闭");
        ControlledDriverPanel_2.add(btn_ControlledDriverStatusToggle2, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        ControlledDriverPanel_3 = new JPanel();
        ControlledDriverPanel_3.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_ControlledDriver.add(ControlledDriverPanel_3, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_ControlledDriverName3 = new JLabel();
        l_ControlledDriverName3.setText("设备3");
        ControlledDriverPanel_3.add(l_ControlledDriverName3, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle3 = new JButton();
        btn_ControlledDriverStatusToggle3.setText("关闭");
        ControlledDriverPanel_3.add(btn_ControlledDriverStatusToggle3, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        ControlledDriverPanel_4 = new JPanel();
        ControlledDriverPanel_4.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_ControlledDriver.add(ControlledDriverPanel_4, new com.intellij.uiDesigner.core.GridConstraints(0, 3, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_ControlledDriverName4 = new JLabel();
        l_ControlledDriverName4.setText("设备4");
        ControlledDriverPanel_4.add(l_ControlledDriverName4, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle4 = new JButton();
        btn_ControlledDriverStatusToggle4.setText("关闭");
        ControlledDriverPanel_4.add(btn_ControlledDriverStatusToggle4, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        ControlledDriverPanel_5 = new JPanel();
        ControlledDriverPanel_5.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_ControlledDriver.add(ControlledDriverPanel_5, new com.intellij.uiDesigner.core.GridConstraints(0, 4, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_ControlledDriverName5 = new JLabel();
        l_ControlledDriverName5.setText("设备5");
        ControlledDriverPanel_5.add(l_ControlledDriverName5, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle5 = new JButton();
        btn_ControlledDriverStatusToggle5.setText("关闭");
        ControlledDriverPanel_5.add(btn_ControlledDriverStatusToggle5, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        ControlledDriverPanel_6 = new JPanel();
        ControlledDriverPanel_6.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_ControlledDriver.add(ControlledDriverPanel_6, new com.intellij.uiDesigner.core.GridConstraints(0, 5, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_ControlledDriverName6 = new JLabel();
        l_ControlledDriverName6.setText("设备6");
        ControlledDriverPanel_6.add(l_ControlledDriverName6, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle6 = new JButton();
        btn_ControlledDriverStatusToggle6.setText("关闭");
        ControlledDriverPanel_6.add(btn_ControlledDriverStatusToggle6, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        ControlledDriverPanel_7 = new JPanel();
        ControlledDriverPanel_7.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_ControlledDriver.add(ControlledDriverPanel_7, new com.intellij.uiDesigner.core.GridConstraints(0, 6, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_ControlledDriverName7 = new JLabel();
        l_ControlledDriverName7.setText("设备7");
        ControlledDriverPanel_7.add(l_ControlledDriverName7, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle7 = new JButton();
        btn_ControlledDriverStatusToggle7.setText("关闭");
        ControlledDriverPanel_7.add(btn_ControlledDriverStatusToggle7, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        RunMode = new JPanel();
        RunMode.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(1, 2, new Insets(0, 0, 0, 0), -1, -1));
        p_ControlledDriver.add(RunMode, new com.intellij.uiDesigner.core.GridConstraints(2, 0, 1, 7, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        final JLabel label1 = new JLabel();
        label1.setText("运行模式：");
        RunMode.add(label1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledMCURunMode = new JButton();
        btn_ControlledMCURunMode.setText("自控");
        RunMode.add(btn_ControlledMCURunMode, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        ControlledDriverPanel_8 = new JPanel();
        ControlledDriverPanel_8.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_ControlledDriver.add(ControlledDriverPanel_8, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_ControlledDriverName8 = new JLabel();
        l_ControlledDriverName8.setText("设备8");
        ControlledDriverPanel_8.add(l_ControlledDriverName8, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle8 = new JButton();
        btn_ControlledDriverStatusToggle8.setText("关闭");
        ControlledDriverPanel_8.add(btn_ControlledDriverStatusToggle8, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        p_SensorWarnValue = new JPanel();
        p_SensorWarnValue.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(1, 7, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_SensorWarnValue, new com.intellij.uiDesigner.core.GridConstraints(4, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        p_SensorWarnValue.setBorder(BorderFactory.createTitledBorder(null, "传感器报警阈值", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, null, null));
        SensorWarnValuePanel_1 = new JPanel();
        SensorWarnValuePanel_1.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorWarnValue.add(SensorWarnValuePanel_1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorWarnName1 = new JLabel();
        l_SensorWarnName1.setText("传感器1：");
        SensorWarnValuePanel_1.add(l_SensorWarnName1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue1 = new JTextField();
        l_SensorWarnValue1.setText("999");
        SensorWarnValuePanel_1.add(l_SensorWarnValue1, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit1 = new JLabel();
        l_SensorWarnUnit1.setText("Unit");
        SensorWarnValuePanel_1.add(l_SensorWarnUnit1, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting1 = new JButton();
        btn_SensorWarnSetting1.setText("设置");
        SensorWarnValuePanel_1.add(btn_SensorWarnSetting1, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorWarnValuePanel_2 = new JPanel();
        SensorWarnValuePanel_2.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorWarnValue.add(SensorWarnValuePanel_2, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorWarnName2 = new JLabel();
        l_SensorWarnName2.setText("传感器2：");
        SensorWarnValuePanel_2.add(l_SensorWarnName2, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue2 = new JTextField();
        l_SensorWarnValue2.setText("999");
        SensorWarnValuePanel_2.add(l_SensorWarnValue2, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit2 = new JLabel();
        l_SensorWarnUnit2.setText("Unit");
        SensorWarnValuePanel_2.add(l_SensorWarnUnit2, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting2 = new JButton();
        btn_SensorWarnSetting2.setText("设置");
        SensorWarnValuePanel_2.add(btn_SensorWarnSetting2, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorWarnValuePanel_3 = new JPanel();
        SensorWarnValuePanel_3.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorWarnValue.add(SensorWarnValuePanel_3, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorWarnName3 = new JLabel();
        l_SensorWarnName3.setText("传感器3：");
        SensorWarnValuePanel_3.add(l_SensorWarnName3, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue3 = new JTextField();
        l_SensorWarnValue3.setText("999");
        SensorWarnValuePanel_3.add(l_SensorWarnValue3, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit3 = new JLabel();
        l_SensorWarnUnit3.setText("Unit");
        SensorWarnValuePanel_3.add(l_SensorWarnUnit3, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting3 = new JButton();
        btn_SensorWarnSetting3.setText("设置");
        SensorWarnValuePanel_3.add(btn_SensorWarnSetting3, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorWarnValuePanel_4 = new JPanel();
        SensorWarnValuePanel_4.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorWarnValue.add(SensorWarnValuePanel_4, new com.intellij.uiDesigner.core.GridConstraints(0, 3, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorWarnName4 = new JLabel();
        l_SensorWarnName4.setText("传感器4：");
        SensorWarnValuePanel_4.add(l_SensorWarnName4, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue4 = new JTextField();
        l_SensorWarnValue4.setText("999");
        SensorWarnValuePanel_4.add(l_SensorWarnValue4, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit4 = new JLabel();
        l_SensorWarnUnit4.setText("Unit");
        SensorWarnValuePanel_4.add(l_SensorWarnUnit4, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting4 = new JButton();
        btn_SensorWarnSetting4.setText("设置");
        SensorWarnValuePanel_4.add(btn_SensorWarnSetting4, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorWarnValuePanel_5 = new JPanel();
        SensorWarnValuePanel_5.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorWarnValue.add(SensorWarnValuePanel_5, new com.intellij.uiDesigner.core.GridConstraints(0, 4, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorWarnName5 = new JLabel();
        l_SensorWarnName5.setText("传感器5：");
        SensorWarnValuePanel_5.add(l_SensorWarnName5, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue5 = new JTextField();
        l_SensorWarnValue5.setText("999");
        SensorWarnValuePanel_5.add(l_SensorWarnValue5, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit5 = new JLabel();
        l_SensorWarnUnit5.setText("Unit");
        SensorWarnValuePanel_5.add(l_SensorWarnUnit5, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting5 = new JButton();
        btn_SensorWarnSetting5.setText("设置");
        SensorWarnValuePanel_5.add(btn_SensorWarnSetting5, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorWarnValuePanel_6 = new JPanel();
        SensorWarnValuePanel_6.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorWarnValue.add(SensorWarnValuePanel_6, new com.intellij.uiDesigner.core.GridConstraints(0, 5, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorWarnName6 = new JLabel();
        l_SensorWarnName6.setEnabled(false);
        l_SensorWarnName6.setText("传感器6：");
        SensorWarnValuePanel_6.add(l_SensorWarnName6, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue6 = new JTextField();
        l_SensorWarnValue6.setEnabled(false);
        l_SensorWarnValue6.setText("999");
        SensorWarnValuePanel_6.add(l_SensorWarnValue6, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit6 = new JLabel();
        l_SensorWarnUnit6.setEnabled(false);
        l_SensorWarnUnit6.setText("Unit");
        SensorWarnValuePanel_6.add(l_SensorWarnUnit6, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting6 = new JButton();
        btn_SensorWarnSetting6.setEnabled(false);
        btn_SensorWarnSetting6.setText("设置");
        SensorWarnValuePanel_6.add(btn_SensorWarnSetting6, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorWarnValuePanel_7 = new JPanel();
        SensorWarnValuePanel_7.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorWarnValue.add(SensorWarnValuePanel_7, new com.intellij.uiDesigner.core.GridConstraints(0, 6, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorWarnName7 = new JLabel();
        l_SensorWarnName7.setEnabled(false);
        l_SensorWarnName7.setText("传感器7：");
        SensorWarnValuePanel_7.add(l_SensorWarnName7, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue7 = new JTextField();
        l_SensorWarnValue7.setEnabled(false);
        l_SensorWarnValue7.setText("999");
        SensorWarnValuePanel_7.add(l_SensorWarnValue7, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit7 = new JLabel();
        l_SensorWarnUnit7.setEnabled(false);
        l_SensorWarnUnit7.setText("Unit");
        SensorWarnValuePanel_7.add(l_SensorWarnUnit7, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting7 = new JButton();
        btn_SensorWarnSetting7.setEnabled(false);
        btn_SensorWarnSetting7.setText("设置");
        SensorWarnValuePanel_7.add(btn_SensorWarnSetting7, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        p_SensorValue = new JPanel();
        p_SensorValue.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(1, 7, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_SensorValue, new com.intellij.uiDesigner.core.GridConstraints(3, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        p_SensorValue.setBorder(BorderFactory.createTitledBorder(null, "传感器实时参数", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, null, null));
        SensorValuePanel_1 = new JPanel();
        SensorValuePanel_1.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorValue.add(SensorValuePanel_1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorName1 = new JLabel();
        l_SensorName1.setText("传感器1：");
        SensorValuePanel_1.add(l_SensorName1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue1 = new JLabel();
        l_SensorValue1.setText("999");
        SensorValuePanel_1.add(l_SensorValue1, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit1 = new JLabel();
        l_SensorUnit1.setText("Unit");
        SensorValuePanel_1.add(l_SensorUnit1, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead1 = new JButton();
        btn_SensorRead1.setText("读取");
        SensorValuePanel_1.add(btn_SensorRead1, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorValuePanel_2 = new JPanel();
        SensorValuePanel_2.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorValue.add(SensorValuePanel_2, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorValue2 = new JLabel();
        l_SensorValue2.setText("999");
        SensorValuePanel_2.add(l_SensorValue2, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit2 = new JLabel();
        l_SensorUnit2.setText("Unit");
        SensorValuePanel_2.add(l_SensorUnit2, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead2 = new JButton();
        btn_SensorRead2.setText("读取");
        SensorValuePanel_2.add(btn_SensorRead2, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorName2 = new JLabel();
        l_SensorName2.setText("传感器2：");
        SensorValuePanel_2.add(l_SensorName2, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorValuePanel_3 = new JPanel();
        SensorValuePanel_3.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorValue.add(SensorValuePanel_3, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorName3 = new JLabel();
        l_SensorName3.setText("传感器3：");
        SensorValuePanel_3.add(l_SensorName3, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue3 = new JLabel();
        l_SensorValue3.setText("999");
        SensorValuePanel_3.add(l_SensorValue3, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit3 = new JLabel();
        l_SensorUnit3.setText("Unit");
        SensorValuePanel_3.add(l_SensorUnit3, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead3 = new JButton();
        btn_SensorRead3.setText("读取");
        SensorValuePanel_3.add(btn_SensorRead3, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorValuePanel_4 = new JPanel();
        SensorValuePanel_4.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorValue.add(SensorValuePanel_4, new com.intellij.uiDesigner.core.GridConstraints(0, 3, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorName4 = new JLabel();
        l_SensorName4.setText("传感器4：");
        SensorValuePanel_4.add(l_SensorName4, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue4 = new JLabel();
        l_SensorValue4.setText("999");
        SensorValuePanel_4.add(l_SensorValue4, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit4 = new JLabel();
        l_SensorUnit4.setText("Unit");
        SensorValuePanel_4.add(l_SensorUnit4, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead4 = new JButton();
        btn_SensorRead4.setText("读取");
        SensorValuePanel_4.add(btn_SensorRead4, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorValuePanel_5 = new JPanel();
        SensorValuePanel_5.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorValue.add(SensorValuePanel_5, new com.intellij.uiDesigner.core.GridConstraints(0, 4, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorName5 = new JLabel();
        l_SensorName5.setText("传感器5：");
        SensorValuePanel_5.add(l_SensorName5, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue5 = new JLabel();
        l_SensorValue5.setText("999");
        SensorValuePanel_5.add(l_SensorValue5, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit5 = new JLabel();
        l_SensorUnit5.setText("Unit");
        SensorValuePanel_5.add(l_SensorUnit5, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead5 = new JButton();
        btn_SensorRead5.setText("读取");
        SensorValuePanel_5.add(btn_SensorRead5, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorValuePanel_6 = new JPanel();
        SensorValuePanel_6.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorValue.add(SensorValuePanel_6, new com.intellij.uiDesigner.core.GridConstraints(0, 5, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorName6 = new JLabel();
        l_SensorName6.setText("传感器6：");
        SensorValuePanel_6.add(l_SensorName6, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue6 = new JLabel();
        l_SensorValue6.setText("999");
        SensorValuePanel_6.add(l_SensorValue6, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit6 = new JLabel();
        l_SensorUnit6.setText("Unit");
        SensorValuePanel_6.add(l_SensorUnit6, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead6 = new JButton();
        btn_SensorRead6.setText("读取");
        SensorValuePanel_6.add(btn_SensorRead6, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        SensorValuePanel_7 = new JPanel();
        SensorValuePanel_7.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        p_SensorValue.add(SensorValuePanel_7, new com.intellij.uiDesigner.core.GridConstraints(0, 6, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SensorName7 = new JLabel();
        l_SensorName7.setText("传感器7：");
        SensorValuePanel_7.add(l_SensorName7, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue7 = new JLabel();
        l_SensorValue7.setText("999");
        SensorValuePanel_7.add(l_SensorValue7, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit7 = new JLabel();
        l_SensorUnit7.setText("Unit");
        SensorValuePanel_7.add(l_SensorUnit7, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead7 = new JButton();
        btn_SensorRead7.setText("读取");
        SensorValuePanel_7.add(btn_SensorRead7, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        p_Log = new JPanel();
        p_Log.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 2, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_Log, new com.intellij.uiDesigner.core.GridConstraints(2, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        p_SerialLog = new JPanel();
        p_SerialLog.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_Log.add(p_SerialLog, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 2, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SerialLog = new JLabel();
        l_SerialLog.setText("串口日志");
        p_SerialLog.add(l_SerialLog, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        sp_SerialLog = new JScrollPane();
        p_SerialLog.add(sp_SerialLog, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, null, null, null, 0, false));
        serialLogTextArea = new JTextArea();
        serialLogTextArea.setEditable(false);
        serialLogTextArea.setMargin(new Insets(10, 10, 10, 10));
        serialLogTextArea.setText("Serial Log");
        sp_SerialLog.setViewportView(serialLogTextArea);
        p_NetworkLog = new JPanel();
        p_NetworkLog.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 1, new Insets(0, 0, 0, 0), -1, -1));
        p_Log.add(p_NetworkLog, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 2, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_NetworkLog = new JLabel();
        l_NetworkLog.setText("网络日志");
        p_NetworkLog.add(l_NetworkLog, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        jp_NetworkLog = new JScrollPane();
        p_NetworkLog.add(jp_NetworkLog, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, null, null, null, 0, false));
        networkLogTextArea = new JTextArea();
        networkLogTextArea.setEditable(false);
        networkLogTextArea.setMargin(new Insets(10, 10, 10, 10));
        networkLogTextArea.setText("Network Log");
        jp_NetworkLog.setViewportView(networkLogTextArea);
        p_SerialConnectSetting = new JPanel();
        p_SerialConnectSetting.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(1, 4, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_SerialConnectSetting, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SerialName = new JLabel();
        l_SerialName.setText("串口：");
        p_SerialConnectSetting.add(l_SerialName, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        cb_SerialSelect = new JComboBox();
        cb_SerialSelect.setEditable(false);
        cb_SerialSelect.setEnabled(true);
        final DefaultComboBoxModel defaultComboBoxModel1 = new DefaultComboBoxModel();
        defaultComboBoxModel1.addElement("COM8");
        cb_SerialSelect.setModel(defaultComboBoxModel1);
        p_SerialConnectSetting.add(cb_SerialSelect, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SerialConnect = new JButton();
        btn_SerialConnect.setText("打开");
        p_SerialConnectSetting.add(btn_SerialConnect, new com.intellij.uiDesigner.core.GridConstraints(0, 3, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        cb_SerialBaudRate = new JComboBox();
        final DefaultComboBoxModel defaultComboBoxModel2 = new DefaultComboBoxModel();
        defaultComboBoxModel2.addElement("9600");
        defaultComboBoxModel2.addElement("115200");
        cb_SerialBaudRate.setModel(defaultComboBoxModel2);
        p_SerialConnectSetting.add(cb_SerialBaudRate, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        p_NetworkConnectSetting = new JPanel();
        p_NetworkConnectSetting.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(1, 4, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_NetworkConnectSetting, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_NetworkName = new JLabel();
        l_NetworkName.setText("Wi-Fi：");
        p_NetworkConnectSetting.add(l_NetworkName, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        tf_NetworkIPAddress = new JTextField();
        tf_NetworkIPAddress.setText("192.168.137.77");
        p_NetworkConnectSetting.add(tf_NetworkIPAddress, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        tf_NetworkPort = new JTextField();
        tf_NetworkPort.setText("8080");
        p_NetworkConnectSetting.add(tf_NetworkPort, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, new Dimension(150, -1), null, 0, false));
        btn_NetworkConnect = new JButton();
        btn_NetworkConnect.setText("设置");
        p_NetworkConnectSetting.add(btn_NetworkConnect, new com.intellij.uiDesigner.core.GridConstraints(0, 3, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
    }

    /**
     * @noinspection ALL
     */
    public JComponent $$$getRootComponent$$$() {
        return p_Main;
    }

}
