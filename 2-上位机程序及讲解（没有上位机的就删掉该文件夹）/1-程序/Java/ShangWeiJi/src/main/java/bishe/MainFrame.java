package bishe;

import com.fazecast.jSerialComm.SerialPort;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import java.awt.*;
import java.awt.event.*;
import java.io.IOException;

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
    private JButton btn_ControlledDriverStatusToggle8;

    SerialServer serialServer;
    NetworkServer networkServer;

    private static MainFrame instance = null;

    private Thread thread;
    private String ThreadName;

    public static void main(String[] args) {
        JFrame frame = new JFrame("MainFrame");
        frame.setContentPane(new MainFrame().p_Main);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

    public void run() {
        while (true) {
            if (FrameData.NETWORK_IS_OK) {
                try {
                    btn_SensorRead1.doClick();
                    Thread.sleep(3000);
                    btn_SensorRead2.doClick();
                    Thread.sleep(3000);
                    btn_SensorRead3.doClick();
                    Thread.sleep(3000);
                    btn_SensorRead4.doClick();
                    Thread.sleep(3000);
                    btn_SensorRead5.doClick();
                    Thread.sleep(3000);
                    btn_SensorRead6.doClick();
                    Thread.sleep(3000);
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
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
        String[] tmp_SensorNames = new String[6];
        tmp_SensorNames[0] = "温度" + ':';
        tmp_SensorNames[1] = "湿度" + ':';
        tmp_SensorNames[2] = "烟雾" + ':';
        tmp_SensorNames[3] = "可燃气体" + ':';
        tmp_SensorNames[4] = "火焰强度" + ':';
        tmp_SensorNames[5] = "红外感应" + ':';
        //  单位
        String[] tmp_SensorUnit = new String[6];
        tmp_SensorUnit[0] = "℃";
        tmp_SensorUnit[1] = "rh%";
        tmp_SensorUnit[2] = "ppm";
        tmp_SensorUnit[3] = "ppm";
        tmp_SensorUnit[4] = "kW/m";
        tmp_SensorUnit[5] = "";
        // 被控设备
        String[] tmp_ControlledDrivers = new String[7];
        tmp_ControlledDrivers[0] = "排风";
        tmp_ControlledDrivers[1] = "灭火";
        tmp_ControlledDrivers[2] = "除湿";
        tmp_ControlledDrivers[3] = "降温";
        tmp_ControlledDrivers[4] = "窗户";
        tmp_ControlledDrivers[5] = "蜂鸣器";
        tmp_ControlledDrivers[6] = "语音播报";
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
        //

    }

    private void UsefullContronlledDriverButtonStatus(boolean status) {
        btn_ControlledDriverStatusToggle1.setEnabled(status);
        btn_ControlledDriverStatusToggle2.setEnabled(status);
        btn_ControlledDriverStatusToggle3.setEnabled(status);
        btn_ControlledDriverStatusToggle4.setEnabled(status);
        btn_ControlledDriverStatusToggle5.setEnabled(status);
        btn_ControlledDriverStatusToggle6.setEnabled(status);
        btn_ControlledDriverStatusToggle7.setEnabled(status);
    }

    private void MainFrameControlledDriverToggleButtonTextStatus(JButton btn_tmp, byte[] v) {
        if (btn_tmp.getText().equals("关闭")) {
            v[2] = (byte) 0x01;
            btn_tmp.setText("开启");
        } else if (btn_tmp.getText().equals("开启")) {
            v[2] = (byte) 0x00;
            btn_tmp.setText("关闭");
        }
        if (btn_tmp.getText().equals("自控")) {
            v[2] = (byte) 0x01;
            UsefullContronlledDriverButtonStatus(true);
            btn_tmp.setText("手动");
        } else if (btn_tmp.getText().equals("手动")) {
            v[2] = (byte) 0x00;
            UsefullContronlledDriverButtonStatus(false);
            btn_tmp.setText("自控");
        }
        try {
            networkServer.SendFrame(v);
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

        this.l_SensorValue1.setText(String.valueOf((int) (FrameData.SENSOR_3[2])));
        this.l_SensorValue2.setText(String.valueOf((int) (FrameData.SENSOR_4[2])));
        this.l_SensorValue3.setText(String.valueOf((int) (Byte.toUnsignedInt(FrameData.SENSOR_2[2]) / 0.0255)));
        this.l_SensorValue4.setText(String.valueOf((int) (Byte.toUnsignedInt(FrameData.SENSOR_2[2]) / 0.0255)));
        this.l_SensorValue5.setText(String.valueOf((int) (Byte.toUnsignedInt(FrameData.SENSOR_1[2]) / 0.0625)));
        this.l_SensorValue6.setText(String.valueOf((int) (FrameData.SENSOR_5[2])));

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
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorRead3.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SendFrame(FrameData.SENSOR_2);
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorRead4.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SendFrame(FrameData.SENSOR_2);
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorRead5.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    networkServer.SendFrame(FrameData.SENSOR_1);
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
                    if (Integer.parseInt(l_SensorWarnValue1.getText()) <= 60 && Integer.parseInt(l_SensorWarnValue1.getText()) >= 0) {
                        l_SensorWarnValue1.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue1.getText())));
                        FrameData.SET_SENSOR_WARN_3[2] = (byte) (Integer.parseInt(l_SensorWarnValue1.getText()) & 0xFF);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_3);
                    } else l_SensorWarnValue1.setText("30");
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    if (Integer.parseInt(l_SensorWarnValue2.getText()) <= 95 && Integer.parseInt(l_SensorWarnValue2.getText()) >= 0) {
                        l_SensorWarnValue2.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue2.getText())));
                        FrameData.SET_SENSOR_WARN_4[2] = (byte) (Integer.parseInt(l_SensorWarnValue2.getText()) & 0xFF);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_4);
                    } else l_SensorWarnValue2.setText("%d".formatted((int) (95 / 2)));
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting3.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    if (Integer.parseInt(l_SensorWarnValue3.getText()) <= 10000 && Integer.parseInt(l_SensorWarnValue3.getText()) >= 300) {
                        l_SensorWarnValue3.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue3.getText())));
                        FrameData.SET_SENSOR_WARN_2[2] = (byte) (Integer.parseInt(l_SensorWarnValue3.getText()) * 0.0255);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_2);
                    } else l_SensorWarnValue3.setText("%d".formatted((int) (9700 / 2)));
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting4.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    if (Integer.parseInt(l_SensorWarnValue4.getText()) <= 10000 && Integer.parseInt(l_SensorWarnValue4.getText()) >= 300) {
                        l_SensorWarnValue4.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue4.getText())));
                        FrameData.SET_SENSOR_WARN_2[2] = (byte) (Integer.valueOf(l_SensorWarnValue4.getText()) * 0.0255);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_2);
                    } else l_SensorWarnValue4.setText("%d".formatted((int) (9700 / 2)));
                } catch (IOException er) {
                    er.printStackTrace();
                }
            }
        });
        btn_SensorWarnSetting5.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    if (Integer.parseInt(l_SensorWarnValue5.getText()) <= 4095 && Integer.parseInt(l_SensorWarnValue5.getText()) >= 0) {
                        l_SensorWarnValue5.setText("%d".formatted(Integer.parseInt(l_SensorWarnValue5.getText())));
                        FrameData.SET_SENSOR_WARN_1[2] = (byte) (Integer.valueOf(l_SensorWarnValue5.getText()) * 0.0622);
                        networkServer.SendFrame(FrameData.SET_SENSOR_WARN_1);
                    } else l_SensorWarnValue5.setText("%d".formatted((int) (4096 / 2)));
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

        btn_ControlledDriverStatusToggle8.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                MainFrameControlledDriverToggleButtonTextStatus(btn_ControlledDriverStatusToggle8, FrameData.CONTROLLED_DRIVER_8);
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
        p_ControlledDriver.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 14, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_ControlledDriver, new com.intellij.uiDesigner.core.GridConstraints(5, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        p_ControlledDriver.setBorder(BorderFactory.createTitledBorder(null, "控制设备", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, null, null));
        l_ControlledDriverName1 = new JLabel();
        l_ControlledDriverName1.setText("设备1");
        p_ControlledDriver.add(l_ControlledDriverName1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle1 = new JButton();
        btn_ControlledDriverStatusToggle1.setText("关闭");
        p_ControlledDriver.add(btn_ControlledDriverStatusToggle1, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_ControlledDriverName2 = new JLabel();
        l_ControlledDriverName2.setText("设备2");
        p_ControlledDriver.add(l_ControlledDriverName2, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle2 = new JButton();
        btn_ControlledDriverStatusToggle2.setText("关闭");
        p_ControlledDriver.add(btn_ControlledDriverStatusToggle2, new com.intellij.uiDesigner.core.GridConstraints(0, 3, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_ControlledDriverName3 = new JLabel();
        l_ControlledDriverName3.setText("设备3");
        p_ControlledDriver.add(l_ControlledDriverName3, new com.intellij.uiDesigner.core.GridConstraints(0, 4, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle3 = new JButton();
        btn_ControlledDriverStatusToggle3.setText("关闭");
        p_ControlledDriver.add(btn_ControlledDriverStatusToggle3, new com.intellij.uiDesigner.core.GridConstraints(0, 5, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_ControlledDriverName4 = new JLabel();
        l_ControlledDriverName4.setText("设备4");
        p_ControlledDriver.add(l_ControlledDriverName4, new com.intellij.uiDesigner.core.GridConstraints(0, 6, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle4 = new JButton();
        btn_ControlledDriverStatusToggle4.setText("关闭");
        p_ControlledDriver.add(btn_ControlledDriverStatusToggle4, new com.intellij.uiDesigner.core.GridConstraints(0, 7, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_ControlledDriverName5 = new JLabel();
        l_ControlledDriverName5.setText("设备5");
        p_ControlledDriver.add(l_ControlledDriverName5, new com.intellij.uiDesigner.core.GridConstraints(0, 8, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle5 = new JButton();
        btn_ControlledDriverStatusToggle5.setText("关闭");
        p_ControlledDriver.add(btn_ControlledDriverStatusToggle5, new com.intellij.uiDesigner.core.GridConstraints(0, 9, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_ControlledDriverName6 = new JLabel();
        l_ControlledDriverName6.setText("设备6");
        p_ControlledDriver.add(l_ControlledDriverName6, new com.intellij.uiDesigner.core.GridConstraints(0, 10, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle6 = new JButton();
        btn_ControlledDriverStatusToggle6.setText("关闭");
        p_ControlledDriver.add(btn_ControlledDriverStatusToggle6, new com.intellij.uiDesigner.core.GridConstraints(0, 11, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_ControlledDriverName7 = new JLabel();
        l_ControlledDriverName7.setText("设备7");
        p_ControlledDriver.add(l_ControlledDriverName7, new com.intellij.uiDesigner.core.GridConstraints(0, 12, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle7 = new JButton();
        btn_ControlledDriverStatusToggle7.setText("关闭");
        p_ControlledDriver.add(btn_ControlledDriverStatusToggle7, new com.intellij.uiDesigner.core.GridConstraints(0, 13, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        final JLabel label1 = new JLabel();
        label1.setText("运行模式：");
        p_ControlledDriver.add(label1, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_ControlledDriverStatusToggle8 = new JButton();
        btn_ControlledDriverStatusToggle8.setText("自控");
        p_ControlledDriver.add(btn_ControlledDriverStatusToggle8, new com.intellij.uiDesigner.core.GridConstraints(1, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        p_SensorWarnValue = new JPanel();
        p_SensorWarnValue.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 18, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_SensorWarnValue, new com.intellij.uiDesigner.core.GridConstraints(4, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        p_SensorWarnValue.setBorder(BorderFactory.createTitledBorder(null, "传感器报警阈值", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, null, null));
        l_SensorWarnName1 = new JLabel();
        l_SensorWarnName1.setText("传感器1：");
        p_SensorWarnValue.add(l_SensorWarnName1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue1 = new JTextField();
        l_SensorWarnValue1.setText("999");
        p_SensorWarnValue.add(l_SensorWarnValue1, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit1 = new JLabel();
        l_SensorWarnUnit1.setText("Unit");
        p_SensorWarnValue.add(l_SensorWarnUnit1, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnName2 = new JLabel();
        l_SensorWarnName2.setText("传感器2：");
        p_SensorWarnValue.add(l_SensorWarnName2, new com.intellij.uiDesigner.core.GridConstraints(0, 3, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue2 = new JTextField();
        l_SensorWarnValue2.setText("999");
        p_SensorWarnValue.add(l_SensorWarnValue2, new com.intellij.uiDesigner.core.GridConstraints(0, 4, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit2 = new JLabel();
        l_SensorWarnUnit2.setText("Unit");
        p_SensorWarnValue.add(l_SensorWarnUnit2, new com.intellij.uiDesigner.core.GridConstraints(0, 5, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnName3 = new JLabel();
        l_SensorWarnName3.setText("传感器3：");
        p_SensorWarnValue.add(l_SensorWarnName3, new com.intellij.uiDesigner.core.GridConstraints(0, 6, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue3 = new JTextField();
        l_SensorWarnValue3.setText("999");
        p_SensorWarnValue.add(l_SensorWarnValue3, new com.intellij.uiDesigner.core.GridConstraints(0, 7, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit3 = new JLabel();
        l_SensorWarnUnit3.setText("Unit");
        p_SensorWarnValue.add(l_SensorWarnUnit3, new com.intellij.uiDesigner.core.GridConstraints(0, 8, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnName4 = new JLabel();
        l_SensorWarnName4.setText("传感器4：");
        p_SensorWarnValue.add(l_SensorWarnName4, new com.intellij.uiDesigner.core.GridConstraints(0, 9, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue4 = new JTextField();
        l_SensorWarnValue4.setText("999");
        p_SensorWarnValue.add(l_SensorWarnValue4, new com.intellij.uiDesigner.core.GridConstraints(0, 10, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit4 = new JLabel();
        l_SensorWarnUnit4.setText("Unit");
        p_SensorWarnValue.add(l_SensorWarnUnit4, new com.intellij.uiDesigner.core.GridConstraints(0, 11, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnName5 = new JLabel();
        l_SensorWarnName5.setText("传感器5：");
        p_SensorWarnValue.add(l_SensorWarnName5, new com.intellij.uiDesigner.core.GridConstraints(0, 12, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue5 = new JTextField();
        l_SensorWarnValue5.setText("999");
        p_SensorWarnValue.add(l_SensorWarnValue5, new com.intellij.uiDesigner.core.GridConstraints(0, 13, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit5 = new JLabel();
        l_SensorWarnUnit5.setText("Unit");
        p_SensorWarnValue.add(l_SensorWarnUnit5, new com.intellij.uiDesigner.core.GridConstraints(0, 14, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnName6 = new JLabel();
        l_SensorWarnName6.setEnabled(false);
        l_SensorWarnName6.setText("传感器6：");
        p_SensorWarnValue.add(l_SensorWarnName6, new com.intellij.uiDesigner.core.GridConstraints(0, 15, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnValue6 = new JTextField();
        l_SensorWarnValue6.setEnabled(false);
        l_SensorWarnValue6.setText("999");
        p_SensorWarnValue.add(l_SensorWarnValue6, new com.intellij.uiDesigner.core.GridConstraints(0, 16, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorWarnUnit6 = new JLabel();
        l_SensorWarnUnit6.setEnabled(false);
        l_SensorWarnUnit6.setText("Unit");
        p_SensorWarnValue.add(l_SensorWarnUnit6, new com.intellij.uiDesigner.core.GridConstraints(0, 17, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting1 = new JButton();
        btn_SensorWarnSetting1.setText("设置");
        p_SensorWarnValue.add(btn_SensorWarnSetting1, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting2 = new JButton();
        btn_SensorWarnSetting2.setText("设置");
        p_SensorWarnValue.add(btn_SensorWarnSetting2, new com.intellij.uiDesigner.core.GridConstraints(1, 3, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting3 = new JButton();
        btn_SensorWarnSetting3.setText("设置");
        p_SensorWarnValue.add(btn_SensorWarnSetting3, new com.intellij.uiDesigner.core.GridConstraints(1, 6, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting4 = new JButton();
        btn_SensorWarnSetting4.setText("设置");
        p_SensorWarnValue.add(btn_SensorWarnSetting4, new com.intellij.uiDesigner.core.GridConstraints(1, 9, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting5 = new JButton();
        btn_SensorWarnSetting5.setText("设置");
        p_SensorWarnValue.add(btn_SensorWarnSetting5, new com.intellij.uiDesigner.core.GridConstraints(1, 12, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorWarnSetting6 = new JButton();
        btn_SensorWarnSetting6.setEnabled(false);
        btn_SensorWarnSetting6.setText("设置");
        p_SensorWarnValue.add(btn_SensorWarnSetting6, new com.intellij.uiDesigner.core.GridConstraints(1, 15, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        p_SensorValue = new JPanel();
        p_SensorValue.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 18, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_SensorValue, new com.intellij.uiDesigner.core.GridConstraints(3, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        p_SensorValue.setBorder(BorderFactory.createTitledBorder(null, "传感器实时参数", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, null, null));
        l_SensorName1 = new JLabel();
        l_SensorName1.setText("传感器1：");
        p_SensorValue.add(l_SensorName1, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue1 = new JLabel();
        l_SensorValue1.setText("999");
        p_SensorValue.add(l_SensorValue1, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit1 = new JLabel();
        l_SensorUnit1.setText("Unit");
        p_SensorValue.add(l_SensorUnit1, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorName2 = new JLabel();
        l_SensorName2.setText("传感器2：");
        p_SensorValue.add(l_SensorName2, new com.intellij.uiDesigner.core.GridConstraints(0, 3, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue2 = new JLabel();
        l_SensorValue2.setText("999");
        p_SensorValue.add(l_SensorValue2, new com.intellij.uiDesigner.core.GridConstraints(0, 4, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit2 = new JLabel();
        l_SensorUnit2.setText("Unit");
        p_SensorValue.add(l_SensorUnit2, new com.intellij.uiDesigner.core.GridConstraints(0, 5, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorName3 = new JLabel();
        l_SensorName3.setText("传感器3：");
        p_SensorValue.add(l_SensorName3, new com.intellij.uiDesigner.core.GridConstraints(0, 6, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue3 = new JLabel();
        l_SensorValue3.setText("999");
        p_SensorValue.add(l_SensorValue3, new com.intellij.uiDesigner.core.GridConstraints(0, 7, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit3 = new JLabel();
        l_SensorUnit3.setText("Unit");
        p_SensorValue.add(l_SensorUnit3, new com.intellij.uiDesigner.core.GridConstraints(0, 8, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorName4 = new JLabel();
        l_SensorName4.setText("传感器4：");
        p_SensorValue.add(l_SensorName4, new com.intellij.uiDesigner.core.GridConstraints(0, 9, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue4 = new JLabel();
        l_SensorValue4.setText("999");
        p_SensorValue.add(l_SensorValue4, new com.intellij.uiDesigner.core.GridConstraints(0, 10, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit4 = new JLabel();
        l_SensorUnit4.setText("Unit");
        p_SensorValue.add(l_SensorUnit4, new com.intellij.uiDesigner.core.GridConstraints(0, 11, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorName5 = new JLabel();
        l_SensorName5.setText("传感器5：");
        p_SensorValue.add(l_SensorName5, new com.intellij.uiDesigner.core.GridConstraints(0, 12, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue5 = new JLabel();
        l_SensorValue5.setText("999");
        p_SensorValue.add(l_SensorValue5, new com.intellij.uiDesigner.core.GridConstraints(0, 13, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit5 = new JLabel();
        l_SensorUnit5.setText("Unit");
        p_SensorValue.add(l_SensorUnit5, new com.intellij.uiDesigner.core.GridConstraints(0, 14, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorName6 = new JLabel();
        l_SensorName6.setText("传感器6：");
        p_SensorValue.add(l_SensorName6, new com.intellij.uiDesigner.core.GridConstraints(0, 15, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorValue6 = new JLabel();
        l_SensorValue6.setText("999");
        p_SensorValue.add(l_SensorValue6, new com.intellij.uiDesigner.core.GridConstraints(0, 16, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        l_SensorUnit6 = new JLabel();
        l_SensorUnit6.setText("Unit");
        p_SensorValue.add(l_SensorUnit6, new com.intellij.uiDesigner.core.GridConstraints(0, 17, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead1 = new JButton();
        btn_SensorRead1.setText("读取");
        p_SensorValue.add(btn_SensorRead1, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead2 = new JButton();
        btn_SensorRead2.setText("读取");
        p_SensorValue.add(btn_SensorRead2, new com.intellij.uiDesigner.core.GridConstraints(1, 3, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead3 = new JButton();
        btn_SensorRead3.setText("读取");
        p_SensorValue.add(btn_SensorRead3, new com.intellij.uiDesigner.core.GridConstraints(1, 6, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead4 = new JButton();
        btn_SensorRead4.setText("读取");
        p_SensorValue.add(btn_SensorRead4, new com.intellij.uiDesigner.core.GridConstraints(1, 9, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead5 = new JButton();
        btn_SensorRead5.setText("读取");
        p_SensorValue.add(btn_SensorRead5, new com.intellij.uiDesigner.core.GridConstraints(1, 12, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        btn_SensorRead6 = new JButton();
        btn_SensorRead6.setText("读取");
        p_SensorValue.add(btn_SensorRead6, new com.intellij.uiDesigner.core.GridConstraints(1, 15, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        p_Log = new JPanel();
        p_Log.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 2, new Insets(0, 0, 0, 0), -1, -1));
        p_Main.add(p_Log, new com.intellij.uiDesigner.core.GridConstraints(2, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, null, null, null, 0, false));
        l_SerialLog = new JLabel();
        l_SerialLog.setText("串口日志");
        p_Log.add(l_SerialLog, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        sp_SerialLog = new JScrollPane();
        p_Log.add(sp_SerialLog, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, null, null, null, 0, false));
        serialLogTextArea = new JTextArea();
        serialLogTextArea.setEditable(false);
        serialLogTextArea.setMargin(new Insets(10, 10, 10, 10));
        serialLogTextArea.setText("Serial Log");
        sp_SerialLog.setViewportView(serialLogTextArea);
        l_NetworkLog = new JLabel();
        l_NetworkLog.setText("网络日志");
        p_Log.add(l_NetworkLog, new com.intellij.uiDesigner.core.GridConstraints(0, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
        jp_NetworkLog = new JScrollPane();
        p_Log.add(jp_NetworkLog, new com.intellij.uiDesigner.core.GridConstraints(1, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_BOTH, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, null, null, null, 0, false));
        networkLogTextArea = new JTextArea();
        networkLogTextArea.setEditable(false);
        networkLogTextArea.setMargin(new Insets(10, 10, 10, 10));
        networkLogTextArea.setText("Network Log");
        jp_NetworkLog.setViewportView(networkLogTextArea);
        p_SerialConnectSetting = new JPanel();
        p_SerialConnectSetting.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(1, 3, new Insets(0, 0, 0, 0), -1, -1));
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
        p_SerialConnectSetting.add(btn_SerialConnect, new com.intellij.uiDesigner.core.GridConstraints(0, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null, 0, false));
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
