# Software test script
import melus_solve

propagation_speed = 343
t21 = (-8.921e-5)
t31 = (-3.965e-5)
t41 = (3.382e-5)

point_of_impact = melus_solve.find_poi(t21, t31, t41, propagation_speed)
print(point_of_impact["x"])
print(melus_solve.cartesian_to_target(point_of_impact["x"]))