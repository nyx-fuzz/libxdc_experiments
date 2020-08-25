import json 
import numpy as np 
import matplotlib.pyplot as plt 
from matplotlib.patches import Patch
import glob

res = {}
for res_path in glob.glob("results/*.json"):
    with open(res_path) as f:
        res.update(json.load(f))

targets=["mruby", "unzip", "kafl", "foo", "kernel", "avscript32", "infiniteloop1", "qemu"]
tools = ["libxdc", "killerbeez", "PTrix", "honggfuzz", "WinAFL",  "libipt",]
names = ["{\\large\\textbf{\\textsc{libxdc}}}", "{\small killerbeez}", "{\small PTrix}", "{\small honggfuzz}", "{\small WinAFL}",  "{\small libipt}",]
dpi=160
fig = plt.figure(figsize=(10, 10), dpi=dpi)
#plt.rcParams.update({
#    "text.usetex": True,
#    "font.family": "sans-serif",
#    "font.sans-serif": ["Helvetica"]})
## for Palatino and other serif fonts use:
plt.rcParams.update({
    "text.usetex": True,
    "font.family": "serif",
    "font.serif": ["Palatino"],
})

fig.tight_layout()
ax = plt.axes()

for target_i in range(len(targets)):
    # first boxplot pair
    tdat = [res[targets[target_i]][t] for t in tools]
    fastest = min([sum(d)/float(len(d)) for d in tdat])
    tdat = [ [r/fastest for r in d] for d in tdat]
    offset =  0.1 + target_i*len(tools)*0.25
    for i in range(len(tools)):
        color = ["royalblue","ghostwhite","lightsteelblue","lightsteelblue", "royalblue", "royalblue"][i]
        avg = sum(tdat[i])/float(len(tdat[i]))
        mmin = abs(min(tdat[i])-avg)
        mmax = max(tdat[i]) -avg
        bp = plt.bar(offset+0.1+0.21*i, avg, yerr=[[mmin],[mmax]], width = 0.2, color=color, linewidth=0.5, edgecolor="black")


ax.set_yscale("log", nonposy='clip')

tick_lables = []
tick_pos = []
target_labels = []
target_pos = []
for target_i in range(len(targets)):
    tick_lables+=names
    offset =  0.1 + target_i*len(tools)*0.25
    tick_pos += [ offset+0.105+(0.21*i) for i in range(len(tools))]
    target_labels.append(targets[target_i])
    target_pos.append(offset+0.125*len(tools))
ax.set_xticklabels(tick_lables, rotation='vertical')
ax.set_xticks(tick_pos)
ax.set_xlabel('Decoder',fontsize=18)

axR = ax.twiny()
axR.set_xticks(target_pos)
axR.set_xticklabels(target_labels)
axR.set_xlim(ax.get_xlim())
axR.set_xlabel('Target', fontsize=18)

ticks = [1.0,1.2,1.4,1.6,1.8,2,3,4,5,6,7,8,9,10,20,30,40]
ax.set_yticklabels([str(t)+"x" for t in ticks])
ax.set_yticks(ticks)

ax.set_ylabel('Slowdown (lower is better)',fontsize=18)

legend_elements = [Patch(facecolor='royalblue', edgecolor='black', label='Full Edge Coverage'),
                   Patch(facecolor='lightsteelblue', edgecolor='black', label='Noisy Coverage'),
                   Patch(facecolor='ghostwhite', edgecolor='black', label='Only hash(path)')]

ax.legend(handles=legend_elements)

plt.savefig('eval.png', bbox_inches='tight' )
#plt.show()
