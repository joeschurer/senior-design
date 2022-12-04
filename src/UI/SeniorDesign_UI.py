import os, sys, serial, serial.tools.list_ports, warnings
sys.path.append(".")
if sys.platform == "darwin":
    os.environ["QT_MAC_WANTS_LAYER"] = "1"
from PyQt5 import QtWidgets, uic
from PyQt5.QtWidgets import QApplication, QComboBox, QDialog, QMainWindow, QWidget, QLabel, QTextEdit, QListWidget, \
    QListView
from PyQt5.QtCore import QSize, QRect, QObject, pyqtSignal, QThread, pyqtSignal, pyqtSlot
import time

#be sure to pip install PyQt5 and pyserial

ser = serial.Serial()
ports = []


class Worker(QObject):
    finished = pyqtSignal()
    intReady = pyqtSignal(str)
    fileDone = pyqtSignal(list)
    fileData = pyqtSignal(list)
    connected = pyqtSignal()
    disconnected = pyqtSignal()
    disconnectError = pyqtSignal()



    @pyqtSlot()
    def __init__(self):
        super(Worker, self).__init__()
        self.working = True
        self.is_paused = False

    def work(self):
        while self.working:
            while self.is_paused: #Provides the ability to pause the thread
                time.sleep(0)
                #self.is_paused = False
                print("paused")
            try:
                #maybe decode after
                line = ser.readline().decode('utf-8')[:-2]
                #detect file being sent
                #prob sit in this loop until done....
                if(line.find('file_begin') != -1):
                    file_contents = []
                    print("reading file")
                    #data_raw = ser.read_until(b'E')
                    #print(data_raw)
                    line = ser.readline().decode('utf-8')[:-2] #throw out first line
                    line = ser.readline()
                    line_num = 0
                    while(line.count(b'E') != 1):
                    #while(line.find('END_SCAN') == -1):
                        file_contents.append(line)
                        #line = ser.readline().decode('utf-8')[:-2]
                        line = ser.readline()
                        #print(line)
                        line_num = line_num+1
                        if(line_num % 10000==0):
                            print("At line: " + str(line_num))

                    #print(file_contents)
                    print("file read complete")
                    self.fileDone.emit(file_contents)
                else:
                    self.intReady.emit(line)


            except Exception as e:
                print("Failed to read!")
                print(repr(e))
                self.is_paused = True
                self.disconnectError.emit()




        self.finished.emit()

    def pause_thread(self):
        self.is_paused = True

    def unpause_thread(self):
        self.is_paused = False

class SeniorDesign_UI(QtWidgets.QMainWindow):
    thread = None
    worker = None
    loop_status = False
    file_lines = []
    def __init__(self):

        ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            #if 'USB' in p.description
        ]
        print(ports)
        super(SeniorDesign_UI, self).__init__()
        ui_path = os.path.dirname(os.path.abspath(__file__))
        uic.loadUi(os.path.join(ui_path,'SeniorDesign_UI.ui'), self)

        if len(ports) >= 1:
            warnings.warn('Connected....')
            ser.port = ports[0]
            ser.baudrate = 230400
            self.port_label.setText(ports[0])
            self.start_loop()
        self.show()
        self.initUI()

    def loop_finished(self):
        print("Loop done")

    def onFileData(self,i):
        self.file_lines.append(i)

    def onFileDone(self,data):
        #Prob skip lines 0 and n-1,n

        new_data = [x.decode('utf-8')[:-2] for x in data]
        with open('scan.txt', 'w') as f:
            #f.write(data)
            for line in new_data:
                line = line + '\n'#remove if not needed
                f.write(line)
        self.file_lines.clear()

    def onIntReady(self, i):
        self.textEdit.append("{}".format(i))
        self.port_label.setText(str(ser.port))

    def onDisconnectError(self):
        self.port_label.setText("Disconnected")


    def start_loop(self):
        self.loop_status = True
        if(ser.isOpen() == False):
            ser.open()

        self.worker = Worker()
        self.thread = QThread()
        self.worker.moveToThread(self.thread)
        self.thread.started.connect(self.worker.work)
        self.worker.intReady.connect(self.onIntReady)
        self.worker.fileData.connect(self.onFileData)
        self.worker.fileDone.connect(self.onFileDone)

        self.worker.disconnectError.connect(self.onDisconnectError)
        #self.stop_button.clicked.connect(self.worker.pause_thread)
        #self.start_button.clicked.connect(self.worker.unpause_thread)
        #self.connect_button.clicked.connect(self.reconnect)
        self.worker.finished.connect(self.loop_finished)
        self.worker.finished.connect(self.thread.quit)
        self.worker.finished.connect(self.worker.deleteLater)
        self.thread.finished.connect(self.thread.deleteLater)

        self.thread.start()

    def on_save_button_clicked(self):
        with open('test.txt', 'w') as f:
            my_text = self.textEdit.toPlainText()
            #file_text = my_text[my_text.find("BEGIN_SCAN"):my_text.find("END_SCAN")] #Should grab only the file for Collin
            #file_text = file_text.split("BEGIN_SCAN\n")
            #print(file_text)
            #f.write(file_text[1])
            f.write(my_text)

    def on_stop_button_clicked(self):
        self.worker.pause_thread()
        print("Thread Paused")

    def on_start_button_clicked(self):
        self.worker.unpause_thread()
        print("Thread Unpaused")

    def on_scan_button_clicked(self):
        ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            #if 'USB' in p.description
        ]
        print(ports)
        if len(ports) >= 1:
            self.device_box.clear()
            for port in ports:
                self.device_box.addItem(port)


    def on_connect_button_clicked(self):
        p = self.device_box.currentIndex()
        ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            #if 'USB' in p.description
        ]

        print(p)
        print(ports)
        if(ports):
            ser.port = ports[p]
            #ser.baudrate = 500000
            ser.baudrate = 1000000
            if(self.loop_status== False):
                self.start_loop()
            else:
                self.worker.unpause_thread()
            print(ports[p])
            self.port_label.setText(ports[p])
        else:
            print("No ports identified. Skipping...")

    def upload(self):
        try:
            option = self.prog_select.currentIndex()
            if(option != 0):
                to_send = str(option)
                print(to_send.encode())
                ser.write(to_send.encode())
        except:
            print("Serial possibly not connected, failed to upload!")

    def on_clear_button_clicked(self):
        self.textEdit.clear()


    def initUI(self):
        self.upload_button.released.connect(self.upload)





if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = SeniorDesign_UI()
    title = "Senior Design"
    window.setWindowTitle(title)
    app.exec_()

