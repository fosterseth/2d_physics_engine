import math
force1 = -20.0
force2 = 20.0
force = force1
inverseMass = 15.0
pos = 750.0
vel = 0.0
fid = open("c:\\users\\sbf\\desktop\\py_test.txt", "w")
n = 1
while True:
    n += 1
    fid.write("%f,%f\n" % (pos, vel))
    pos = pos + (vel * 0.016)
    vel = vel + (force * inverseMass * 0.016)
    #vel = vel * math.pow(.98, 0.016)
    if pos <= 500.0:
        force = force2
    else:
        force = force1
    if n > 1014:
        break
        
fid.close()
    