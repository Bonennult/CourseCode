############################################################
# Media and Cognition 2019: Homework Chapter 2
# V2.0 2019-03-06
############################################################

student_ID = "Dylan"

############################################################
# Imports
############################################################

import matplotlib.pyplot as plt
from matplotlib.pyplot import plot

# Include your imports here, if any are used.

import numpy as np

############################################################
# Perceptron
############################################################

class BinaryPerceptron(object):

    def __init__(self):
        #Type your codes here
        self.w = np.zeros([2])  # class = np.array
        self.b = np.zeros([1])
        self.r = 1

    def learn(self, X, Y, max_iterations):
        #Type your codes here
        for iter in range(max_iterations):
            for i in range(len(X)):
                y_pred = self.predict(X[i])
                if np.dot(Y[i], y_pred) <= 0:
                    self.w = self.w + np.dot(self.r*Y[i], X[i])
                    self.b = self.b + self.r*Y[i]

    def predict(self, X):
        #Type your codes here
        #print(np.sign(np.matmul(X, np.transpose(self.w)) + self.b))
        return np.sign(np.matmul(X, np.transpose(self.w)) + self.b)

    def plotimg(self, X , Y=None):
        #Type your codes here
        #Plot samples X
        #Plot the decision boundary according to w*x+b=0
        X = np.array(X)
        plot(X[:,0], (-self.b-np.dot(self.w[0],X[:,0]))/self.w[1])
        if Y is None:
            Y = self.predict(X)
            print('\n    X0    X1     Y')
            print(np.concatenate((X,Y[:,np.newaxis]),axis=1))
        else:
            X_pos = X[np.where(np.array(Y)>=0)]
            X_neg = X[np.where(np.array(Y)<0)]
            plot(X_pos[:,0],X_pos[:,1],'o')
            plot(X_neg[:,0],X_neg[:,1],'^')

if __name__ == '__main__':

    #Dataset
    X_train= [(3, 3),(4, 3),(1, 1)]
    Y_train= [ 1, 1, -1]
    X_test= [(4, 0),(1, 0)]

    #Define your dataset here    
    my_X_train =[(54,67),(183,-121),(-226,72),(86,163),(32,49),(-131,103),(-43,73),(34,-30),(358,29),(277,-79),(-135,89),(303,-115),(73,-107),(-6,-81),(71,-294),(-20,144),(-12,33),(149,-75),(141,137),(142,-171)]
    my_Y_train = [-1,-1,-1,1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1]
    my_X_test =[(48,103),(141,29),(2,-78),(-5,57),(170,-138),(-51,24),(0,81),(92,21),(15,88),(140,204)]

    #Type your codes here
    #perceptron = BinaryPerceptron()
    #call learn() function for the your training set
    #perceptron.learn(X_train,Y_train,10)
    #call predict() function with the your test set
    #call plotimg() function for the your training set
    #call plotimg() function for the your test set
    perceptron = BinaryPerceptron()
    '''perceptron.learn(X_train,Y_train,10)
    perceptron.predict(X_test)
    #plt.figure()
    #plt.subplot(211)
    perceptron.plotimg(X_train,Y_train)
    #plt.figure()
    #plt.subplot(212)
    #perceptron.plotimg(X_test)'''
    
    perceptron.learn(my_X_train,my_Y_train,500)
    print('w: ', perceptron.w)
    print('b: ', perceptron.b)
    perceptron.predict(my_X_test)
    perceptron.plotimg(my_X_train, my_Y_train)
    perceptron.plotimg(my_X_test)
    pass

############################################################
# The End
############################################################

