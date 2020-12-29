# Software test script
import melus_solve
import time

propagation_speed = 343
t21 = (-8.921e-5)
t31 = (-3.965e-5)
t41 = (3.382e-5)

# Call the find_poi method and check the computation time
time_start = time.time()
point_of_impact = melus_solve.find_poi(t21, t31, t41, propagation_speed)
time_finish = time.time()
time_computation = time_finish - time_start

print("Time of computation: " + str(time_computation))
print(point_of_impact["x"])
print(melus_solve.cartesian_to_target(point_of_impact["x"]))