# Welcome to MelusSolve
# This module contains various methods to compute the point of impact
from math import *
from numpy import empty, array
from scipy.optimize import fsolve

def find_poi(t21, t31, t41, propagation_speed):
    """
    Method to find the point of impact from three time difference of arrival points
    """
    print("HI")