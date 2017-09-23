#!/usr/bin/python
from matplotlib import pyplot as plt
import csv

def plot(reader):
    line = reader.next()
    plt.title("Parallel pi")
    plt.xlabel(line[0])
    plt.ylabel(line[2])

    points = []
    times = []

    line = next(reader, None)
    while line:
        num_points, pi, time = line
        points.append(num_points)
        times.append(time)
        line = next(reader, None)

    plt.plot(points, times)

if __name__ == '__main__':
    plot(csv.reader(open('par_times.csv')))
    plt.savefig('parallel_pi.png')
    plt.show()
