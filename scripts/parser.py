#This Python script is used to parse the logs generated by cooja

import matplotlib.pyplot as plt 
import numpy as np
import sys

if len(sys.argv) < 2:
    print("Syntax error: ./{} discoveryLog dutyCycleLog".format(sys.argv[0]))
    sys.exit(1)

# Get beginning and end of snort
discoveryLog = sys.argv[1]
dutyLog = sys.argv[2]

#Load the data
with open(discoveryLog) as f: 
    lines = [line.rstrip('\n') for line in f]

for i in enumerate(lines):
    if(.find(sub,start,end) != -1)

#data[0,0] = 0

#plt.plot(data[:,0],data[:,1])
#plt.axvline(x=int(start), color='r', linestyle='--')
#plt.axvline(x=int(stop), color='r', linestyle='--')

#plt.savefig(sys.argv[2])