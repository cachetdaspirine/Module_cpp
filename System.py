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

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection
from matplotlib.colors import LinearSegmentedColormap
import os

cdict = {'blue':   ((0.0,  0.9,0.9),
                    (0.5,  0.4, 0.4),
                    (1.0,  0.1, 0.1)),

         'green': ((0.0,  0.5, 0.5),
                   (0.5 , 1, 1),
                   (1.0,  0.3, 0.3)),

         'alpha': ((0.0,  1, 1),
                   (0.5 , 0.8, 0.8),
                   (1.0,  1, 1)),

         'red':  ((0.0,  0.4, 0.4),
                   (0.5,  0.5, 0.5),
                   (1.0,  0.9,0.9)),
}
cm = LinearSegmentedColormap('my_colormap', cdict, 1024)

class System:
    def __init__(self,State):
        #To the system is created by a 2D array for a 2D system the shape[0]
        #is the Y lengfth while the shape[1]  is  the  X  shape  the    most
        #important part of this object is self.adress which is the adress of
        #the pointer toward the cpp object.
        self.Lx=State.shape[1]
        self.Ly=State.shape[0]
        #--------------Convert the array into a pointer array---------------
        array = State.flatten()
        Arraycpp = array.ctypes.data_as(POINTER(c_int))
        for i in range(array.shape[0]):
            Arraycpp[i]=array[i]
        #-------------------------------------------------------------------
        #---------------------Create the cpp object-------------------------
        self.Adress=lib.CreateSystem(Arraycpp,self.Lx,self.Ly)
        #--------------------Store the value of the Energy------------------
        self.Energy=lib.GetSystemEnergy(self.Adress)
    def Evolv(self,NewState):
        #------------Convert the new state into a pointer array-------------
        array = NewState.flatten()
        Arraycpp = array.ctypes.data_as(POINTER(c_int))
        for i in range(array.shape[0]):
            Arraycpp[i]=array[i]
        #-------------------------------------------------------------------
        lib.UpdateSystemEnergy(self.Adress,Arraycpp,self.Lx,self.Ly)
        self.Energy=lib.GetSystemEnergy(self.Adress)
    def PrintPerSite(self,Name='NoName.txt'):
        lib.OutputSystemSite(self.Adress,Name.encode('utf-8'))
    def PrintPerSpring(self,Name='NoName.txt'):
        lib.OutputSystemSpring(self.Adress,Name.encode('utf-8'))
    def PlotPerSite(self):
        fig,ax=plt.subplots(figsize=(7,5))
        self.PrintPerSite('ToPlot.txt')
        Data=np.loadtxt('ToPlot.txt',dtype=float)
        XC,YC=0,0
        for ligne in Data :
            XY=[]
            for i in range(ligne.shape[0]//2):
                XY.append([ligne[2*i],ligne[2*i+1]])
            XC+=sum(np.transpose(XY)[0])/len(XY)
            YC+=sum(np.transpose(XY)[1])/len(XY)
            ax.add_patch(Polygon(XY,closed=True,linewidth=0.8,fill=True,fc=(0.41,0.83,0.94,0.5),ec=(0,0,0,1),ls='-',zorder=0))

        ax.set_xlim([XC/Data.shape[0]-2*np.sqrt(Data.shape[0]),XC/Data.shape[0]+2*np.sqrt(Data.shape[0])])
        ax.set_ylim([YC/Data.shape[0]-2*np.sqrt(Data.shape[0]),YC/Data.shape[0]+2*np.sqrt(Data.shape[0])])

        plt.show()
        os.system('rm -rf ToPlot.txt')

