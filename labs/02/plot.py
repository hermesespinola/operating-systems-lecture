#!/usr/bin/python

from matplotlib import pyplot as plt
plt.xkcd()

names = []
vals = []
f = open('stats.dat', 'r')
for line in f:
    name, val = line.split(' ')
    print name, val
    vals.append(int(val))
    names.append(name)
f.close()

plt.title("Number of files by directory")
plt.bar(range(len(vals)), vals)
plt.xticks(range(len(vals)), names)
plt.show()
