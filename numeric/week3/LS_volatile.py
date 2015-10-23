import numpy as np
import numpy.linalg as linalg
import matplotlib 
matplotlib.use('Agg')
import matplotlib.pyplot as plt

from scipy.special import legendre

def getG(bases, x):
    G = np.zeros((x.shape[0], len(bases)))
    for ind, ele in enumerate(bases):
        G[:, ind] = ele(x)
    return G

def lncond(G):
    return np.log(linalg.cond(np.dot(G.T, G)))

def predict(bases, a, x):
    y = np.zeros_like(x)
    for f, coef in zip(bases, a):
        y += coef*f(x)
    return y

def lab2():
    plots = []
    errors = []
    errors_pos = []
    for l in [3,5,7,9,11,13,15,17,19]: #[(3, 'red'), (5, 'black'), (7, 'blue'), (9, 'green')]:
        # 1, x, x**2, ..., x**l
        polynomial_bases = map(lambda k: (lambda x: x ** k), range(l+1))

        plotx = []
        ploty = []
        for n in range(l+1, 200, 1):
            x = np.linspace(-1, 1, n)
            y = np.exp(x)
            G = getG(polynomial_bases, x)

            a = np.dot(np.dot(linalg.inv(np.dot(G.T, G)), G.T), y)
            yhat = predict(polynomial_bases, a, x)
            error = ((yhat-y)**2).sum()
            errors.append(error)

            # print 'ln(cond(A)) =', lncond(polynomial_bases, x)
            lncondG = lncond(G)
            plotx.append(n)
            ploty.append(lncondG)
            errors_pos.append((n, lncondG))

        plot, = plt.plot(plotx, ploty, '-', label='l=%d'%l)
        plots.append(plot)

    plt.legend(handles=plots)
    print x

    # display errors as marker size
    # map errors into [4, 12]
    # print zip(np.array(errors_pos)[errors.argsort()], errors[errors.argsort()])
    errors = np.array(errors)
    errors = np.log2(errors)
    errors = errors - errors.min()
    errors = errors/errors.max()*20 + 4
    for error, (x, y) in zip(errors, errors_pos):
        plt.plot(x, y, '.', color='black', ms = error, alpha=0.7)



    plt.xlabel('n')
    plt.ylabel('ln(cond(A))')
    plt.savefig('polynomial-ln(cond(A))-n.jpg')

    plt.clf()


def labmain():
    def polynomial_bases(l):
        return map(lambda k: (lambda x: x ** k), range(l+1))
    def legendre_bases(l):
        return map(legendre, range(l+1))

    n = 200
    x = np.linspace(-1, 1, n)
    y = np.exp(x)
    plots = []
    for basef in [polynomial_bases, legendre_bases]:
        plotx = []
        ploty = []
        for l in range(5, 300, 5):
            print l
            base = basef(l)
            G = getG(base, x)

            a = np.dot(np.dot(linalg.inv(np.dot(G.T, G)), G.T), y)
            yhat = predict(base, a, x)
            error = ((yhat-y)**2).sum()

            lncondG = lncond(G)
            plotx.append(l)
            ploty.append(lncondG)
            plt.text(l, lncondG,'')#, 'l=%d, '%l+r'$\sigma$'+'=%.2e'%error)

        plot, = plt.plot(plotx, ploty, '.-', ms=10, label=basef.__name__)
        plots.append(plot)

    plt.legend(handles=plots, loc='upper left')
#   plt.xlim(1, 25)
#   plt.ylim(0, 43)
    plt.xlabel('l')
    plt.ylabel('ln(cond(A))')
    plt.title('ln(cond(A))-l')
    plt.savefig('ln(cond(A))-l.jpg')

#    plt.title('polynomial-ln(cond(A))-l')
#    plt.savefig('polynomial-ln(cond(A))-l.jpg')
#    plt.title('legendre-ln(cond(A))-l')
#    plt.savefig('legendre-ln(cond(A))-l.jpg')




if __name__ == '__main__':
    labmain()
 






