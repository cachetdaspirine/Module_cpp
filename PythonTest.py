#!/home/hugo/anaconda3/bin/python3
import numpy as np

from ctypes import cdll
from ctypes import c_double
from ctypes import c_int
from ctypes import POINTER
from ctypes import c_void_p
from ctypes import c_char_p

lib = cdll.LoadLibrary('./lib.so')



lib.CreateSystem.restype=c_void_p
lib.CreateSystem.argtypes=[POINTER(c_int) , c_int,c_int]

lib.UpdateSystemEnergy.argtypes=[c_void_p,POINTER(c_int),c_int,c_int]

lib.GetSystemEnergy.restype=c_double
lib.GetSystemEnergy.argtypes=[c_void_p]

lib.OutputSystemSite.argtypes=[c_void_p,c_char_p]
lib.OutputSystemSpring.argtypes=[c_void_p,c_char_p]


Array=np.array([1,0,0,0,0 ,0,0,0,0,0 ,0,0,0,0,0 ,0,0,0,0,0 ,0,0,0,0,1 ],dtype=int);

#ArrayIn=Array.ctypes.data_as(POINTER(c_int))
#ArrayIn.data=Array
#for i in range(Array.shape[0]):
#    ArrayIn[i]=Array[i]
Array = Array.ctypes.data_as(POINTER(c_int))
for i in range(25):
    print(Array[i])
system=lib.CreateSystem(ArrayIn,5,5)

print(lib.GetSystemEnergy(system))

FileName='Test.txt'

lib.OutputSystemSite(system,FileName.encode('utf-8'))


Array=np.array([1,1,1,1,1 ,0,0,0,0,0 ,0,0,0,0,1 ,0,0,0,0,0 ,0,0,0,0,0 ]);
Array=Array.astype(int)
ArrayIn=Array.ctypes.data_as(POINTER(c_int))
for i in range(Array.shape[0]):
    ArrayIn[i]=Array[i]

lib.UpdateSystemEnergy(system,ArrayIn,5,5)

FileName='Test2.txt'

lib.OutputSystemSite(system,FileName.encode('utf-8'))
