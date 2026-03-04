package bishe;

import com.sun.tools.javac.Main;

public class FrameData {

    //  缓冲区
    public static byte NOW_UUID = (byte) 0x00;
    public static int NETWORK_BUFF_INDEX = 0;
    public static boolean NETWORK_IS_OK = false;

    public static int SERIAL_BUFF_INDEX = 0;
    public static byte SERIAL_NOW_UUID = (byte) 0x00;
    public static byte[] SERIAL_NOW_VALUE = new byte[]{(byte) 0x00, (byte) 0x00};

    //  读传感器指令
    public static byte[] SENSOR_1 = new byte[]{(byte) 0x01, (byte) 0x01, (byte) 0x01, (byte) 0x01};  //  DRIVER_FIRE
    public static byte[] SENSOR_2 = new byte[]{(byte) 0x01, (byte) 0x02, (byte) 0x01, (byte) 0x01};  //  DRIVER_SMOKE
    public static byte[] SENSOR_3 = new byte[]{(byte) 0x01, (byte) 0x03, (byte) 0x01, (byte) 0x01};  //  DRIVER_TEMPTRUE
    public static byte[] SENSOR_4 = new byte[]{(byte) 0x01, (byte) 0x04, (byte) 0x01, (byte) 0x01};  //  DRIVER_HUMIDITY
    public static byte[] SENSOR_5 = new byte[]{(byte) 0x01, (byte) 0x05, (byte) 0x01, (byte) 0x01};  //  DRIVER_INFRARED_RANGING
    public static byte[] SENSOR_6 = new byte[]{(byte) 0x01, (byte) 0x06, (byte) 0x01, (byte) 0x01};  //  DRIVER_??
    public static byte[] SENSOR_7 = new byte[]{(byte) 0x01, (byte) 0x07, (byte) 0x01, (byte) 0x01};  //  DRIVER_??
    //  设置传感器阈值指令
    public static byte[] SET_SENSOR_WARN_1 = new byte[]{(byte) 0x01, (byte) 0x11, (byte) 0x01, (byte) 0x01};  //  SET_FIRE_WARN
    public static byte[] SET_SENSOR_WARN_2 = new byte[]{(byte) 0x01, (byte) 0x12, (byte) 0x01, (byte) 0x01};  //  SET_SMOKE_WARN
    public static byte[] SET_SENSOR_WARN_3 = new byte[]{(byte) 0x01, (byte) 0x13, (byte) 0x01, (byte) 0x01};  //  SET_TEMPTRUE_WARN
    public static byte[] SET_SENSOR_WARN_4 = new byte[]{(byte) 0x01, (byte) 0x14, (byte) 0x01, (byte) 0x01};  //  SET_HUMIDITY_WARN
    public static byte[] SET_SENSOR_WARN_5 = new byte[]{(byte) 0x01, (byte) 0x15, (byte) 0x01, (byte) 0x01};  //  SET_INFRARED_RANGING_WARN
    public static byte[] SET_SENSOR_WARN_6 = new byte[]{(byte) 0x01, (byte) 0x16, (byte) 0x01, (byte) 0x01};  //  SET_??_WARN
    public static byte[] SET_SENSOR_WARN_7 = new byte[]{(byte) 0x01, (byte) 0x17, (byte) 0x01, (byte) 0x01};  //  SET_??_WARN
    //  控制设备指令
    public static byte[] CONTROLLED_DRIVER_1 = new byte[]{(byte) 0x01, (byte) 0x21, (byte) 0x01, (byte) 0x01};  //  CTL_FIRE_RELAY
    public static byte[] CONTROLLED_DRIVER_2 = new byte[]{(byte) 0x01, (byte) 0x22, (byte) 0x01, (byte) 0x01};  //  CTL_WIND_RELAY
    public static byte[] CONTROLLED_DRIVER_3 = new byte[]{(byte) 0x01, (byte) 0x23, (byte) 0x01, (byte) 0x01};  //  CTL_WINDO_MOTOR
    public static byte[] CONTROLLED_DRIVER_4 = new byte[]{(byte) 0x01, (byte) 0x24, (byte) 0x01, (byte) 0x01};  //  CTL_COLD_RELAY
    public static byte[] CONTROLLED_DRIVER_5 = new byte[]{(byte) 0x01, (byte) 0x25, (byte) 0x01, (byte) 0x01};  //  CTL_DRY_RELAY
    public static byte[] CONTROLLED_DRIVER_6 = new byte[]{(byte) 0x01, (byte) 0x26, (byte) 0x01, (byte) 0x01};  //  CTL_BUZZER_DRIVER
    public static byte[] CONTROLLED_DRIVER_7 = new byte[]{(byte) 0x01, (byte) 0x27, (byte) 0x01, (byte) 0x01};  //  CTL_AUDIOPLAY_DRIVER
    public static byte[] CONTROLLED_DRIVER_9 = new byte[]{(byte) 0x01, (byte) 0x28, (byte) 0x01, (byte) 0x01};  //  CTL_LED_DRIVER
    //  运行模式
    public static byte[] CONTROLLED_DRIVER_8 = new byte[]{(byte) 0x01, (byte) 0xff, (byte) 0x01, (byte) 0x01};  //  CTL_RUN_MODE

    //传感器变量
    public static int[] SENSOR_VALUE = new int[]{0, 0, 0, 0, 0};

