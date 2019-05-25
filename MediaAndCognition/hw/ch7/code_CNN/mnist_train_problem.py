'''
Replace COMPLETE with your code
'''
import pickle
import numpy as np 
import matplotlib.pyplot as plt 
import random
from utils import Net, NLLLoss, SGD, get_batch

'''
(1) Load MNIST Dataset
'''
## load dataset
def load():
    with open("mnist.pkl",'rb') as f:
        mnist = pickle.load(f)
    return mnist['training_images'], mnist['training_labels'], mnist['test_images'], mnist['test_labels']
# mnist['training_images'].shape=(60000,784) numpy.ndarray
# mnist['training_labels'].shape=(60000,) numpy.ndarray
# mnist['test_images'].shape=(10000,784) numpy.ndarray
# mnist['test_labels'].shape=(10000,) numpy.ndarray

X_train, Y_train, X_test, Y_test = load()
print('The shape of training data is ', X_train.shape)
print('The shape of training label is ', Y_train.shape)
print('The shape of testing data is ', X_test.shape)
print('The shape of testing label is ', Y_test.shape)

## show the 1st image and label
def show(x,y,K):
    plt.figure("Image")
    plt.imshow(np.reshape(x[K,:],[28,28]))
    plt.title(['label:', y[K]])
    plt.show()

show(X_train,Y_train,0)

## normalize images
def normalize(x):
    print('The range of data is [%d,%d]' % (x.min(), x.max()))
    print('The type of data is %s' % (x.dtype))
    # COMPLETE
    # rearrange x to [-1,1]
    x = (x-x.min()) / x.max()*2 - 1
    print('The range of data is [%f,%f]' % (x.min(), x.max()))
    print('The type of data is %s' % (x.dtype))
    return x

X_train = normalize(X_train)
X_test = normalize(X_test)

## convert label to one-hot codes
D_in = X_train.shape[-1] # 784
D_out = 10
def label2OH(y,D_out):
    N = y.shape[0]
    OH = np.zeros((N,D_out))
    OH[np.arange(N), y] = 1
    return OH

def OH2label(OH):
    y = np.argmax(OH, axis=1)
    return y

OH_train = label2OH(Y_train,D_out)
OH_test = label2OH(Y_test,D_out)

## check whether there is sth. wrong with OH
if not OH_train[0,Y_train[0]] or OH2label(OH_train)[0] != Y_train[0]:
    print('Something wrong with OH[0]!')

'''
(2) Construct Model
'''
## ReLU activation layer
class ReLU():
    def __init__(self):
        self.input = None

    def _forward(self, x):
        out = np.maximum(0,x)
        self.input = x
        return out

    def _backward(self, d):
        dX = d*((self.input)>0)

        return dX

## check implementation of ReLU by numerial approximation
delta =1e-6
x = np.random.randn(8,10)
e = np.random.randn(8,10)
relu_test = ReLU()
y = relu_test._forward(x)
d = relu_test._backward(e)
d_approx = (relu_test._forward(x+delta*e) - relu_test._forward(x)) / delta
error = np.abs(d - d_approx).mean() / np.abs(d_approx).mean()
print('ReLU error: ', error)

## Softmax activation layer
class Softmax():
    def __init__(self):
    	self.input = None
    	self.output = None

    def _forward(self, X):
        Y = np.exp(X - np.tile(X.max(axis=1).reshape([X.shape[0],1]), X.shape[1]))
        Z = Y / np.tile(np.sum(Y, axis=1).reshape([Y.shape[0],1]), Y.shape[1])
        self.input = X
        self.output = Z
        return Z # distribution

    def _backward(self, dout):
        X = self.input
        Z = self.output
        dX = np.zeros(X.shape)
        N = Z.shape[0]
        for n in range(N):
            J = - np.dot(Z[n,:].reshape([Z.shape[1],1]), Z[n,:].reshape([1,Z.shape[1]])) + np.diag(Z[n,:])
            dX[n,:] = np.dot(J,dout[n,:])
        return dX

## check implementation of Softmax by numerial approximation
delta =1e-6
x = np.random.randn(8,10)
e = np.random.randn(8,10)
sm_test = Softmax()
y = sm_test._forward(x)
d = sm_test._backward(e)
d_approx = (sm_test._forward(x+delta*e) - sm_test._forward(x)) / delta
error = np.abs(d - d_approx).mean() / np.abs(d_approx).mean()
print('Softmax error: ', error)

