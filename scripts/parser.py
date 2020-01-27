#This Python script is used to parse the logs generated by cooja

import matplotlib.pyplot as plt 
import numpy as np
import sys
from statistics import mean



if len(sys.argv) < 2:
    print("Syntax error: ./{} discoveryLog dutyCycleLog".format(sys.argv[0]))
    sys.exit(1)

# Get beginning and end of snort
discoveryLog = sys.argv[1]
dutyLog = sys.argv[2]

#Load the data
discRates = dict()
with open(discoveryLog) as f: 
    for line in f:
        pos = line.find("finished Num NBR")
        if(pos != -1):
            epoch = int(line[line.find("Epoch")+5:line.find(" finished")])
            nbr = int(line[pos+17:len(line)])
            node = int(line[line.find("ID:")+3:line.find("App:")-1])
            if not discRates.get(epoch,0):
                discRates[epoch] = []
            discRates[epoch].insert(node,nbr)
for epoch in discRates.items():
	print("{}/{}".format(mean(epoch[1]),len(epoch[1])))