    public static Integer SensorValue_1 = 0;
    public static Integer SensorValue_2 = 0;
    public static Integer SensorValue_3 = 0;
    public static Integer SensorValue_4 = 0;
    public static Integer SensorValue_5 = 0;
    public static Integer SensorValue_6 = 0;
    public static Integer SensorValue_7 = 0;

    public static void DecodeFrame(byte v){
//        System.out.println("[%d] %02X".formatted(NETWORK_BUFF_INDEX, v));
        if(NETWORK_BUFF_INDEX == 0 && v == (byte) 0xAA) NETWORK_BUFF_INDEX++;
        else if(NETWORK_BUFF_INDEX == 1 && v == (byte) 0x55) NETWORK_BUFF_INDEX++;
        else if(NETWORK_BUFF_INDEX == 2 && v == (byte) 0x01) NETWORK_BUFF_INDEX++;
        else if(NETWORK_BUFF_INDEX == 3) {
            NOW_UUID = v;
            NETWORK_BUFF_INDEX++;
        }
        else if(NETWORK_BUFF_INDEX == 4){
            switch (NOW_UUID){
                //  读传感器指令
                case (byte) 0x01:
                    FrameData.SENSOR_1[2] = v;
                    break;
                case (byte) 0x02:
                    FrameData.SENSOR_2[2] = v;
                    break;
                case (byte) 0x03:
                    FrameData.SENSOR_3[2] = v;
                    break;
                case (byte) 0x04:
                    FrameData.SENSOR_4[2] = v;
                    break;
                case (byte) 0x05:
                    FrameData.SENSOR_5[2] = v;
                    break;
                case (byte) 0x06:
                    FrameData.SENSOR_6[2] = v;
                    break;
                case (byte) 0x07:
                    FrameData.SENSOR_7[2] = v;
                    break;
                default:
                    break;
            }
            NETWORK_BUFF_INDEX++;
        }
        else if(NETWORK_BUFF_INDEX == 5) NETWORK_BUFF_INDEX++;
        else if(NETWORK_BUFF_INDEX == 6 && v == (byte) 0x55) NETWORK_BUFF_INDEX++;
        else if(NETWORK_BUFF_INDEX == 7 && v == (byte) 0xAA) NETWORK_BUFF_INDEX++;
        else if(NETWORK_BUFF_INDEX == 8){
            NETWORK_BUFF_INDEX = 0;
            MainFrame.getHandler().UpdateSensorValue();
        }



    }

    public static void SerialDecodeFrame(byte v){
//        System.out.println("[%d] %02X".formatted(NETWORK_BUFF_INDEX, v));
        if(SERIAL_BUFF_INDEX == 0 && v == (byte) 0xAA) SERIAL_BUFF_INDEX++;
        else if(SERIAL_BUFF_INDEX == 1 && v == (byte) 0x55) SERIAL_BUFF_INDEX++;
        else if(SERIAL_BUFF_INDEX == 2){
            SERIAL_NOW_UUID = v;
            SERIAL_BUFF_INDEX++;
        }
        else if(SERIAL_BUFF_INDEX == 3) {
            SERIAL_NOW_VALUE[0] = v;
            SERIAL_BUFF_INDEX++;
        }
        else if(SERIAL_BUFF_INDEX == 4){
            SERIAL_NOW_VALUE[1] = v;
            SERIAL_BUFF_INDEX++;
        }
        else if(SERIAL_BUFF_INDEX == 5) SERIAL_BUFF_INDEX++;
        else if(SERIAL_BUFF_INDEX == 6 && v == (byte) 0x55) SERIAL_BUFF_INDEX++;
        else if(SERIAL_BUFF_INDEX == 7 && v == (byte) 0xAA) SERIAL_BUFF_INDEX++;
        else if(SERIAL_BUFF_INDEX == 8){
            switch (SERIAL_NOW_UUID){
                case (byte) 0x00:
                    MainFrame.getHandler().GetData();
                //  读传感器指令
                case (byte) 0x01:
                    FrameData.SENSOR_1[2] = SERIAL_NOW_VALUE[0];
                    FrameData.SENSOR_1[3] = SERIAL_NOW_VALUE[1];
                    break;
                case (byte) 0x02:
                    FrameData.SENSOR_2[2] = SERIAL_NOW_VALUE[0];
                    FrameData.SENSOR_2[3] = SERIAL_NOW_VALUE[1];
                    break;
                case (byte) 0x03:
                    FrameData.SENSOR_3[2] = SERIAL_NOW_VALUE[0];
                    FrameData.SENSOR_3[3] = SERIAL_NOW_VALUE[1];
                    break;
                case (byte) 0x04:
                    FrameData.SENSOR_4[2] = SERIAL_NOW_VALUE[0];
                    FrameData.SENSOR_4[3] = SERIAL_NOW_VALUE[1];
                    break;
                case (byte) 0x05:
                    FrameData.SENSOR_5[2] = SERIAL_NOW_VALUE[0];
                    FrameData.SENSOR_5[3] = SERIAL_NOW_VALUE[1];
                    break;
                case (byte) 0x06:
                    FrameData.SENSOR_6[2] = SERIAL_NOW_VALUE[0];
                    FrameData.SENSOR_6[3] = SERIAL_NOW_VALUE[1];
                    break;
                case (byte) 0x07:
                    FrameData.SENSOR_7[2] = SERIAL_NOW_VALUE[0];
                    FrameData.SENSOR_7[3] = SERIAL_NOW_VALUE[1];
                    break;
                default:
                    break;
            }
            SERIAL_BUFF_INDEX = 0;
            MainFrame.getHandler().UpdateSensorValue();
        }



    }

}
