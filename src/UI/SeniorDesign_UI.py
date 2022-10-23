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

    def work(self):
        while self.working:
            line = ser.readline().decode('utf-8')
            #print(line)
            #time.sleep(0.1)
            self.intReady.emit(line)

        self.finished.emit()

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
            #ser.open()
            self.start_loop()
        self.show()

    def loop_finished(self):
        print("Loop done")
        #do any needed ui updates

    def onIntReady(self, i):
            self.textEdit.append("{}".format(i))
            #print(i)

    def start_loop(self):
        if(ser.isOpen() == False):
            ser.open()

        self.worker = Worker()
        self.thread = QThread()
        self.worker.moveToThread(self.thread)  # move the worker into the thread, do this first before connecting the signals

        self.thread.started.connect(self.worker.work) # begin our worker object's loop when the thread starts running

        self.worker.intReady.connect(self.onIntReady)

        self.stop_button.clicked.connect(self.stop_loop)      # stop the loop on the stop button click
        self.connect_button.clicked.connect(self.reconnect)
        self.worker.finished.connect(self.loop_finished)       # do something in the gui when the worker loop ends
        self.worker.finished.connect(self.thread.quit)         # tell the thread it's time to stop running
        self.worker.finished.connect(self.worker.deleteLater)
        self.thread.finished.connect(self.thread.deleteLater)

        self.thread.start()

    def stop_loop(self):
        self.worker.working = False


    def on_save_button_clicked(self):
        #name = QtGui.QFileDialog.getSaveFileName(self, 'Save File')
        with open('test.txt', 'w') as f:
            my_text = self.textEdit.toPlainText()
            print(my_text)
            f.write(my_text)

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
            self.worker.working = True
            #self.start_loop()
        else:
            print("No ports identified. Skipping...")

    def on_upload_button_clicked(self):
        option = self.prog_select.currentIndex()
        if(option != 0):
            to_send = str(option)
            print(to_send.encode())
            ser.write(to_send.encode())

    def on_clear_button_clicked(self):
        self.textEdit.clear()




if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = SeniorDesign_UI()
    app.exec_()