## Fully connected layer
class FC():
    def __init__(self, D_in, D_out):
        self.input = None
        self.W = {'val': np.random.normal(0.0, np.sqrt(2/D_in), (D_in,D_out)), 'grad': 0}
        self.b = {'val': np.random.randn(D_out), 'grad': 0}

    def _forward(self, X):
        out = np.dot(X, self.W['val']) + self.b['val']
        self.input = X
        return out

    def _backward(self, dout):
        X = self.input
        dX = np.dot(dout, self.W['val'].T).reshape(X.shape)
        self.W['grad'] = np.dot(np.sum(X,axis=1).T / X.shape[0], dout)
        self.b['grad'] = dout.mean(axis=0)
        return dX

## Simple 2 layer NN
class SimpleNet(Net):
    def __init__(self, D_in, H, D_out, weights=''):
        self.FC1 = FC(D_in, H)
        self.ReLU1 = ReLU()
        self.FC2 = FC(H, D_out)

        if weights == '':
            pass
        else:
            with open(weights,'rb') as f:
                params = pickle.load(f)
                self.set_params(params)

    def forward(self, X):
        h1 = self.FC1._forward(X)
        a1 = self.ReLU1._forward(h1)
        h2 = self.FC2._forward(a1)
        return h2

    def backward(self, dout):
        dout = self.FC2._backward(dout)
        dout = self.ReLU1._backward(dout)
        dout = self.FC1._backward(dout)

    def get_params(self):
        return [self.FC1.W, self.FC1.b, self.FC2.W, self.FC2.b]

    def set_params(self, params):
        [self.FC1.W, self.FC1.b, self.FC2.W, self.FC2.b] = params

## Cross Entropy Loss
class CrossEntropyLoss():
    def __init__(self):
        pass

    def get(self, Y_pred, Y_true):
        N = Y_pred.shape[0]
        softmax = Softmax()
        prob = softmax._forward(Y_pred)
        loss = NLLLoss(prob, Y_true)
        Y_serial = np.argmax(Y_true, axis=1)
        dout = prob.copy()
        dout[np.arange(N), Y_serial] -= 1
        return loss, dout

## build simple model
H = 300
model = SimpleNet(D_in, H, D_out)

'''
(3) Training
'''
## Evaluation
def evaluation(model,X,Y):
    pred = model.forward(X)
    result = OH2label(pred) == Y
    result = list(result)
    return result.count(1), X.shape[0]

optim = SGD(model.get_params(), lr=0.0001, reg=0.00003)
criterion = CrossEntropyLoss()

batch_size = 64
EPOCH = 5
BATCH = int(X_train.shape[0] / batch_size)
for e in range(EPOCH):
    ## shuffle data
    index = [i for i in range(X_train.shape[0])]
    random.shuffle(index)
    X_train = X_train[index,:]
    Y_train = Y_train[index]

    for b in range(BATCH):
        ## get batch, covert to one-hot
        X_batch, Y_batch = get_batch(X_train, Y_train, batch_size, b)
        OH_batch = label2OH(Y_batch, D_out)

        ## forward, loss, backward, update weights
        pred = model.forward(X_batch)
        loss, dout = criterion.get(pred, OH_batch)
        model.backward(dout)
        optim.step()

    print("EPOCH %d" % (e))
    # TRAIN SET ACC
    correct_num, total_num = evaluation(model,X_train,Y_train)
    print("TRAIN SET ACC: " + str(correct_num) + " / " + str(total_num) + " = " + str(correct_num/total_num))

    # TEST SET ACC
    correct_num, total_num = evaluation(model,X_test,Y_test)
    print("TEST SET ACC: " + str(correct_num) + " / " + str(total_num) + " = " + str(correct_num/total_num))


## save weights
weights = model.get_params()
with open("weights.pkl","wb") as f:
    pickle.dump(weights, f)

## load weights and test
model = SimpleNet(D_in, H, D_out, "weights.pkl")
print("FINAL TEST")
# TRAIN SET ACC
correct_num, total_num = evaluation(model,X_train,Y_train)
print("TRAIN SET ACC: " + str(correct_num) + " / " + str(total_num) + " = " + str(correct_num/total_num))

# TEST SET ACC
correct_num, total_num = evaluation(model,X_test,Y_test)
print("TEST SET ACC: " + str(correct_num) + " / " + str(total_num) + " = " + str(correct_num/total_num))
