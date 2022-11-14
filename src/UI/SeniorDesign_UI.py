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

    @pyqtSlot()
    def __init__(self):
        super(Worker, self).__init__()
        self.working = True
        self.is_paused = False

    def work(self):
        while self.working:
            while self.is_paused: #Provides the ability to pause the thread
                time.sleep(0)
                            
            line = ser.readline().decode('utf-8')
            #print(line)
            self.intReady.emit(line)

        self.finished.emit()

    def pause_thread(self):
        self.is_paused = True

    def unpause_thread(self):
        self.is_paused = False

class SeniorDesign_UI(QtWidgets.QMainWindow):
    def __init__(self):
        self.thread = None
        self.worker = None

        ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            if 'USB' in p.description
        ]

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

    def onIntReady(self, i):
        self.textEdit.append("{}".format(i))


    def start_loop(self):
        if(ser.isOpen() == False):
            ser.open()

        self.worker = Worker()
        self.thread = QThread()
        self.worker.moveToThread(self.thread)
        self.thread.started.connect(self.worker.work)
        self.worker.intReady.connect(self.onIntReady)
        self.stop_button.clicked.connect(self.worker.pause_thread)
        self.start_button.clicked.connect(self.worker.unpause_thread)
        self.connect_button.clicked.connect(self.reconnect)
        self.worker.finished.connect(self.loop_finished)
        self.worker.finished.connect(self.thread.quit)
        self.worker.finished.connect(self.worker.deleteLater)
        self.thread.finished.connect(self.thread.deleteLater)

        self.thread.start()

    def on_save_button_clicked(self):
        with open('test.txt', 'w') as f:
            my_text = self.textEdit.toPlainText()
            file_text = my_text[my_text.find("file_begin"):my_text.find("file_end")] #Should grab only the file for Colin
            f.write(file_text)

    def on_scan_button_clicked(self):
        ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            if 'USB' in p.description
        ]

        for index, port in enumerate(ports):
            self.device_box.setItemText(index,port)


    def reconnect(self):
        p = self.device_box.currentIndex()
        ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            if 'USB' in p.description
        ]

        print(p)
        print(ports)
        if(ports):
            ser = serial.Serial(ports[p],230400)
            ser.port = ports[p]
            ser.baudrate = 230400
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

