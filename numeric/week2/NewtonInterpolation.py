'''
    A python script for Newton Interpolation Method
'''

import numpy as np

def calcDifferenceQuotient(x, fx):
    assert len(x) == len(fx)
    N = len(x)
    dq = np.zeros((N, N))
    dq[0] = fx
    for l in range(1, N):
        for i in range(N):
            if i+l >= N: continue
            j = i+l
            dq[l][i] = (dq[l-1][i]-dq[l-1][i+1])/(x[i]-x[j])
            print 'dq[%s] = (%.3f-%.3f)/(%.3f-%.3f) = %.3f' % (', '.join(map(lambda x: 'x_%d' % x, range(i, i+l+1))), dq[l-1][i], dq[l-1][i+1], x[i], x[j], dq[l][i])
    return dq

def getNewtonInterpolation(x, dq, plot=False):
    N = dq.shape[0]
    print 'p(x) = %.3f' % dq[0][0]
    for i in range(0, N-1):
        print '\t+', ''.join(map(lambda y: '(x-%.3f)' % y, dq[0, :i+1])), '*', '%.3f'%dq[i+1][0]

    def p(_x):
        result = dq[0][0]
        if plot: print 'f(%.3f) = %.3f' % (_x, dq[0][0])
        for i in range(0, N-1):
            if plot: print '\t+', ''.join(map(lambda y: '(%.3f-%.3f)' % (_x, y), dq[0, :i+1])), '*', '%.3f'%dq[i+1][0]
            result += (_x-x[:i+1]).prod()*(dq[i+1][0])
        if plot: print '\t= %.6f' % result
        return result
    return p


class NewtonInterpolation(object):
    def __init__(self, x = [], fx = []):
        self.setData(x, fx)
    
    def setData(self, x = [], fx = []):
        self.x = np.array(x)
        self.fx = np.array(fx)
        assert len(x) == len(fx)
        self.N = len(x)

        # Calculate Difference Quotient
        self.dq = calcDifferenceQuotient(self.x, self.fx)

        # Calculate the interpolation equation
        self.p = getNewtonInterpolation(self.x, self.dq)

    def predict(self, x):
        return map(self.p, x)

    def drawAndSave(self, x0=None, xn=None, filename='NewtonInterpolationPlot.jpg', points=100, f=None):
        if not x0: x0 = self.x[0]
        if not xn: xn = self.x[-1]

        x = np.linspace(x0, xn, points)
        y = self.predict(x) 
        import matplotlib
        matplotlib.use('Agg')
        original_plot, = plt.plot(self.x, self.fx, '.', color='red', ms=13, label='Original points')
        legend = [p_plot, original_plot]
        if f:
            f_plot, = plt.plot(x, map(f, x), '-', lw=2, color='green', label='Original Function')
            legend.append(f_plot)

        plt.legend(handles=legend, loc='upper left')
        plt.savefig(filename)
        print 'Plot is saved in [%s]' % filename
            
if __name__ == '__main__':
    # data 1: sin
    x = [np.pi/6, np.pi/4, np.pi/3]
    fx = [1.0/2.0, 1.0/np.sqrt(2.0), np.sqrt(3.0)/2.0] 


    # data 2: random
    x = np.random.uniform(0, 1000, size=(10,))
    fx = np.random.uniform(0, 1000, size=(10,))
    x.sort()

    print '(x, y): ', zip(x, fx)

    ni = NewtonInterpolation(x, fx)

    print map(ni.p, x)
#    ni.drawAndSave(-np.pi/2, np.pi)
#    ni.drawAndSave(-np.pi/2, np.pi, f=np.sin)
    ni.drawAndSave()


       
