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
dataCa= np.empty((0,4), float)
dataNo= np.empty((0,4), float)
labelsCa=[]
labelsNo=[]

with open(file) as f:
	for line in f:
		pos = line.find("Result,br_ca")
		if pos != -1:
			spl = line.split(',')
			spl[5]=spl[5][:-2]
			labelsCa.append(spl[1][6:])
			dataCa = np.vstack((dataCa, np.array(spl[2:6]).astype(float))) 
		pos = line.find("Result,br_no")
		if pos != -1:
			spl = line.split(',')
			spl[5]=spl[5][:-2]
			labelsNo.append(spl[1][6:])
			dataNo = np.vstack((dataNo, np.array(spl[2:6]).astype(float))) 



#### Graph with general CA performance
x = np.arange(len(labelsCa))
width=0.2
fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x - width, dataCa[:,0], width, label='Average Avg')
rects2 = ax.bar(x, dataCa[:,1], width, label='Average Std')
rects3 = ax.bar(x + width, dataCa[:,2], width, label='Average Duty')
plt.ylim((0, 100))
ax.set_ylabel('Percentage %')
ax.set_xlabel('Number of Slots _ Beacon interval (ms)')
ax.set_title('Burst (CA) Performance metrics')
ax.set_xticks(x)
ax.set_xticklabels(labelsCa,rotation='vertical')
plt.tight_layout()
ax.legend()
#plt.show()
plt.savefig("../../graphs/br_ca_metrics.pdf",dpi=None)

################## NO COLLISION AVOIDANCE

x = np.arange(len(labelsNo))
width=0.2

fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x - width, dataNo[:,0], width, label='Average Avg')
rects2 = ax.bar(x, dataNo[:,1], width, label='Average Std')
rects3 = ax.bar(x + width, dataNo[:,2], width, label='Average Duty')

plt.ylim((0, 100))
ax.set_ylabel('Percentage %')
ax.set_xlabel('Number of Slots _ Beacon interval (ms)')
ax.set_title('Burst (no CA) Performance metrics')
ax.set_xticks(x)
ax.set_xticklabels(labelsNo,rotation='vertical')
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/br_no_metrics.pdf",dpi=None)


#### Performance comparison
fig,ax=plt.subplots()
width=0.3
rects1 = ax.bar(x-width/2, dataCa[:,3], width, label='CA')
rects2 = ax.bar(x+width/2, dataNo[:,3], width, label='No CA')

ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
ax.set_ylabel('Performance')
ax.set_xlabel('Number of Slots _ Beacon interval (ms)')
ax.set_title('Burst [CA/No CA] Computed Performance comparison')
ax.set_xticks(x)
ax.set_xticklabels(labelsNo,rotation='vertical')
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/br_ca-no_perf_comparison.pdf",dpi=None)


#### Metrics comparison
width=0.4

fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x-width/2, dataCa[:,0], width, label='CA avg')
rects2 = ax.bar(x+width/2, dataNo[:,0], width, label='No CA avg')
rects3 = ax.bar(x-width/2, dataCa[:,1], width, label='CA std dev')
rects4 = ax.bar(x+width/2, dataNo[:,1], width, label='No CA std dev')
ax.set_ylabel('Average Discovery Rate %')
ax.set_xlabel('Number of Slots _ Beacon interval (ms)')
ax.set_title('Burst [CA/No CA] Discovery Rate comparison')
ax.set_xticks(x)
ax.set_xticklabels(labelsNo,rotation='vertical')
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/br_ca-no_disc_comparison.pdf",dpi=None)

fig,ax=plt.subplots()
ax.grid(axis='y', linestyle='-', linewidth=0.4)
ax.set_axisbelow(True)
rects1 = ax.bar(x-width/2, dataCa[:,2], width, label='CA')
rects2 = ax.bar(x+width/2, dataNo[:,2], width, label='No CA')
ax.set_ylabel('Average Duty Cycle %')
ax.set_xlabel('Number of Slots _ Beacon interval (ms)')
ax.set_title('Burst [CA/No CA] Duty Cycle comparison')
ax.set_xticks(x)
ax.set_xticklabels(labelsNo,rotation='vertical')
ax.legend()
plt.tight_layout()
plt.savefig("../../graphs/br_ca-no_duty_comparison.pdf",dpi=None)
