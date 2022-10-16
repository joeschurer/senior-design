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

ports = [
    p.device
    for p in serial.tools.list_ports.comports()
    if 'USB' in p.description
]

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
            print(line)
            time.sleep(0.1)
            self.intReady.emit(line)

        self.finished.emit()

class SeniorDesign_UI(QtWidgets.QMainWindow):
    def __init__(self):
            super(SeniorDesign_UI, self).__init__()
            ui_path = os.path.dirname(os.path.abspath(__file__))
            uic.loadUi(os.path.join(ui_path,'SeniorDesign_UI.ui'), self)

            if len(ports) >= 1:
                warnings.warn('Connected....')
                ser = serial.Serial(ports[0],9600)
                self.port_label.setText(ports[0])
            self.show()

    def start_loop(self):

        self.worker = Worker()   # a new worker to perform those tasks
        self.thread = QThread()  # a new thread to run our background tasks in
        self.worker.moveToThread(self.thread)  # move the worker into the thread, do this first before connecting the signals

        self.thread.started.connect(self.worker.work) # begin our worker object's loop when the thread starts running

        self.worker.intReady.connect(self.onIntReady)

        self.pushButton_2.clicked.connect(self.stop_loop)      # stop the loop on the stop button click

        self.worker.finished.connect(self.loop_finished)       # do something in the gui when the worker loop ends
        self.worker.finished.connect(self.thread.quit)         # tell the thread it's time to stop running
        self.worker.finished.connect(self.worker.deleteLater)  # have worker mark itself for deletion
        self.thread.finished.connect(self.thread.deleteLater)  # have thread mark itself for deletion

        self.thread.start()

    def stop_loop(self):
        self.worker.working = False

    def onIntReady(self, i):
        self.textEdit.append("{}".format(i))
        print(i)


    def on_save_button_clicked(self):
        #name = QtGui.QFileDialog.getSaveFileName(self, 'Save File')
        with open('test.txt', 'w') as f:
            my_text = self.textEdit.toPlainText()
            f.write(my_text)

    def on_scan_button_clicked(self):
        ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            if 'USB' in p.description
        ]

        for index, port in enumerate(ports):
            device_box.setItemText(index,port)


    def on_connect_button_clicked(self):
        p = device_box.currentIndex()
        ser = serial.Serial(ports[p],9600)

    def on_upload_button_clicked(self):
        option = prog_select.currentIndex()
        if(option != 0):
            print(option.encode())
            ser.write(option.encode())

    def on_clear_button_clicked(self):
        self.textEdit.clear()




if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = SeniorDesign_UI()
    app.exec_()

