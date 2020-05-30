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


x = np.arange(len(labelsCa))
width=0.2

fig,ax=plt.subplots()

rects1 = ax.bar(x - width, dataCa[:,0], width, label='Average Avg')
rects2 = ax.bar(x, dataCa[:,1], width, label='Average Std')
rects3 = ax.bar(x + width, dataCa[:,2], width, label='Average Duty')
plt.ylim((0, 100))
ax.set_ylabel('Percentage %')
ax.set_ylabel('SlotsNumber _ Beacon interval')
ax.set_title('Burst (CA) Performance')
ax.set_xticks(x)
ax.set_xticklabels(labelsCa,rotation='vertical')
ax.legend()


#autolabel(rects1)
#autolabel(rects2)
#autolabel(rects3)
#fig.tight_layout()

plt.show()


fig,ax=plt.subplots()
width=0.6
rects1 = ax.bar(x, dataCa[:,3], width, label='Perf')


ax.set_ylabel('Performance')
ax.set_title('Performance of the various combinations')
ax.set_xticks(x)
ax.set_xticklabels(labels,rotation='vertical')
ax.legend()

plt.show()

################## NO COLLISION AVOIDANCE

x = np.arange(len(labelsNo))
width=0.2

fig,ax=plt.subplots()

rects1 = ax.bar(x - width, dataNo[:,0], width, label='Average Avg')
rects2 = ax.bar(x, dataNo[:,1], width, label='Average Std')
rects3 = ax.bar(x + width, dataNo[:,2], width, label='Average Duty')

plt.ylim((0, 100))
ax.set_ylabel('Percentage %')
ax.set_title('Results for various combinations')
ax.set_xticks(x)
ax.set_xticklabels(labelsNo,rotation='vertical')
ax.legend()


#autolabel(rects1)
#autolabel(rects2)
#autolabel(rects3)
#fig.tight_layout()

plt.show()


fig,ax=plt.subplots()
width=0.6
rects1 = ax.bar(x, dataNo[:,3], width, label='Perf')

ax.set_ylabel('Performance')
ax.set_title('Performance of the various combinations')
ax.set_xticks(x)
ax.set_xticklabels(labels,rotation='vertical')
ax.legend()

plt.show()
