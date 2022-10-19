import pandas as pd
import matplotlib.pyplot as plt

#Read the data from the csv file
df = pd.read_csv('scan.csv')
#print(df)
#Plot the data
plt.plot(df[df.columns[0]], df[df.columns[1]], 'ro')
plt.show()
