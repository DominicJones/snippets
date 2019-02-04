import serial

addr  = 'COM4'
baud  = 9600
fname = 'accel.dat'
fmode = 'ab'
reps  = 10

with serial.Serial(addr,baud) as port, open(fname,fmode) as outf:
    for i in range(reps):
        x = port.read(size=1)
        print x
        outf.write(x)
        outf.flush()
