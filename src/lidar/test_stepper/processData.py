import csv
import math
import random
import pandas as pd


f = open('1dscan.csv','w')
f.truncate()
f.close()

read_file = pd.read_csv('../../../scans/2Dscan.txt')
read_file.to_csv('1dscan.csv')
#f = open('/Users/collinhough/Unity/Senior Design/Assets/test_csv.csv', 'w')
f = open('1dscan.csv')

# Read and find x and y values
reader = csv.reader(f)
x_s = []
y_s = []
average_total_steps = 25600
to_meters = 1/100
for row in reader:
    dist = float(row[1])
    step_count = float(row[2])
    angle = (step_count/average_total_steps) * 360
    x_s.append((dist*math.cos(math.radians(angle)) * to_meters))
    y_s.append((dist*math.sin(math.radians(angle)) * to_meters))

# Write x and y values to file
f.close()

f = open('1dscan.csv','w')
f.truncate()
writer = csv.writer(f)
for i in range(len(x_s)):
    writer.writerow([x_s[i],y_s[i],0])

# Create 2-D circle of radius r
# r = 5
# n = 100
# points = [(math.cos(2*math.pi/n*x)*r,math.sin(2*math.pi/n*x)*r) for x in range(0,n+1)]
# for i in range(100):
#     x = points[i][0]
#     y = points[i][1]
#     writer.writerow([x,y,0])

# Create random point cloud
# for i in range(100000):
#     writer.writerow([random.uniform(-20,20),
#                         random.uniform(-20,20),
#                         random.uniform(-20,20)])

f.close()