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
import socket
import ipaddress

class SensorMonitorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("智能传感器监控系统 - STM32")
        
        # 设置窗口初始大小
        self.root.geometry("1200x750")
        
        # 串口相关变量
        self.serial_port = None
        self.serial_thread = None
        self.running = True
        self.receive_buffer = bytearray()
        self.last_receive_time = time.time()
        
        # 串口配置
        self.port_var = StringVar()
        self.baudrate_var = IntVar(value=115200)
        self.serial_connected = False
        
        # UDP网络相关变量
        self.udp_socket = None
        self.udp_thread = None
        self.udp_running = False
        self.udp_connected = False
        self.last_client_addr = None  # 保存最后一个客户端地址
        
        # 网络配置
        self.network_mode_var = StringVar(value="serial")  # serial 或 udp
        self.server_ip_var = StringVar(value="0.0.0.0")    # 服务器IP
        self.server_port_var = IntVar(value=8888)          # 服务器端口
        self.client_ip_var = StringVar(value="无连接")     # 客户端IP
        self.received_bytes_var = StringVar(value="0")     # 接收字节数
        
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
        
        # 阈值ID映射
        self.threshold_ids = {
            'co': 0x04,     # 一氧化碳阈值
            'fire': 0x05,   # 可燃气体阈值
            'air': 0x06,    # 空气质量阈值
            'temp': 0x07    # 温度阈值
        }
        
        # 时间数据
        self.time_data = deque(maxlen=20)
        self.init_history_data()
        
        # UDP接收统计
        self.udp_received_bytes = 0
        self.last_udp_data_time = 0
        
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
        formatted_message = f"[{timestamp}] {message}"
        
        try:
            print(formatted_message)
            
            # 安全地刷新输出 - 只有在 stdout 不为 None 时才刷新
            if sys.stdout is not None:
                sys.stdout.flush()
        except Exception as e:
            # 如果打印失败，尝试写入文件
            try:
                with open("debug.log", "a", encoding="utf-8") as f:
                    f.write(f"[{timestamp}] {message}\n")
                    f.write(f"[{timestamp}] 打印错误: {e}\n")
            except:
                pass  # 如果连文件写入都失败，就放弃
    
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
        self.main_container.rowconfigure(0, weight=0)   # 连接方式选择
        self.main_container.rowconfigure(1, weight=0)   # 串口/UDP配置
        self.main_container.rowconfigure(2, weight=3)   # 图表区域
        self.main_container.rowconfigure(3, weight=1)   # 传感器显示
        self.main_container.rowconfigure(4, weight=1)   # 阈值设置
        
        # 创建五个主要区域
        self.create_connection_mode()
        self.create_serial_control()
        self.create_udp_control()
        self.create_chart_area()
        self.create_sensor_display()
        self.create_threshold_area()
    
    def create_connection_mode(self):
        """创建连接方式选择区域"""
        mode_frame = ttk.LabelFrame(self.main_container, text="连接方式", padding="10")
        mode_frame.grid(row=0, column=0, sticky='ew', padx=5, pady=5)
        
        # 连接方式选择
        ttk.Label(mode_frame, text="选择连接方式:").pack(side=tk.LEFT, padx=5)
        
        ttk.Radiobutton(mode_frame, text="串口", variable=self.network_mode_var, 
                       value="serial", command=self.on_connection_mode_change).pack(side=tk.LEFT, padx=10)
        ttk.Radiobutton(mode_frame, text="UDP网络", variable=self.network_mode_var,
                       value="udp", command=self.on_connection_mode_change).pack(side=tk.LEFT, padx=10)
        
        # 连接状态显示
        self.connection_status_label = ttk.Label(mode_frame, text="当前: 未连接", foreground="red")
        self.connection_status_label.pack(side=tk.LEFT, padx=20)
    
    def create_serial_control(self):
        """创建串口控制区域"""
        self.serial_frame = ttk.LabelFrame(self.main_container, text="串口设置", padding="10")
        self.serial_frame.grid(row=1, column=0, sticky='ew', padx=5, pady=5)
        
        # 串口选择
        ttk.Label(self.serial_frame, text="串口:").grid(row=0, column=0, padx=5, pady=5, sticky='w')
        self.port_combobox = ttk.Combobox(self.serial_frame, textvariable=self.port_var, width=15)
        self.port_combobox.grid(row=0, column=1, padx=5, pady=5)
        
        # 波特率选择
        ttk.Label(self.serial_frame, text="波特率:").grid(row=0, column=2, padx=5, pady=5, sticky='w')
        baudrate_combo = ttk.Combobox(self.serial_frame, textvariable=self.baudrate_var, width=10)
        baudrate_combo['values'] = (9600, 19200, 38400, 57600, 115200)
        baudrate_combo.grid(row=0, column=3, padx=5, pady=5)
        
        # 按钮
        self.serial_connect_button = ttk.Button(self.serial_frame, text="打开串口", command=self.toggle_serial)
        self.serial_connect_button.grid(row=0, column=4, padx=20, pady=5)
        
        ttk.Button(self.serial_frame, text="扫描串口", command=self.scan_ports).grid(row=0, column=5, padx=5, pady=5)
        
        # 数据帧显示
        self.serial_frame_display = ttk.Label(self.serial_frame, text="接收数据: 无", foreground="blue", 
                                             font=('Consolas', 9))
        self.serial_frame_display.grid(row=1, column=0, columnspan=6, padx=5, pady=5, sticky='w')
    
    def create_udp_control(self):
        """创建UDP网络控制区域"""
        self.udp_frame = ttk.LabelFrame(self.main_container, text="UDP网络设置", padding="10")
        self.udp_frame.grid(row=1, column=0, sticky='ew', padx=5, pady=5)
        
        # 服务器IP
        ttk.Label(self.udp_frame, text="服务器IP:").grid(row=0, column=0, padx=5, pady=5, sticky='w')
        server_ip_entry = ttk.Entry(self.udp_frame, textvariable=self.server_ip_var, width=15)
        server_ip_entry.grid(row=0, column=1, padx=5, pady=5)
        ttk.Label(self.udp_frame, text="(0.0.0.0 监听所有接口)").grid(row=0, column=2, padx=5, sticky='w')
        
        # 服务器端口
        ttk.Label(self.udp_frame, text="端口:").grid(row=0, column=3, padx=5, pady=5, sticky='w')
        server_port_entry = ttk.Entry(self.udp_frame, textvariable=self.server_port_var, width=10)
        server_port_entry.grid(row=0, column=4, padx=5, pady=5)
        
        # 连接按钮
        self.udp_connect_button = ttk.Button(self.udp_frame, text="启动UDP服务器", command=self.toggle_udp_server)
        self.udp_connect_button.grid(row=0, column=5, padx=20, pady=5)
        
        # 客户端信息
        ttk.Label(self.udp_frame, text="客户端IP:").grid(row=1, column=0, padx=5, pady=5, sticky='w')
        client_ip_label = ttk.Label(self.udp_frame, textvariable=self.client_ip_var, foreground="blue")
        client_ip_label.grid(row=1, column=1, columnspan=2, padx=5, pady=5, sticky='w')
        
        # 接收统计
        ttk.Label(self.udp_frame, text="接收字节数:").grid(row=1, column=3, padx=5, pady=5, sticky='w')
        received_bytes_label = ttk.Label(self.udp_frame, textvariable=self.received_bytes_var, foreground="green")
        received_bytes_label.grid(row=1, column=4, padx=5, pady=5, sticky='w')
        
        # UDP数据帧显示
        self.udp_frame_display = ttk.Label(self.udp_frame, text="UDP数据: 等待连接...", foreground="purple",
                                          font=('Consolas', 9))
        self.udp_frame_display.grid(row=2, column=0, columnspan=6, padx=5, pady=5, sticky='w')
        
        # 默认隐藏UDP设置
        self.udp_frame.grid_remove()
    
    def on_connection_mode_change(self):
        """连接方式改变时的处理"""
        mode = self.network_mode_var.get()
        
        if mode == "serial":
            # 显示串口设置，隐藏UDP设置
            self.serial_frame.grid()
            self.udp_frame.grid_remove()
            self.connection_status_label.config(text="当前: 串口模式")
            self.close_udp_server()  # 关闭UDP服务器
        else:  # udp
            # 显示UDP设置，隐藏串口设置
            self.serial_frame.grid_remove()
            self.udp_frame.grid()
            self.connection_status_label.config(text="当前: UDP模式")
            self.close_serial()  # 关闭串口
    
    def create_chart_area(self):
        """创建图表区域"""
        self.chart_container = ttk.LabelFrame(self.main_container, text="传感器历史数据", padding="5")
        self.chart_container.grid(row=2, column=0, sticky='nsew', padx=5, pady=5)
        
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
        self.sensor_container.grid(row=3, column=0, sticky='nsew', padx=5, pady=5)
        
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
        """创建阈值设置区域 - 优化版本"""
        self.threshold_container = ttk.LabelFrame(self.main_container, text="阈值设置", padding="10")
        self.threshold_container.grid(row=4, column=0, sticky='nsew', padx=5, pady=5)
        
        # 配置阈值容器的权重，使其可以扩展
        self.threshold_container.columnconfigure(0, weight=1)
        
        # 创建一个主框架来包含所有内容
        main_frame = ttk.Frame(self.threshold_container)
        main_frame.grid(row=0, column=0, sticky='nsew')
        main_frame.columnconfigure(0, weight=1)
        
        # 创建阈值设置网格（4行 x 3列）
        self.threshold_vars = {}
        
        threshold_configs = [
            ('co', '一氧化碳阈值:', 50, 'ppm', '0-100 ppm'),
            ('fire', '可燃气体阈值:', 30, '%LEL', '0-50 %LEL'),
            ('air', '空气质量阈值:', 100, 'AQI', '0-150 AQI')
            # ('temp', '温度阈值:', 35, '°C', '0-50 °C')
        ]
        
        for i, (key, label_text, default_value, unit, range_text) in enumerate(threshold_configs):
            # 标签
            label = ttk.Label(main_frame, text=label_text, 
                            font=('Microsoft YaHei', 10))
            label.grid(row=i, column=0, padx=5, pady=8, sticky='w')
            
            # 范围提示
            range_label = ttk.Label(main_frame, text=range_text,
                                font=('Microsoft YaHei', 8), foreground='gray')
            range_label.grid(row=i, column=1, padx=5, pady=2, sticky='w')
            
            # 输入框
            var = tk.StringVar(value=str(default_value))
            self.threshold_vars[key] = var
            
            entry_frame = ttk.Frame(main_frame)
            entry_frame.grid(row=i, column=2, padx=10, pady=5, sticky='w')
            
            entry = ttk.Entry(entry_frame, textvariable=var, width=10,
                            font=('Arial', 10))
            entry.pack(side=tk.LEFT)
            ttk.Label(entry_frame, text=f" {unit}", 
                    font=('Arial', 9)).pack(side=tk.LEFT)
            
            # 当前阈值显示
            current_label = ttk.Label(main_frame, 
                                    text=f"当前: {default_value} {unit}",
                                    font=('Microsoft YaHei', 10))
            current_label.grid(row=i, column=3, padx=10, pady=5, sticky='w')
            self.sensor_data[key]['threshold_label'] = current_label
        
        # 添加分隔线
        separator = ttk.Separator(main_frame, orient='horizontal')
        separator.grid(row=4, column=0, columnspan=4, sticky='ew', pady=10)
        
        # 按钮行
        button_frame = ttk.Frame(main_frame)
        button_frame.grid(row=5, column=0, columnspan=4, sticky='ew', pady=5)
        
        # 配置按钮行的列权重
        for i in range(5):
            button_frame.columnconfigure(i, weight=1)
        
        # 按钮配置
        buttons = [
            ("设置阈值", self.set_thresholds),
            ("恢复默认值", self.reset_thresholds),
            ("查看帮助", self.show_help),
            ("测试", self.send_test_data),
            ("调试输出", self.debug_output)
        ]
        
        for i, (text, command) in enumerate(buttons):
            btn = ttk.Button(button_frame, text=text, command=command)
            btn.grid(row=0, column=i, padx=5, pady=5, sticky='ew')
        
        # 添加一些垂直空间，确保按钮完全可见
        main_frame.rowconfigure(6, weight=1)
        
    def toggle_serial(self):
        """打开/关闭串口"""
        if not self.serial_connected:
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
            
            self.serial_connected = True
            self.serial_connect_button.config(text="关闭串口")
            self.connection_status_label.config(text=f"状态: 串口已连接 {port}", foreground="green")
            
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
        
        self.serial_connected = False
        self.serial_connect_button.config(text="打开串口")
        if self.network_mode_var.get() == "serial":
            self.connection_status_label.config(text="状态: 串口未连接", foreground="red")
        self.serial_frame_display.config(text="接收数据: 无")
    
    def toggle_udp_server(self):
        """启动/停止UDP服务器"""
        if not self.udp_connected:
            self.start_udp_server()
        else:
            self.close_udp_server()
    
    def start_udp_server(self):
        """启动UDP服务器"""
        try:
            # 获取IP和端口
            ip = self.server_ip_var.get()
            port = self.server_port_var.get()
            
            # 验证IP地址
            try:
                ipaddress.ip_address(ip)
            except ValueError:
                messagebox.showerror("错误", "IP地址格式不正确")
                return
            
            # 验证端口
            if port < 1 or port > 65535:
                messagebox.showerror("错误", "端口号必须在1-65535之间")
                return
            
            self.debug_print(f"尝试启动UDP服务器: {ip}:{port}")
            
            # 创建UDP socket
            self.udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.udp_socket.settimeout(0.5)  # 设置超时以便检查停止标志
            
            # 绑定到地址和端口
            self.udp_socket.bind((ip, port))
            
            self.udp_connected = True
            self.udp_running = True
            self.udp_connect_button.config(text="停止UDP服务器")
            self.connection_status_label.config(text=f"状态: UDP服务器运行中 {ip}:{port}", foreground="green")
            
            # 重置统计信息
            self.udp_received_bytes = 0
            self.received_bytes_var.set("0")
            self.client_ip_var.set("等待连接...")
            self.last_client_addr = None
            
            # 启动UDP接收线程
            self.udp_thread = threading.Thread(target=self.receive_udp_data, daemon=True)
            self.udp_thread.start()
            
            self.debug_print(f"UDP服务器已启动，监听 {ip}:{port}")
            messagebox.showinfo("成功", f"UDP服务器已启动\n监听地址: {ip}\n端口: {port}")
            
        except Exception as e:
            self.debug_print(f"启动UDP服务器失败: {str(e)}")
            messagebox.showerror("错误", f"启动UDP服务器失败: {str(e)}")
    
    def close_udp_server(self):
        """关闭UDP服务器"""
        self.udp_running = False
        
        if self.udp_socket:
            try:
                self.udp_socket.close()
            except:
                pass
        
        self.udp_connected = False
        self.udp_connect_button.config(text="启动UDP服务器")
        if self.network_mode_var.get() == "udp":
            self.connection_status_label.config(text="状态: UDP服务器未启动", foreground="red")
        self.client_ip_var.set("无连接")
        self.udp_frame_display.config(text="UDP数据: 服务器已停止")
        self.last_client_addr = None
        
        self.debug_print("UDP服务器已关闭")
    
    def receive_udp_data(self):
        """接收UDP数据"""
        self.debug_print("UDP接收线程启动")
        
        while self.udp_running and self.udp_connected:
            try:
                if self.udp_socket:
                    # 接收数据
                    data, addr = self.udp_socket.recvfrom(4096)  # 最大接收4096字节
                    
                    if data:
                        client_ip, client_port = addr
                        self.last_client_addr = addr  # 保存客户端地址
                        
                        # 更新最后接收时间
                        self.last_udp_data_time = time.time()
                        
                        # 更新客户端IP显示
                        self.root.after(0, lambda ip=client_ip: self.client_ip_var.set(f"{ip}:{client_port}"))
                        
                        # 更新接收统计
                        self.udp_received_bytes += len(data)
                        self.root.after(0, lambda b=self.udp_received_bytes: self.received_bytes_var.set(str(b)))
                        
                        # 显示接收信息
                        hex_str = ' '.join([f'{b:02X}' for b in data[:30]])  # 只显示前30字节
                        if len(data) > 30:
                            hex_str += f" ... (共{len(data)}字节)"
                        
                        display_text = f"来自 {client_ip}:{client_port} - {hex_str}"
                        self.root.after(0, lambda t=display_text: self.udp_frame_display.config(text=t))
                        
                        # 处理数据
                        self.process_udp_data(data, client_ip)
                
            except socket.timeout:
                # 超时正常，继续循环
                continue
            except Exception as e:
                if self.udp_running:  # 只有在运行状态下才显示错误
                    self.debug_print(f"接收UDP数据出错: {e}")
                time.sleep(0.1)
        
        self.debug_print("UDP接收线程结束")
    
    def process_udp_data(self, data, client_ip):
        """处理UDP接收到的数据"""
        # 将数据添加到接收缓冲区
        self.receive_buffer.extend(data)
        
        # 显示调试信息
        self.debug_print(f"从 {client_ip} 接收到 {len(data)} 字节UDP数据")
        
        # 尝试解析数据帧
        if len(self.receive_buffer) >= 7:
            self.parse_data_frames_simple()
    
    def read_serial_data(self):
        """读取串口数据"""
        self.debug_print("串口读取线程启动")
        
        while self.running and self.serial_connected:
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
                        self.root.after(0, lambda s=short_str: self.serial_frame_display.config(
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
                    
                    # 根据连接方式更新显示
                    if self.network_mode_var.get() == "serial":
                        self.root.after(0, lambda f=hex_frame: self.serial_frame_display.config(
                            text=f"解析温度: {f}"))
                    else:
                        self.root.after(0, lambda f=hex_frame: self.udp_frame_display.config(
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
                        
                        # 根据连接方式更新显示
                        if self.network_mode_var.get() == "serial":
                            self.root.after(0, lambda f=hex_frame: self.serial_frame_display.config(
                                text=f"解析: {f}"))
                        else:
                            self.root.after(0, lambda f=hex_frame: self.udp_frame_display.config(
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
        self.debug_print(f"当前连接模式: {self.network_mode_var.get()}")
        
        if self.network_mode_var.get() == "serial":
            self.debug_print(f"串口连接状态: {self.serial_connected}")
        else:
            self.debug_print(f"UDP服务器状态: {self.udp_connected}")
            self.debug_print(f"客户端地址: {self.last_client_addr}")
            self.debug_print(f"接收字节数: {self.udp_received_bytes}")
        
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
        """设置阈值并通过串口或UDP发送到STM32"""
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
                    
                    # 根据连接方式发送阈值设置
                    if self.network_mode_var.get() == "serial":
                        # 串口模式
                        if self.send_threshold_serial(key, new_threshold):
                            success_count += 1
                            self.debug_print(f"成功通过串口设置 {key} 阈值为: {new_threshold}")
                        else:
                            self.debug_print(f"串口设置 {key} 阈值发送失败")
                    else:
                        # UDP模式
                        if self.send_threshold_udp(key, new_threshold):
                            success_count += 1
                            self.debug_print(f"成功通过UDP设置 {key} 阈值为: {new_threshold}")
                        else:
                            self.debug_print(f"UDP设置 {key} 阈值发送失败")
                    
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

    def send_threshold_serial(self, sensor_key, value):
        """通过串口发送阈值设置命令到STM32"""
        if not self.serial_connected or not self.serial_port:
            self.debug_print("串口未连接，无法发送阈值设置")
            return False
        
        try:
            if sensor_key not in self.threshold_ids:
                self.debug_print(f"未知的传感器键: {sensor_key}")
                return False
            
            threshold_id = self.threshold_ids[sensor_key]
            
            # 将浮点数转换为整数（根据传感器类型）
            int_value = self.convert_threshold_value(sensor_key, value)
            
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
            self.debug_print(f"串口发送阈值设置帧: {hex_frame}")
            self.debug_print(f"阈值设置 - 传感器: {sensor_key}, ID: 0x{threshold_id:02X}, 值: {value} -> 0x{int_value:04X}")
            
            # 在界面上显示发送的数据
            self.root.after(0, lambda f=hex_frame: self.serial_frame_display.config(
                text=f"发送阈值: {f}"))
            
            return True
            
        except Exception as e:
            self.debug_print(f"串口发送阈值设置失败: {e}")
            return False
    
    def send_threshold_udp(self, sensor_key, value):
        """通过UDP发送阈值设置命令到STM32"""
        if not self.udp_connected or not self.udp_socket:
            self.debug_print("UDP未连接，无法发送阈值设置")
            return False
        
        if not self.last_client_addr:
            self.debug_print("没有客户端连接，无法发送阈值设置")
            return False
        
        try:
            if sensor_key not in self.threshold_ids:
                self.debug_print(f"未知的传感器键: {sensor_key}")
                return False
            
            threshold_id = self.threshold_ids[sensor_key]
            
            # 将浮点数转换为整数（根据传感器类型）
            int_value = self.convert_threshold_value(sensor_key, value)
            
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
            
            # 通过UDP发送数据
            self.udp_socket.sendto(frame, self.last_client_addr)
            
            # 显示发送的数据帧
            hex_frame = ' '.join([f'{b:02X}' for b in frame])
            client_ip, client_port = self.last_client_addr
            self.debug_print(f"UDP发送阈值设置帧到 {client_ip}:{client_port}: {hex_frame}")
            self.debug_print(f"阈值设置 - 传感器: {sensor_key}, ID: 0x{threshold_id:02X}, 值: {value} -> 0x{int_value:04X}")
            
            # 在界面上显示发送的数据
            self.root.after(0, lambda f=hex_frame, ip=client_ip, port=client_port: 
                          self.udp_frame_display.config(text=f"发送阈值到 {ip}:{port}: {f}"))
            
            return True
            
        except Exception as e:
            self.debug_print(f"UDP发送阈值设置失败: {e}")
            return False
    
    def convert_threshold_value(self, sensor_key, value):
        """转换阈值值为整数格式"""
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
        return int_value
    
    def reset_thresholds(self):
        """恢复默认阈值并通过串口或UDP发送到STM32"""
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
            
            # 根据连接方式发送阈值设置
            if self.network_mode_var.get() == "serial":
                # 串口模式
                if self.send_threshold_serial(key, default_value):
                    success_count += 1
                    self.debug_print(f"成功通过串口重置 {key} 阈值为默认值: {default_value}")
                else:
                    self.debug_print(f"串口重置 {key} 阈值发送失败")
            else:
                # UDP模式
                if self.send_threshold_udp(key, default_value):
                    success_count += 1
                    self.debug_print(f"成功通过UDP重置 {key} 阈值为默认值: {default_value}")
                else:
                    self.debug_print(f"UDP重置 {key} 阈值发送失败")
            
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
        
        阈值ID：
        0x04 - 一氧化碳阈值
        0x05 - 可燃气体阈值
        0x06 - 空气质量阈值
        0x07 - 温度阈值
        
        连接方式：
        1. 串口模式：
           - 选择串口和波特率
           - 点击"打开串口"
           - 通过物理串口接收数据
           - 设置阈值后通过串口发送到STM32
        
        2. UDP网络模式：
           - 设置服务器IP（0.0.0.0监听所有接口）
           - 设置端口号（默认8888）
           - 点击"启动UDP服务器"
           - 通过UDP接收网络数据
           - 客户端IP会实时显示
           - 设置阈值后通过UDP发送到STM32客户端
        
        使用说明：
        1. 选择连接方式（串口或UDP）
        2. 配置相应参数并连接
        3. 点击"调试输出"查看程序状态
        4. 点击"测试"按钮发送示例数据
        5. 设置阈值并发送到STM32设备
        6. 查看终端输出以获取调试信息
        
        UDP客户端可以使用任何支持UDP的工具发送数据，
        如：Python socket、网络调试助手等。
        
        阈值设置说明：
        - 设置阈值后，会通过当前连接方式发送到STM32设备
        - 串口模式：直接通过串口发送
        - UDP模式：发送到最后连接的客户端
        """
        messagebox.showinfo("系统帮助", help_text)
    
    def on_closing(self):
        """关闭程序时的清理工作"""
        self.debug_print("程序正在关闭...")
        self.running = False
        self.close_serial()
        self.close_udp_server()
        self.root.destroy()


def main():
    # 添加启动日志
    print("=" * 50)
    print("智能传感器监控系统启动 - 支持串口和UDP网络")
    print("=" * 50)
    
    # 可选：检查并刷新输出，但已添加安全处理
    try:
        if sys.stdout is not None:
            sys.stdout.flush()
    except:
        pass  # 如果失败，继续执行
    
    root = tk.Tk()
    app = SensorMonitorApp(root)
    
    # 设置关闭事件处理
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    
    # 运行主循环
    root.mainloop()


if __name__ == "__main__":
    main()