import numpy as np 
from abc import ABCMeta, abstractmethod

# Neural network super class
class Net(metaclass=ABCMeta):
    @abstractmethod
    def __init__(self):
        pass

    @abstractmethod
    def forward(self, X):
        pass

    @abstractmethod
    def backward(self, dout):
        pass

    @abstractmethod
    def get_params(self):
        pass

    @abstractmethod
    def set_params(self, params):
        pass

def NLLLoss(Y_pred, Y_true):
    """
    Negative log likelihood loss
    """
    loss = 0.0
    N = Y_pred.shape[0]
    M = np.sum(Y_pred*Y_true, axis=1)
    for e in M:
        if e == 0:
            loss += 500
        else:
            loss += -np.log(e)
    return loss/N

class SGD():
    def __init__(self, params, lr=0.001, reg=0):
        self.parameters = params
        self.lr = lr
        self.reg = reg

    def step(self):
        for param in self.parameters:
            param['val'] -= (self.lr*param['grad'] + self.reg*param['val'])

def get_batch(X,Y,batch_size,batch_ind):
	X_batch = X[batch_ind*batch_size:(batch_ind+1)*batch_size]
	Y_batch = Y[batch_ind*batch_size:(batch_ind+1)*batch_size]
	return X_batch, Y_batch
