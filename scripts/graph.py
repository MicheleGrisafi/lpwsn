import matplotlib.pyplot as plt 
import numpy as np


def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')


file="results.txt"
dataBrCa= np.empty((0,4), float)
dataBrNo= np.empty((0,4), float)
dataScNo= np.empty((0,4), float)
dataScCa= np.empty((0,4), float)
labelsBrCa=[]
labelsBrNo=[]
labelsScNo=[]
labelsBrCaComplete=[]
labelsBrNoComplete=[]
labelsScNoComplete=[]

with open(file) as f:
	for line in f:
		pos = line.find("Result,br_ca")
		if pos != -1:
			spl = line.split(',')
			spl[5]=spl[5][:-2]
			labelsBrCa.append(spl[1][6:])
			labelsBrCaComplete.append(spl[1])
			dataBrCa = np.vstack((dataBrCa, np.array(spl[2:6]).astype(float))) 
		pos = line.find("Result,br_no")
		if pos != -1:
			spl = line.split(',')
			spl[5]=spl[5][:-2]
			labelsBrNo.append(spl[1][6:])
			labelsBrNoComplete.append(spl[1])
			dataBrNo = np.vstack((dataBrNo, np.array(spl[2:6]).astype(float))) 
		pos = line.find("Result,sc_no")
		if pos != -1:
			spl = line.split(',')
			spl[5]=spl[5][:-2]
			labelsScNo.append(spl[1][6:])
			labelsScNoComplete.append(spl[1])
			dataScNo = np.vstack((dataScNo, np.array(spl[2:6]).astype(float))) 
		""" pos = line.find("Result,sc_ca")
		if pos != -1:
			spl = line.split(',')
			spl[5]=spl[5][:-2]
			labelsScCa.append(spl[1][6:])
			dataScCa = np.vstack((dataScCa, np.array(spl[2:6]).astype(float)))  """



#### Graph with general CA performance
x = np.arange(len(labelsBrCa))
width=0.2
fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x - width, dataBrCa[:,0], width, label='Average Avg')
rects2 = ax.bar(x, dataBrCa[:,1], width, label='Average Std')
rects3 = ax.bar(x + width, dataBrCa[:,2], width, label='Average Duty')
plt.ylim((0, 100))
ax.set_ylabel('Percentage %')
ax.set_xlabel('Slots per epoch _ Beacon interval (ms)')
ax.set_title('Burst (CA) Performance metrics')
ax.set_xticks(x)
ax.set_xticklabels(labelsBrCa,rotation=70)
plt.tight_layout()
ax.legend()
#plt.show()
plt.savefig("../../graphs/br_ca_metrics.pdf",dpi=None)

""" x = np.arange(len(labelsScCa))
width=0.2
fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x - width, dataScCa[:,0], width, label='Average Avg')
rects2 = ax.bar(x, dataScCa[:,1], width, label='Average Std')
rects3 = ax.bar(x + width, dataScCa[:,2], width, label='Average Duty')
plt.ylim((0, 100))
ax.set_ylabel('Percentage %')
ax.set_xlabel('Slots per epoch')
ax.set_title('Scatter (CA) Performance metrics')
ax.set_xticks(x)
ax.set_xticklabels(labelsScCa,rotation=70)
plt.tight_layout()
ax.legend()
#plt.show()
plt.savefig("../../graphs/sc_ca_metrics.pdf",dpi=None) """

################## NO COLLISION AVOIDANCE

x = np.arange(len(labelsBrNo))
width=0.2

fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x - width, dataBrNo[:,0], width, label='Average Avg')
rects2 = ax.bar(x, dataBrNo[:,1], width, label='Average Std')
rects3 = ax.bar(x + width, dataBrNo[:,2], width, label='Average Duty')

plt.ylim((0, 100))
ax.set_ylabel('Percentage %')
ax.set_xlabel('Slots per epoch _ Beacon interval (ms)')
ax.set_title('Burst (no CA) Performance metrics')
ax.set_xticks(x)
ax.set_xticklabels(labelsBrNo,rotation=70)
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/br_no_metrics.pdf",dpi=None)


x = np.arange(len(labelsScNo))
width=0.2

fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x - width, dataScNo[:,0], width, label='Average Avg')
rects2 = ax.bar(x, dataScNo[:,1], width, label='Average Std')
rects3 = ax.bar(x + width, dataScNo[:,2], width, label='Average Duty')


plt.ylim((0, 100))
ax.set_ylabel('Percentage %')
ax.set_xlabel('Slots per epoch')
ax.set_title('Scatter (no CA) Performance metrics')
ax.set_xticks(x)
ax.set_xticklabels(labelsScNo,rotation=70)
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/sc_no_metrics.pdf",dpi=None)

