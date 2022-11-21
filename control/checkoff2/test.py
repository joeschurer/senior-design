from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import numpy as np

app = QtGui.QGuiApplication([])
w = gl.GLViewWidget()
w.show()
g = gl.GLGridItem()
w.addItem(g)

data = np.genfromtxt('data.csv', delimiter=',')
#Three columns: x, y, z
z = data[:,2]
x = data[:,0]
y = data[:,1]
#stack the data into pos
pos = np.vstack((x,y,z)).transpose()


#pos = np.random.randint(-10, 10, size=(100,10,3))   # 生成三维随机数
#pos[:,:,2] = np.abs(pos[:,:,2])                     # 将 Z 是 负值的点移动到 XY 平面镜像位置

ScatterPlotItems = {}
for point in np.arange(10):
    ScatterPlotItems[point] = gl.GLScatterPlotItem(pos=pos[:,point,:])  # 100 个 三维点？
    w.addItem(ScatterPlotItems[point])


t = QtCore.QTimer()
t.timeout.connect(update)
t.start(1000)

## Start Qt event loop unless running in interactive mode.
if __name__ == '__main__':
    import sys
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QGuiApplication.instance().exec_()