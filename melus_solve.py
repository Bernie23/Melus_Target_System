# Welcome to MelusSolve
# This module contains various methods to compute the point of impact
import numpy
from math import *
from scipy.optimize import fsolve, least_squares


def find_poi(t21, t31, t41, propagation_speed):
    """
    Method to find the point of impact from three time difference of arrival points
        args:
            t21 (float):
                * Time difference between sensors 2 and 1
            t31 (float):
                * Time difference between sensors 3 and 1
            t41 (float):
                * Time difference between sensors 4 and 1
            propagation_speed (float):
                * Measured acoustic propagation velocity
    """
    def TDOA(z):
        d = 0.1
        F0 = sqrt((d-z[0])**2 + z[1]**2) - sqrt(z[0]**2 + z[1]**2) - vt21
        F1 = sqrt((d-z[0])**2 + (d-z[1])**2) - sqrt(z[0]**2 + z[1]**2) - vt31
        F2 = sqrt(z[0]**2 + (d-z[1])**2) - sqrt(z[0]**2 + z[1]**2) - vt41
        return [F0, F1, F2]

    guess = [0.05, 0.05]
    vt21 = propagation_speed * t21
    vt31 = propagation_speed * t31
    vt41 = propagation_speed * t41

    return least_squares(TDOA, guess)


def cartesian_to_target(impact):
    shifted_impact = [impact[0] - 0.05, impact[1] - 0.05]
    magnitude = sqrt(shifted_impact[0]**2 + shifted_impact[1]**2)
    ratio = shifted_impact[0] / shifted_impact[1]
    theta = numpy.arctan(ratio) * 180 / numpy.pi
    return [magnitude, theta]
