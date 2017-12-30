import numpy as np
import matplotlib.pyplot as plt
 
inputfile = open("Data.txt")
 
data = []
 
for line in inputfile:
    line_s = line.split()
    try:
        if len(line_s) > 1:
            value = float((line_s[-1]))
        else:
            value = float(line_s[0])
        data.append(value)
    except:
        pass

data[:] += np.abs(np.min(data[:]))
max_val = np.max(data[:])
offset = 0
mul = (255 - offset)/max_val
data = np.asarray([x * mul for x in data])
data = np.asarray([np.round(x) for x in data])
data[:] += offset
print(np.max(data))

outputfile = open('out.txt', 'w')
for x in data:
    outputfile.writelines(str(int(x)) + ",\n")
outputfile.close()

time = range(0, len(data), 1) 
plt.plot(time, data)    
plt.axis([0, time[-1] + 1, 0, 255])
plt.show()
