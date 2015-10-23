import os

if __name__ == '__main__':
    print 'Compiling...'
    os.popen('g++ ./M-C.cpp --std=c++11 -o M-C')

    print 'Running...'
    for N, K in [(5, 3), (10, 5), (100, 5), (1000, 5), (10000, 5), (30000, 10)]:
        print N, K
        res = os.popen('./M-C %d %d' % (N, K))
        print res.readlines()
