import numpy as np
import math



R = np.array([[1, -1., 3, -1., -1., 5, -1., -1., 5, -1., 4, -1.], 
              [-1., -1., 5, 4, -1., -1., 4, -1., -1., 2, 1, 3],
              [2, 4, -1., 1, 2, -1., 3, -1., 4, 3, 5, -1.],
              [-1., 2, 4, -1., 5, -1., -1., 4, -1., -1., 2, -1.],
              [-1., -1., 4, 3, 4, 2, -1., -1., -1., -1., 2, 5],
              [1, -1., 3, -1., 3, -1., -1., 2, -1., -1., 4, -1.]])

#print(R)

r_mean = []

for i in range(12):
    count = 0
    total = 0
    for item in R[:,i]:
        if item >0:
            total += item
            count += 1
    r_mean.append(total/count)
#print(r_mean)    

sim5_m = []


for i in range(12): #计算user_5和user_i之间的相似度
    top=0
    bottom_l=0
    bottom_r=0
    for k in range(6):
        if R[k][i] >= 0:
            bottom_r += np.square(R[k][i]-r_mean[i])
            if R[k][4] >= 0:
                top += (R[k][i]-r_mean[i]) * (R[k][4] - r_mean[4])
        if R[k][4] >= 0:
            bottom_l += np.square(R[k][4] - r_mean[4])
    sim5_m.append(top/ (np.sqrt(bottom_l)* np.sqrt(bottom_r)) )


print(sim5_m)

