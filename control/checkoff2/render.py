from mayavi import mlab
import numpy as np
##Read in data from data.csv 
data = np.genfromtxt('data.csv', delimiter=',')
#Three columns: x, y, z
x = data[:,0]
y = data[:,1]
z = data[:,2]
mlab.figure(bgcolor=(1,1,1))
#Plot the as a 3D scatter plot
mlab.points3d(x, y, z )
mlab.show()
