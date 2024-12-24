import sys


def fun(f, level):
    res = None
    for i in range(int(sys.argv[1])):
        _, ftime, utime, stime = f.readline(), f.readline(), f.readline(), f.readline()
        min, sec = utime.strip()[5:-1].split('m')
        s = int(min) * 60 + float(sec)
        if res is None or s < res:
            res = s
    print(level, s)

with open('tmp.txt') as f:
    fun(f, '-O0')
    fun(f, '-O1')
    fun(f, '-O2')
    fun(f, '-O3')
