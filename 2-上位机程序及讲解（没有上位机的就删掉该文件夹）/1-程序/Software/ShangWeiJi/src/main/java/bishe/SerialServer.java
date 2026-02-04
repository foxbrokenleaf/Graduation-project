package bishe;

import com.fazecast.jSerialComm.*;

import javax.swing.*;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;


public class SerialServer{

    private SerialPort serialPort;

    private JTextArea MainFrameSerialLog;

    SerialServer(JTextArea jTextArea){
        this.MainFrameSerialLog = jTextArea;
    }

    public SerialPort[] getSerialPorts() {return  SerialPort.getCommPorts();}

    public void SerialOpen(){
        if(!this.serialPort.isOpen() && this.serialPort != null){
            this.serialPort.addDataListener(new SerialPortDataListener() {
                @Override
                public int getListeningEvents() {
                    return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
                }

                @Override
                public void serialEvent(SerialPortEvent serialPortEvent) {
                    if(serialPortEvent.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return;

                    byte[] bytes = new byte[serialPort.bytesAvailable()];
                    int numRead = serialPort.readBytes(bytes, bytes.length);

//                    System.out.println("[%s] >> %s".formatted(format.format(date), new String(bytes)));
                    MainFrameSerialLog.append("[%s] >> %s\r\n".formatted(getTime(), new String(bytes)));
                }
            });

            this.serialPort.setBaudRate(115200);
            this.serialPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING | SerialPort.TIMEOUT_WRITE_BLOCKING, 1000, 1000);
            this.serialPort.setFlowControl(SerialPort.FLOW_CONTROL_DISABLED);
            this.serialPort.setNumDataBits(8);
            this.serialPort.setNumStopBits(SerialPort.ONE_STOP_BIT);
            this.serialPort.setParity(SerialPort.NO_PARITY);
            this.serialPort.openPort();
            if(this.serialPort.isOpen()) System.out.println("串口打开成功！");
            else System.out.println("串口打开失败！");
        }
    }

    public void SerialClose(){
        if(this.serialPort != null)
            if(this.serialPort.closePort()) System.out.println("串口关闭成功！");
    }

    public void SerialSelect(String com){
        for(SerialPort port : SerialPort.getCommPorts()){
            if(port.getSystemPortName().equals(com)){
                this.serialPort = port;
                break;
            }
        }
    }

    public void SerialRead(){
        byte[] bytes = new byte[1];
        this.serialPort.readBytes(bytes, 1);
        System.out.println(Arrays.toString(bytes));
    }

    private String getTime(){
        long currentTimeMillis = System.currentTimeMillis();
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        Date date = new Date(currentTimeMillis);
        return format.format(date);
    }

    public void SetSerialLog(JTextArea jTextArea) {this.MainFrameSerialLog = jTextArea;}

}
