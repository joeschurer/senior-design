import os
import sys
sys.path.append(".")
if sys.platform == "darwin":
    os.environ["QT_MAC_WANTS_LAYER"] = "1"
from PyQt5 import QtWidgets, uic


class SeniorDesign_UI(QtWidgets.QMainWindow):
        def __init__(self):
                super(SeniorDesign_UI, self).__init__()
                ui_path = os.path.dirname(os.path.abspath(__file__))
                uic.loadUi(os.path.join(ui_path,'SeniorDesign_UI.ui'), self)
                self.show()

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = SeniorDesign_UI()
    app.exec_()