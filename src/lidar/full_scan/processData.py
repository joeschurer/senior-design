import csv
import math
import random
import pandas as pd


# f = open('3dscan.csv','w')
# f.truncate()
# f.close()

read_file = pd.read_csv('../../../scans/3dscan.txt')
read_file.to_csv('3dscan.csv')
#f = open('/Users/collinhough/Unity/Senior Design/Assets/test_csv.csv', 'w')
f = open('3dscan.csv')

# Read and find x and y values
reader = csv.reader(f)
x_s = []
y_s = []
z_s = []
average_total_steps = 25600
to_meters = 1/100
offset = 0
for row in reader:
    # Read in parameters
    dist = float(row[1])
    step_count = float(row[2])
    step_count2 = float(row[3])
    direction = bool(row[4])

    # Check for overflow
    if step_count > 25600:
        continue

    # Calculate x, y, and z
    angle_sweep = (step_count/average_total_steps) * 360
    angle_elevation = ((step_count2/average_total_steps) * 360) + offset

    if direction == 0:
        angle_sweep = 360 - angle_sweep
    
    x = dist * math.sin(math.radians(angle_elevation)) * math.cos(math.radians(angle_sweep)) * to_meters
    y = dist * math.cos(math.radians(angle_elevation)) * to_meters
    z = dist * math.sin(math.radians(angle_sweep)) * math.sin(math.radians(angle_elevation)) * to_meters
    # Potential filtering
    # if x < 0.05 or y < 0.05 or z < 0.05:
    #     continue
    x_s.append(x)
    y_s.append(y)
    z_s.append(z)

# Write x and y values to file
f.close()

f = open('3dscan.csv','w')
f.truncate()
writer = csv.writer(f)
for i in range(len(x_s)):
    writer.writerow([x_s[i],y_s[i],z_s[i]])

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