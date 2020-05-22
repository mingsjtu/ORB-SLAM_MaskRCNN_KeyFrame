import numpy as np
import cv2
def hello(s):
    print("hello world")
    print(s)
 
 
def arg(a, b):
    print('a=', a)
    print('b=', b)
    return a + b
 
 
class Test:

    def __init__(self):
        print("init")
        self.a="mya"
 
    def say_hello(self, name):
        print("start")
        print("hello", name)
        print("hello", self.a)
        print("end")

        return name

    def save_pic(self,path):
        image=np.zeros((405,720))
        cv2.imwrite(path,image)
