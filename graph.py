import os
import matplotlib.pyplot as plt
import re

fname = './thread_non_cached.out'

fname2 = './thread.out'

with open(fname,'r') as f:
    ogdata = f.read()
speed = [float(p) for p in re.findall('Speed.* (.*\d)',ogdata)]

with open(fname2,'r') as f2:
    ogdata2 = f2.read()
speed2 = [float(p) for p in re.findall('Speed.* (.*\d)',ogdata2)]

def formatter(x, pos):
    return str(round(x / 1e9, 1)) + "B"

from matplotlib.ticker import NullFormatter

fig,ax = plt.subplots(figsize=(14,8))
# plt.plot(range(1,33),speed2,'-o',markersize=4,label='Cached')
plt.plot(range(1,33),speed,'-o',markersize=4,label='Threaded non cached')
plt.plot(range(1,33),speed2,'-o',markersize=4,label='Threaded cached')
# ax.yaxis.get_major_formatter().set_scientific(False)
# ax.yaxis.get_major_formatter().set_useOffset(False)
# ax.yaxis.set_major_formatter(formatter)
# ax.yaxis.set_minor_formatter(NullFormatter())
plt.legend()
# plt.yscale("symlog")
plt.title('Speed vs number of threads')
plt.xlabel('Number of threads')
plt.ylabel('Speed in MB/s')
plt.yticks(range(100,11111,1000))
plt.xticks(range(1,33,2))
plt.show()
# plt.savefig('cached_vs_noncached.png')