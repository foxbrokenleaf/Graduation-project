import tkinter as tk
from tkinter import ttk, messagebox, StringVar, IntVar
from datetime import datetime, timedelta
import threading
import time
from collections import deque
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib.dates as mdates
import serial
import serial.tools.list_ports
import sys

class SensorMonitorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("智能传感器监控系统 - STM32")
        
        # 设置窗口初始大小
        self.root.geometry("1000x700")
        
        # 串口相关变量
        self.serial_port = None
        self.serial_thread = None
        self.running = True
        self.receive_buffer = bytearray()
        self.last_receive_time = time.time()
        
        # 串口配置
        self.port_var = StringVar()
        self.baudrate_var = IntVar(value=115200)
        self.connected = False
        
        # 传感器数据
        self.sensor_data = {
            'co': {'current': 0.0, 'history': deque(maxlen=20), 'threshold': 50, 'label': '一氧化碳', 'unit': 'ppm'},
            'fire': {'current': 0.0, 'history': deque(maxlen=20), 'threshold': 30, 'label': '可燃气体', 'unit': '%LEL'},
            'air': {'current': 0.0, 'history': deque(maxlen=20), 'threshold': 100, 'label': '空气质量', 'unit': 'AQI'},
            'temp': {'current': 0.0, 'history': deque(maxlen=20), 'threshold': 35, 'label': '温度', 'unit': '°C'}
        }
        
        # 传感器ID映射
        self.sensor_ids = {
            0x01: 'co',      # 一氧化碳
            0x02: 'air',     # 空气质量
            0x03: 'temp',    # 温度
            0x08: 'fire'     # 可燃气体
        }
        
        # 时间数据
        self.time_data = deque(maxlen=20)
        self.init_history_data()
        
        # 设置中文字体
        self.setup_chinese_font()
        
        # 创建GUI
        self.create_widgets()
        
        # 扫描可用串口
        self.scan_ports()
        
        # 初始更新
        self.update_display()
        
        # 添加日志输出
        self.debug_print("程序启动完成")
    
    def debug_print(self, message):
        """调试输出，确保在终端显示"""
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        print(f"[{timestamp}] {message}")
        sys.stdout.flush()
    
    def setup_chinese_font(self):
        """设置中文字体支持"""
        try:
            plt.rcParams['axes.unicode_minus'] = False
            plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'SimSun']
        except:
            pass
    
    def init_history_data(self):
        """初始化历史数据"""
        now = datetime.now()
        for i in range(20):
            self.time_data.append(now - timedelta(seconds=(19-i)))
            for key in self.sensor_data.keys():
                self.sensor_data[key]['history'].append(0)
    
    def scan_ports(self):
        """扫描可用串口"""
        try:
            ports = serial.tools.list_ports.comports()
            port_list = [port.device for port in ports]
            self.port_combobox['values'] = port_list
            if port_list:
                self.port_var.set(port_list[0])
            self.debug_print(f"扫描到串口: {port_list}")
        except Exception as e:
            self.debug_print(f"扫描串口失败: {e}")
    
    def create_widgets(self):
        """创建所有界面组件"""
        # 创建主容器
        self.main_container = ttk.Frame(self.root)
        self.main_container.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # 配置主容器的权重
        self.main_container.columnconfigure(0, weight=1)
        self.main_container.rowconfigure(0, weight=1)   # 串口控制
        self.main_container.rowconfigure(1, weight=3)   # 图表区域
        self.main_container.rowconfigure(2, weight=1)   # 传感器显示
        self.main_container.rowconfigure(3, weight=1)   # 阈值设置
        
        # 创建四个主要区域
        self.create_serial_control()
        self.create_chart_area()
        self.create_sensor_display()
        self.create_threshold_area()
    
    def create_serial_control(self):
        """创建串口控制区域"""
        serial_frame = ttk.LabelFrame(self.main_container, text="串口设置", padding="10")
        serial_frame.grid(row=0, column=0, sticky='ew', padx=5, pady=5)
        
        # 串口选择
        ttk.Label(serial_frame, text="串口:").grid(row=0, column=0, padx=5, pady=5, sticky='w')
        self.port_combobox = ttk.Combobox(serial_frame, textvariable=self.port_var, width=15)
        self.port_combobox.grid(row=0, column=1, padx=5, pady=5)
        
        # 波特率选择
        ttk.Label(serial_frame, text="波特率:").grid(row=0, column=2, padx=5, pady=5, sticky='w')
        baudrate_combo = ttk.Combobox(serial_frame, textvariable=self.baudrate_var, width=10)
        baudrate_combo['values'] = (9600, 19200, 38400, 57600, 115200)
        baudrate_combo.grid(row=0, column=3, padx=5, pady=5)
        
        # 按钮
        self.connect_button = ttk.Button(serial_frame, text="打开串口", command=self.toggle_serial)
        self.connect_button.grid(row=0, column=4, padx=20, pady=5)
        
        ttk.Button(serial_frame, text="扫描串口", command=self.scan_ports).grid(row=0, column=5, padx=5, pady=5)
        
        # 状态显示
        self.status_label = ttk.Label(serial_frame, text="状态: 未连接", foreground="red")
        self.status_label.grid(row=0, column=6, padx=20, pady=5)
        
        # 数据帧显示
        self.frame_display = ttk.Label(serial_frame, text="接收数据: 无", foreground="blue", 
                                       font=('Consolas', 9))
        self.frame_display.grid(row=1, column=0, columnspan=7, padx=5, pady=5, sticky='w')
    
    def create_chart_area(self):
        """创建图表区域"""
        self.chart_container = ttk.LabelFrame(self.main_container, text="传感器历史数据", padding="5")
        self.chart_container.grid(row=1, column=0, sticky='nsew', padx=5, pady=5)
        
        # 创建图表框架
        chart_frame = ttk.Frame(self.chart_container)
        chart_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # 创建图表
        self.fig = Figure(figsize=(8, 5), dpi=80)
        self.fig.subplots_adjust(hspace=0.4, wspace=0.3)
        
        self.axes = []
        self.lines = []
        titles = ['一氧化碳 (ppm)', '可燃气体 (%LEL)', '空气质量 (AQI)', '温度 (°C)']
        
        for i in range(4):
            ax = self.fig.add_subplot(2, 2, i+1)
            line, = ax.plot([], [], linewidth=1.5)
            
            # 设置图表样式
            ax.set_title(titles[i], fontsize=9)
            ax.grid(True, alpha=0.3)
            ax.tick_params(axis='both', labelsize=7)
            
            # 格式化x轴时间显示
            ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))
            plt.setp(ax.xaxis.get_majorticklabels(), rotation=30, fontsize=6)
            
            self.axes.append(ax)
            self.lines.append(line)
        
        # 嵌入图表到Tkinter
        self.canvas = FigureCanvasTkAgg(self.fig, master=chart_frame)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
    
    def create_sensor_display(self):
        """创建传感器数据显示区域"""
        self.sensor_container = ttk.LabelFrame(self.main_container, text="当前传感器数值", padding="5")
        self.sensor_container.grid(row=2, column=0, sticky='nsew', padx=5, pady=5)
        
        # 创建4列网格
        for i in range(4):
            self.sensor_container.columnconfigure(i, weight=1)
        
        # 创建4个传感器显示框
        self.sensor_frames = []
        self.value_labels = []
        self.status_labels = []
        
        sensor_keys = ['co', 'fire', 'air', 'temp']
        
        for i, key in enumerate(sensor_keys):
            frame = tk.Frame(self.sensor_container, relief='ridge', borderwidth=2, bg='white')
            frame.grid(row=0, column=i, padx=5, pady=5, sticky='nsew')
            
            # 传感器名称
            name_label = tk.Label(frame, text=self.sensor_data[key]['label'], 
                                 font=('Microsoft YaHei', 10, 'bold'), bg='white')
            name_label.pack(pady=(10, 5))
            
            # 当前值显示
            value_text = tk.StringVar(value="0.0")
            self.value_labels.append((key, value_text))
            
            value_frame = tk.Frame(frame, bg='white')
            value_frame.pack(pady=5)
            
            value_label = tk.Label(value_frame, textvariable=value_text, 
                                  font=('Arial', 14, 'bold'), fg='blue', bg='white')
            value_label.pack(side=tk.LEFT)
            
            # 单位
            unit_label = tk.Label(value_frame, text=self.sensor_data[key]['unit'],
                                 font=('Arial', 10), bg='white')
            unit_label.pack(side=tk.LEFT, padx=(5, 0))
            
            # 状态指示
            status_text = tk.StringVar(value="正常")
            self.status_labels.append((key, status_text))
            status_label = tk.Label(frame, textvariable=status_text,
                                   font=('Microsoft YaHei', 10, 'bold'), fg='green', bg='white')
            status_label.pack(pady=(5, 10))
            
            self.sensor_frames.append(frame)
    
    def create_threshold_area(self):
        """创建阈值设置区域"""
        self.threshold_container = ttk.LabelFrame(self.main_container, text="阈值设置", padding="10")
        self.threshold_container.grid(row=3, column=0, sticky='nsew', padx=5, pady=5)
        
        # 创建四个阈值设置行
        self.threshold_vars = {}
        
        threshold_configs = [
            ('co', '一氧化碳阈值:', 50, 'ppm', '0-100 ppm'),
            ('fire', '可燃气体阈值:', 30, '%LEL', '0-50 %LEL'),
            ('air', '空气质量阈值:', 100, 'AQI', '0-150 AQI')
        ]
        
        for i, (key, label_text, default_value, unit, range_text) in enumerate(threshold_configs):
            row_frame = ttk.Frame(self.threshold_container)
            row_frame.pack(fill=tk.X, pady=5)
            
            # 左侧：标签和范围提示
            label_frame = ttk.Frame(row_frame)
            label_frame.pack(side=tk.LEFT, fill=tk.X, expand=True)
            
            ttk.Label(label_frame, text=label_text, 
                     font=('Microsoft YaHei', 10), anchor='w').pack(anchor='w')
            ttk.Label(label_frame, text=range_text,
                     font=('Microsoft YaHei', 8), foreground='gray', anchor='w').pack(anchor='w')
            
            # 中间：输入框
            input_frame = ttk.Frame(row_frame)
            input_frame.pack(side=tk.LEFT, padx=20)
            
            var = tk.StringVar(value=str(default_value))
            self.threshold_vars[key] = var
            
            entry_frame = ttk.Frame(input_frame)
            entry_frame.pack()
            
            entry = ttk.Entry(entry_frame, textvariable=var, width=10,
                            font=('Arial', 10))
            entry.pack(side=tk.LEFT)
            ttk.Label(entry_frame, text=f" {unit}", font=('Arial', 9)).pack(side=tk.LEFT)
            
            # 右侧：当前阈值显示
            current_frame = ttk.Frame(row_frame)
            current_frame.pack(side=tk.LEFT)
            
            current_label = ttk.Label(current_frame, 
                                     text=f"当前: {default_value} {unit}",
                                     font=('Microsoft YaHei', 10))
            current_label.pack()
            self.sensor_data[key]['threshold_label'] = current_label
        
        # 按钮行
        self.button_frame = ttk.Frame(self.threshold_container)
        self.button_frame.pack(fill=tk.X, pady=(10, 5))
        
        # 创建按钮
        ttk.Button(self.button_frame, text="设置阈值", 
                  command=self.set_thresholds).pack(side=tk.LEFT, expand=True, padx=5)
        
        ttk.Button(self.button_frame, text="恢复默认值", 
                  command=self.reset_thresholds).pack(side=tk.LEFT, expand=True, padx=5)
        
        ttk.Button(self.button_frame, text="查看帮助", 
                  command=self.show_help).pack(side=tk.LEFT, expand=True, padx=5)
        
        # 测试按钮
        ttk.Button(self.button_frame, text="测试", 
                  command=self.send_test_data).pack(side=tk.LEFT, expand=True, padx=5)
        
        # 调试按钮
        ttk.Button(self.button_frame, text="调试输出", 
                  command=self.debug_output).pack(side=tk.LEFT, expand=True, padx=5)
    
    def toggle_serial(self):
        """打开/关闭串口"""
        if not self.connected:
            self.open_serial()
        else:
            self.close_serial()
    
    def open_serial(self):
        """打开串口"""
        try:
            port = self.port_var.get()
            baudrate = self.baudrate_var.get()
            
            if not port:
                messagebox.showerror("错误", "请选择串口")
                return
            
            self.debug_print(f"尝试打开串口: {port}, 波特率: {baudrate}")
            
            self.serial_port = serial.Serial(
                port=port,
                baudrate=baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=0.5
            )
            
            # 设置较小的读取超时
            self.serial_port.timeout = 0.1
            
            self.connected = True
            self.connect_button.config(text="关闭串口")
            self.status_label.config(text=f"状态: 已连接 {port}", foreground="green")
            
            # 清空串口缓冲区
            self.serial_port.reset_input_buffer()
            
            # 启动串口读取线程
            self.serial_thread = threading.Thread(target=self.read_serial_data, daemon=True)
            self.serial_thread.start()
            
            self.debug_print(f"串口 {port} 已打开，启动读取线程")
            messagebox.showinfo("成功", f"串口 {port} 已打开")
            
        except Exception as e:
            self.debug_print(f"打开串口失败: {str(e)}")
            messagebox.showerror("错误", f"打开串口失败: {str(e)}")
    
    def close_serial(self):
        """关闭串口"""
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
            self.debug_print("串口已关闭")
        
        self.connected = False
        self.connect_button.config(text="打开串口")
        self.status_label.config(text="状态: 未连接", foreground="red")
        self.frame_display.config(text="接收数据: 无")
    
    def read_serial_data(self):
        """读取串口数据"""
        self.debug_print("串口读取线程启动")
        
        while self.running and self.connected:
            try:
                if self.serial_port and self.serial_port.is_open:
                    # 读取所有可用数据
                    data = self.serial_port.read(self.serial_port.in_waiting or 1)
                    
                    if data:
                        # 更新最后接收时间
                        self.last_receive_time = time.time()
                        
                        # 添加到缓冲区
                        self.receive_buffer.extend(data)
                        
                        # 显示接收到的数据
                        hex_str = ' '.join([f'{b:02X}' for b in data])
                        short_str = hex_str[:60] + ('...' if len(hex_str) > 60 else '')
                        self.root.after(0, lambda s=short_str: self.frame_display.config(
                            text=f"收到: {s}"))
                
                # 定期解析数据
                if len(self.receive_buffer) >= 20:
                    self.parse_data_frames_simple()
                elif len(self.receive_buffer) > 0:
                    current_time = time.time()
                    if current_time - self.last_receive_time > 0.2:
                        self.parse_data_frames_simple()
                
                time.sleep(0.01)
                
            except Exception as e:
                self.debug_print(f"读取串口数据出错: {e}")
                time.sleep(0.1)
        
        self.debug_print("串口读取线程结束")
    
    def parse_data_frames_simple(self):
        """简化版的解析逻辑 - 直接查找完整帧"""
        if len(self.receive_buffer) < 7:
            return
        
        self.debug_print(f"开始解析，缓冲区大小: {len(self.receive_buffer)} 字节")
        
        # 显示缓冲区内容
        hex_buffer = ' '.join([f'{b:02X}' for b in self.receive_buffer[:100]])
        if len(self.receive_buffer) > 100:
            hex_buffer += f" ... (总共{len(self.receive_buffer)}字节)"
        self.debug_print(f"缓冲区内容: {hex_buffer}")
        
        frames_parsed = 0
        i = 0
        
        while i < len(self.receive_buffer):
            # 先检查温度帧 - 根据实际数据，温度帧是8字节：AA 55 01 03 数据 校验和 55 AA
            if i + 8 <= len(self.receive_buffer):
                # 温度帧格式: AA 55 01 03 数据 校验和 55 AA (8字节)
                if (self.receive_buffer[i] == 0xAA and 
                    self.receive_buffer[i+1] == 0x55 and
                    self.receive_buffer[i+2] == 0x01 and
                    self.receive_buffer[i+3] == 0x03 and
                    self.receive_buffer[i+6] == 0x55 and  # 位置6是55
                    self.receive_buffer[i+7] == 0xAA):    # 位置7是AA
                    
                    # 提取温度数据
                    raw_value = self.receive_buffer[i+4]
                    hex_frame = ' '.join([f'{b:02X}' for b in self.receive_buffer[i:i+8]])
                    self.debug_print(f"✓ 找到温度帧 (位置{i}): {hex_frame}")
                    self.debug_print(f"  温度值: {raw_value}°C")
                    self.debug_print(f"  校验和: 0x{self.receive_buffer[i+5]:02X}")
                    
                    self.process_sensor_data(0x03, raw_value)
                    frames_parsed += 1
                    self.root.after(0, lambda f=hex_frame: self.frame_display.config(
                        text=f"解析温度: {f}"))
                    
                    i += 8
                    continue
            
            # 检查其他传感器帧 (9字节)
            if i + 9 <= len(self.receive_buffer):
                # 传感器帧格式: AA 55 02 传感器ID 数据高 数据低 校验和 55 AA
                if (self.receive_buffer[i] == 0xAA and 
                    self.receive_buffer[i+1] == 0x55 and
                    self.receive_buffer[i+2] == 0x02 and
                    self.receive_buffer[i+7] == 0x55 and
                    self.receive_buffer[i+8] == 0xAA):
                    
                    # 传感器ID
                    sensor_id = self.receive_buffer[i+3]
                    
                    if sensor_id in [0x01, 0x02, 0x08]:  # 有效的传感器ID
                        # 提取数据 (2字节，高位在前)
                        raw_value = (self.receive_buffer[i+4] << 8) | self.receive_buffer[i+5]
                        hex_frame = ' '.join([f'{b:02X}' for b in self.receive_buffer[i:i+9]])
                        self.debug_print(f"✓ 找到传感器帧 (位置{i}): {hex_frame}")
                        self.debug_print(f"  传感器ID: 0x{sensor_id:02X}, 数据值: {raw_value}")
                        self.debug_print(f"  校验和: 0x{self.receive_buffer[i+6]:02X}")
                        
                        self.process_sensor_data(sensor_id, raw_value)
                        frames_parsed += 1
                        self.root.after(0, lambda f=hex_frame: self.frame_display.config(
                            text=f"解析: {f}"))
                        
                        i += 9
                        continue
            
            # 如果没有找到有效帧，向前移动一个字节
            i += 1
        
        # 移除已解析的数据
        if i > 0:
            self.receive_buffer = self.receive_buffer[i:]
            self.debug_print(f"移除已解析的{i}字节，缓冲区剩余: {len(self.receive_buffer)}字节")
        
        if frames_parsed > 0:
            self.debug_print(f"成功解析了 {frames_parsed} 个数据帧")
    
    def process_sensor_data(self, sensor_id, raw_value):
        """处理传感器数据"""
        if sensor_id in self.sensor_ids:
            sensor_key = self.sensor_ids[sensor_id]
            
            # 转换数据值
            if sensor_key == 'fire':
                value = raw_value / 10.0  # 可燃气体除以10
            elif sensor_key == 'temp':
                value = float(raw_value)  # 温度直接使用
            else:
                value = float(raw_value)
            
            self.debug_print(f"传感器 {sensor_key}: 原始值={raw_value}, 转换值={value:.1f}")
            
            # 更新数据
            self.sensor_data[sensor_key]['current'] = value
            self.sensor_data[sensor_key]['history'].append(value)
            
            # 更新时间
            self.time_data.append(datetime.now())
            
            # 更新显示
            self.root.after(0, self.update_display)
            self.root.after(0, self.update_charts)
        else:
            self.debug_print(f"未知传感器ID: 0x{sensor_id:02X}, 值: {raw_value}")
    
    def send_test_data(self):
        """发送测试数据"""
        self.debug_print("发送测试数据")
        
        # 测试数据帧
        test_frames = [
            b'\xAA\x55\x01\x03\x13\x15\x55\xAA',          # 温度: 19°C
            b'\xAA\x55\x02\x01\x00\x02\x03\x55\xAA',      # CO: 2 ppm
            b'\xAA\x55\x02\x08\x00\x02\x0A\x55\xAA',      # 可燃气体: 0.2%LEL
            b'\xAA\x55\x02\x02\x01\x2C\x2F\x55\xAA',      # 空气质量: 300 AQI
        ]
        
        for i, frame in enumerate(test_frames):
            hex_str = ' '.join([f'{b:02X}' for b in frame])
            self.debug_print(f"发送测试帧 {i+1}: {hex_str}")
            
            # 添加到接收缓冲区
            self.receive_buffer.extend(frame)
            
            # 解析
            self.parse_data_frames_simple()
            
            time.sleep(0.5)
        
        messagebox.showinfo("测试", "测试数据已发送")
    
    def debug_output(self):
        """调试输出当前状态"""
        self.debug_print("=== 系统状态调试 ===")
        self.debug_print(f"串口连接状态: {self.connected}")
        self.debug_print(f"接收缓冲区大小: {len(self.receive_buffer)} 字节")
        
        # 显示传感器数据
        for key in ['co', 'fire', 'air', 'temp']:
            data = self.sensor_data[key]
            self.debug_print(f"{data['label']}: {data['current']:.1f}{data['unit']} (阈值: {data['threshold']}{data['unit']})")
        
        # 显示缓冲区内容
        if len(self.receive_buffer) > 0:
            hex_buffer = ' '.join([f'{b:02X}' for b in self.receive_buffer[:100]])
            self.debug_print(f"缓冲区前100字节: {hex_buffer}")
        else:
            self.debug_print("缓冲区为空")
    
    def update_charts(self):
        """更新图表数据"""
        try:
            sensor_keys = ['co', 'fire', 'air', 'temp']
            
            for i, key in enumerate(sensor_keys):
                data = list(self.sensor_data[key]['history'])
                times = list(self.time_data)
                
                if len(data) > 0:
                    # 确保时间数据长度匹配
                    if len(times) > len(data):
                        times = times[-len(data):]
                    elif len(data) > len(times):
                        data = data[-len(times):]
                    
                    # 更新数据线
                    self.lines[i].set_data(times, data)
                    
                    # 调整坐标轴范围
                    ax = self.axes[i]
                    ax.relim()
                    ax.autoscale_view()
            
            # 重绘图表
            self.canvas.draw()
            
        except Exception as e:
            self.debug_print(f"更新图表时出错: {e}")
    
    def update_display(self):
        """更新显示数据"""
        try:
            # 更新传感器数值显示
            for key, var in self.value_labels:
                value = self.sensor_data[key]['current']
                var.set(f"{value:.1f}")
            
            # 更新状态显示
            for key, var in self.status_labels:
                value = self.sensor_data[key]['current']
                threshold = self.sensor_data[key]['threshold']
                if value > threshold:
                    var.set("警告!")
                    index = ['co', 'fire', 'air', 'temp'].index(key)
                    self.sensor_frames[index].config(bg='#FFCCCC')
                else:
                    var.set("正常")
                    index = ['co', 'fire', 'air', 'temp'].index(key)
                    self.sensor_frames[index].config(bg='white')
            
        except Exception as e:
            self.debug_print(f"更新显示时出错: {e}")
    
    def set_thresholds(self):
        """设置阈值并通过串口发送到STM32"""
        try:
            success_count = 0
            total_count = len(self.threshold_vars)
            
            for i, (key, var) in enumerate(self.threshold_vars.items()):
                try:
                    new_threshold = float(var.get())
                    if new_threshold < 0:
                        raise ValueError("阈值不能为负数")
                    
                    # 更新本地数据
                    self.sensor_data[key]['threshold'] = new_threshold
                    
                    # 更新显示
                    if 'threshold_label' in self.sensor_data[key]:
                        unit = self.sensor_data[key]['unit']
                        self.sensor_data[key]['threshold_label'].config(
                            text=f"当前: {new_threshold:.1f} {unit}")
                    
                    # 通过串口发送阈值设置到STM32
                    if self.send_threshold_to_stm32(key, new_threshold):
                        success_count += 1
                        self.debug_print(f"成功设置 {key} 阈值为: {new_threshold}")
                    else:
                        self.debug_print(f"设置 {key} 阈值发送失败")
                    
                    # 如果不是最后一个阈值，等待1秒再发送下一个
                    if i < total_count - 1:
                        self.debug_print(f"等待1秒后发送下一个阈值...")
                        time.sleep(1)
                    
                except ValueError as e:
                    messagebox.showerror("输入错误", 
                        f"{self.sensor_data[key]['label']}阈值输入无效: {str(e)}")
                    return
            
            if success_count == total_count:
                messagebox.showinfo("成功", f"所有{total_count}个阈值已更新并发送到STM32")
            elif success_count > 0:
                messagebox.showwarning("部分成功", 
                    f"{success_count}/{total_count}个阈值已更新并发送到STM32")
            else:
                messagebox.showerror("失败", "所有阈值发送到STM32失败")
            
        except Exception as e:
            self.debug_print(f"设置阈值时出错: {e}")
            messagebox.showerror("错误", f"设置阈值时出错: {str(e)}")

    def send_threshold_to_stm32(self, sensor_key, value):
        """发送阈值设置命令到STM32"""
        if not self.connected or not self.serial_port:
            self.debug_print("串口未连接，无法发送阈值设置")
            return False
        
        try:
            # 阈值标识符映射
            threshold_ids = {
                'co': 0x04,     # 一氧化碳阈值
                'fire': 0x05,   # 可燃气体阈值
                'air': 0x06,    # 空气质量阈值
                'temp': 0x07    # 温度阈值
            }
            
            if sensor_key not in threshold_ids:
                self.debug_print(f"未知的传感器键: {sensor_key}")
                return False
            
            threshold_id = threshold_ids[sensor_key]
            
            # 将浮点数转换为整数（根据传感器类型）
            if sensor_key == 'fire':
                # 可燃气体需要乘以10（因为接收时除以10）
                int_value = int(value * 10)
            elif sensor_key == 'temp':
                # 温度直接取整
                int_value = int(value)
            else:
                # 其他传感器直接取整
                int_value = int(value)
            
            # 确保值在合理范围内
            int_value = max(0, min(65535, int_value))  # 限制在0-65535范围内
            
            # 构建数据帧：AA 55 数据长度 阈值ID 数据高位 数据低位 校验和 55 AA
            frame = bytearray()
            frame.extend([0xAA, 0x55])          # 帧头
            frame.append(0x02)                  # 数据长度：阈值ID(1) + 数据(2) = 3
            frame.append(threshold_id)          # 阈值标识符
            
            # 添加2字节数据（高位在前）
            frame.append((int_value >> 8) & 0xFF)  # 高位
            frame.append(int_value & 0xFF)         # 低位
            
            # 计算校验和（从帧头到数据的所有字节的和，取低8位）
            checksum = sum(frame) & 0xFF
            checksum = (checksum + 0x55 + 0xAA) & 0xFF
            frame.append(checksum)              # 校验和
            
            frame.extend([0x55, 0xAA])          # 帧尾
            
            # 发送数据
            self.serial_port.write(frame)
            
            # 显示发送的数据帧
            hex_frame = ' '.join([f'{b:02X}' for b in frame])
            self.debug_print(f"发送阈值设置帧: {hex_frame}")
            self.debug_print(f"阈值设置 - 传感器: {sensor_key}, ID: 0x{threshold_id:02X}, 值: {value} -> 0x{int_value:04X}")
            
            # 在界面上显示发送的数据
            self.root.after(0, lambda f=hex_frame: self.frame_display.config(
                text=f"发送阈值: {f}"))
            
            return True
            
        except Exception as e:
            self.debug_print(f"发送阈值设置失败: {e}")
            return False

    def reset_thresholds(self):
        """恢复默认阈值并通过串口发送到STM32"""
        default_values = {'co': 50, 'fire': 30, 'air': 100, 'temp': 35}
        
        success_count = 0
        total_count = len(default_values)
        
        for i, (key, default_value) in enumerate(default_values.items()):
            self.threshold_vars[key].set(str(default_value))
            self.sensor_data[key]['threshold'] = default_value
            
            # 更新显示
            if 'threshold_label' in self.sensor_data[key]:
                unit = self.sensor_data[key]['unit']
                self.sensor_data[key]['threshold_label'].config(
                    text=f"当前: {default_value} {unit}")
            
            # 通过串口发送阈值设置到STM32
            if self.send_threshold_to_stm32(key, default_value):
                success_count += 1
                self.debug_print(f"成功重置 {key} 阈值为默认值: {default_value}")
            else:
                self.debug_print(f"重置 {key} 阈值发送失败")
            
            # 如果不是最后一个阈值，等待1秒再发送下一个
            if i < total_count - 1:
                self.debug_print(f"等待1秒后发送下一个阈值...")
                time.sleep(1)
        
        if success_count == total_count:
            messagebox.showinfo("成功", f"所有{total_count}个默认阈值已发送到STM32")
        elif success_count > 0:
            messagebox.showwarning("部分成功", 
                f"{success_count}/{total_count}个默认阈值已发送到STM32")
        else:
            messagebox.showerror("失败", "所有默认阈值发送到STM32失败")
    
    def show_help(self):
        """显示帮助信息"""
        help_text = """
        智能传感器监控系统
        
        数据帧格式（二进制）：
        AA 55 数据长度 传感器ID 数据... 校验和 55 AA
        
        传感器ID：
        0x01 - 一氧化碳 (ppm)
        0x02 - 空气质量 (AQI)
        0x03 - 温度 (°C)
        0x08 - 可燃气体 (%LEL，实际值=原始值/10)
        
        使用说明：
        1. 选择串口和波特率，点击"打开串口"
        2. 点击"调试输出"查看程序状态
        3. 点击"测试"按钮发送示例数据
        4. 设置阈值后点击"设置阈值"
        5. 查看终端输出以获取调试信息
        """
        messagebox.showinfo("系统帮助", help_text)
    
    def on_closing(self):
        """关闭程序时的清理工作"""
        self.debug_print("程序正在关闭...")
        self.running = False
        self.close_serial()
        self.root.destroy()

def main():
    # 添加启动日志
    print("=" * 50)
    print("智能传感器监控系统启动 - 简化解析版本")
    print("=" * 50)
    sys.stdout.flush()
    
    root = tk.Tk()
    app = SensorMonitorApp(root)
    
    # 设置关闭事件处理
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    
    # 运行主循环
    root.mainloop()

if __name__ == "__main__":
    main()