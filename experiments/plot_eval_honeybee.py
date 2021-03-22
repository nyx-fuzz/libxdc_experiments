import json 
import re
import numpy as np 
import matplotlib.pyplot as plt 
from matplotlib.patches import Patch
import glob

plot_cold = False

res = {}
for res_path in glob.glob("results_honeybee/*.json"):
    with open(res_path) as f:
        res.update(json.load(f))


targets=[
"contrived_small_trace_1",
"contrived_small_trace_2_1",
"contrived_small_trace_2_2",
"contrived_small_trace_2_3",
"contrived_medium_trace_1",
"contrived_medium_trace_2_1",
"clang_compile_simple_c_1",
"clang_compile_simple_c_2",
"honey_mirror_1_bash",
"honey_mirror_1_clang_huge",
"contrived_medium_trace_2_2",
"contrived_medium_trace_2_3",
"contrived_medium_trace_2_4",
"html_fast_parse_6_txt",
"ssh_interactive_login_attempt_overflow",
"tar_decompress_clion",
"tar_help_page",
]

tools = ["libxdc", "honeybee" ]
if plot_cold:
    names = ["{\\large\\textbf{\\textsc{libxdc (Cold)}}}", "{\small Honeybee (Cold)}"]
else:
    names = ["{\\large\\textbf{\\textsc{libxdc}}}", "{\small Honeybee}"]
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
    cur = res[targets[target_i]+".pt"]
    if plot_cold:
        tdat = [cur["libxdc"]["cold"], cur["honeybee"]["cold"] ]
    else:
        tdat = [cur["libxdc"]["avg"], cur["honeybee"]["avg"] ]
    fastest = min([sum(d)/float(len(d)) for d in tdat])
    tdat = [ [r/fastest for r in d] for d in tdat]
    offset =  0.1 + target_i*len(tools)*0.25
    for i in range(len(tools)):
        color = ["royalblue","lightsteelblue"][i]
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
    tick_lables+=["" for n in names]
    offset =  0.1 + target_i*len(tools)*0.25
    tick_pos += [ offset+0.105+(0.21*i) for i in range(len(tools))]
    target_labels.append(re.sub("_", "\\_", targets[target_i][:15]))
    target_pos.append(offset+0.125*len(tools))
ax.set_xticklabels(tick_lables, rotation='vertical')
ax.set_xticks(tick_pos)
ax.set_xlabel('Decoder',fontsize=18)

axR = ax.twiny()
axR.set_xticks(target_pos)
axR.set_xticklabels(target_labels, rotation=45)
axR.set_xlim(ax.get_xlim())
if plot_cold:
    axR.set_xlabel('Target (Cold)', fontsize=18)
else:
    axR.set_xlabel('Target (Avg on 25 Traces)', fontsize=18)

if plot_cold:
    ticks = [1.0,1.2,1.4,1.6,1.8,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,200]
else:
    ticks = [1.0,1.2,1.4,1.6,1.8,2,3,4,5]
ax.set_yticklabels([str(t)+"x" for t in ticks])
ax.set_yticks(ticks)

ax.set_ylabel('Slowdown (lower is better)',fontsize=18)

legend_elements = [Patch(facecolor='royalblue', edgecolor='black', label='libxdc'),
                   Patch(facecolor='lightsteelblue', edgecolor='black', label='honeybee'),
                   ]

ax.legend(handles=legend_elements)

if plot_cold:
    plt.savefig('eval_honeybee_cold.png', bbox_inches='tight' )
else:
    plt.savefig('eval_honeybee.png', bbox_inches='tight' )

#plt.show()