#### Performance comparison between Burst Ca and No Ca
x = np.arange(len(labelsBrCa))
fig,ax=plt.subplots()
width=0.3
rects1 = ax.bar(x-width/2, dataBrCa[:,3], width, label='CA')
rects2 = ax.bar(x+width/2, dataBrNo[:,3], width, label='No CA')

ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
ax.set_ylabel('Performance')
ax.set_xlabel('Slots per epoch _ Beacon interval (ms)')
ax.set_title('Burst [CA/No CA] Computed Performance comparison')
ax.set_xticks(x)
ax.set_xticklabels(labelsBrNo,rotation=70)
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/br_ca-no_perf_comparison.pdf",dpi=None)

#### Performance scatter
x = np.arange(len(labelsScNo))
fig,ax=plt.subplots()
width=0.3
rects1 = ax.bar(x, dataScNo[:,3], width)

ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
ax.set_ylabel('Performance')
ax.set_xlabel('Slots per epoch')
ax.set_title('Scatter Computed Performance comparison')
ax.set_xticks(x)
ax.set_xticklabels(labelsScNo,rotation=70)
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/sc_no_perf.pdf",dpi=None)

#### Metrics comparison between Burst Ca and No Ca
x = np.arange(len(labelsBrCa))
width=0.4

fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x-width/2, dataBrCa[:,0], width, label='CA avg')
rects2 = ax.bar(x+width/2, dataBrNo[:,0], width, label='No CA avg')
rects3 = ax.bar(x-width/2, dataBrCa[:,1], width, label='CA std dev')
rects4 = ax.bar(x+width/2, dataBrNo[:,1], width, label='No CA std dev')
ax.set_ylabel('Average Discovered Nodes %')
ax.set_xlabel('Slots per epoch _ Beacon interval (ms)')
ax.set_title('Burst [CA/No CA] Discovery Rate comparison')
ax.set_xticks(x)
ax.set_xticklabels(labelsBrNo,rotation=70)
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/br_ca-no_disc_comparison.pdf",dpi=None)

x = np.arange(len(labelsBrCa))
fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x-width/2, dataBrCa[:,2], width, label='CA')
rects2 = ax.bar(x+width/2, dataBrNo[:,2], width, label='No CA')
ax.set_ylabel('Average Duty Cycle %')
ax.set_xlabel('Slots per epoch _ Beacon interval (ms)')
ax.set_title('Burst [CA/No CA] Duty Cycle comparison')
ax.set_xticks(x)
ax.set_xticklabels(labelsBrNo,rotation=70)
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/br_ca-no_duty_comparison.pdf",dpi=None)


########### COMPARISON  between Scatter and Burst
x = np.arange(len(labelsBrCaComplete)+len(labelsBrNoComplete)+len(labelsScNoComplete))
labels = labelsBrCaComplete + labelsBrNoComplete + labelsScNoComplete
width=0.2

fig = plt.figure(figsize=(13, 5))
ax = fig.add_subplot(111)
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)

avg = np.concatenate((dataBrCa[:,0], dataBrNo[:,0],dataScNo[:,0]))
std = np.concatenate((dataBrCa[:,1], dataBrNo[:,1],dataScNo[:,1]))
duty = np.concatenate((dataBrCa[:,2], dataBrNo[:,2],dataScNo[:,2]))

rects1 = ax.bar(x - width, avg, width, label='Discovery Avg')
rects2 = ax.bar(x, std, width, label='Discovery Std')
rects3 = ax.bar(x+width, duty, width, label='Duty Avg')

ax.set_ylabel('Average  %')
ax.set_xlabel('Primitive _ CA/NO _ Slots per epoch _ Beacon interval (ms)')
ax.set_title('Comparison between the various performance metrics among all of the tested primitives')
ax.set_xticks(x)
ax.set_xticklabels(labels,rotation=70)
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/all_metrics_comparison.pdf",dpi=None)



x = np.arange(len(labelsBrCaComplete)+len(labelsBrNoComplete)+len(labelsScNoComplete))
labels = labelsBrCaComplete + labelsBrNoComplete + labelsScNoComplete
width=0.7

fig = plt.figure(figsize=(13, 5))
ax = fig.add_subplot(111)
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)

perf = np.concatenate((dataBrCa[:,3], dataBrNo[:,3],dataScNo[:,3]))

rects1 = ax.bar(x, perf, width)

ax.set_ylabel('Performance')
ax.set_xlabel('Primitive _ CA/NO _ Slots per epoch _ Beacon interval (ms)')
ax.set_title('Comparison of computed performance among all of the tested primitives')
ax.set_xticks(x)
ax.set_xticklabels(labels,rotation=70)
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/all_perf_comparison.pdf",dpi=None)

