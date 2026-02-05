package bishe;

import javax.swing.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.io.IOException;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class NetworkServer implements Runnable{

    private Thread thread;
    private String ThreadName;

    private int port;
    private InetAddress TargetIP;
    private DatagramSocket socket;

    private byte[] buff;

    private JTextArea MainFrameNetworkLog;

    NetworkServer(JTextArea jTextArea) throws UnknownHostException {

        this.ThreadName = "NetworkServer";
        this.thread = new Thread(this, this.ThreadName);

        this.MainFrameNetworkLog = jTextArea;
        this.port = 8080;
        this.TargetIP = InetAddress.getByAddress(new byte[] {(byte) 192, (byte) 168, (byte) 137, (byte) 160});
        this.socket = null;

        this.buff = new byte[1];

        try{
            this.socket = new DatagramSocket(this.port);
//            System.out.println("UDP Server is listening on port:" + this.port);
            this.MainFrameNetworkLog.append("UDP Server is listening on port: %d\r\n".formatted(this.port));
        }catch (IOException e){
            e.printStackTrace();
        }


    }

    public void run(){
        while (true){
            try{
                DatagramPacket packet = new DatagramPacket(this.buff, this.buff.length);
                socket.receive(packet);

                InetAddress ClientAddress = packet.getAddress();
                int clientPort = packet.getPort();

//                String RxData = new String(, 0, packet.getLength());
                byte[] RxDatas = packet.getData();
//                System.out.println("[%s] (%s:%d) >> %s".formatted(this.getTime(), ClientAddress.toString(), clientPort, RxData));
                FrameData.DecodeFrame(RxDatas[0]);
                this.MainFrameNetworkLog.append("[%s] (%s:%d) >> %02X\r\n".formatted(this.getTime(), ClientAddress.toString(), clientPort, RxDatas[0]));

            }catch (IOException e){
                e.printStackTrace();
            }

        }
    }

    public void start(){
        if(this.thread != null){
            this.thread.start();
            System.out.println("UDP服务器启动");
        }
    }

    private String getTime(){
        long currentTimeMillis = System.currentTimeMillis();
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        Date date = new Date(currentTimeMillis);
        return format.format(date);
    }

    public void SetNetworkLog(JTextArea jTextArea) {this.MainFrameNetworkLog = jTextArea;}

    public void SettingTargetIPAddressAndPort(String ip, String port) throws UnknownHostException {
        String patter = "(2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2}";
        Pattern p = Pattern.compile(patter);
        Matcher m = p.matcher(ip);
        byte[] b_ip = new byte[4];
        for (int i = 0;m.find() && i < 4;i++) b_ip[i] = (byte)(Integer.parseInt(m.group(0)));
        this.TargetIP = InetAddress.getByAddress(b_ip);
        this.port = Integer.parseInt(port);
        FrameData.NETWORK_IS_OK = true;
    }

    public void SendByte(byte v) throws IOException {
        byte[] tmp = new byte[1];
        tmp[0] = v;
        DatagramPacket packet = new DatagramPacket(tmp, tmp.length, this.TargetIP, this.port);
        this.socket.send(packet);
    }

    public void SendFrame(byte[] v) throws IOException {
        byte[] tmp = new byte[]{(byte) 0xAA, (byte) 0x55, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x55, (byte) 0xAA};
        tmp[3] = v[1];
        tmp[4] = v[2];
        tmp[5] = (byte)(tmp[2] + tmp[3] + tmp[4]);

        DatagramPacket packet = new DatagramPacket(tmp, tmp.length, this.TargetIP, this.port);
        this.socket.send(packet);
    }

}
