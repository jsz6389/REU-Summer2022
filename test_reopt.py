"""
Tests reopt on binaries stored in a config file
"""
import os, time, subprocess


def reopt(program, extension, options):
    print("reopt:",program+extension,options)
    ret = os.popen("which "+program)
    location = ret.read().strip()
    ret = os.system("reopt "+options+" -o "+program+extension+" "+location+" &> "+program+".out")

    if ret==0:
        print("reopt: Program lifted successfully")
    else:
        print("reopt: Failed to lift program")

    return ret


def read_config(filename):
    programs = []
    with open(filename, 'r') as file:
        for line in file:
            programs.append(line.strip())

    return programs


def test_functionality(filename):
    ret = os.system(filename + " > /dev/null")
    if ret == 0:
        print(filename+": Runs successfully")
    else:
        print(filename+": Failed to run")
    return ret


def test_speed(filename, n):
    start = time.time()

    for i in range(0,n):
        os.popen(filename)

    stop = time.time()
    run_time = stop-start
    print(filename+": Ran "+str(n)+" tests in "+str(run_time)+" seconds.")


def main():
    programs = read_config("programs.txt")
    for filename in programs:
        ret = reopt(filename,".exe","")
        if ret==0: 
            ret=test_functionality("./"+filename+".exe")
        ret==0 and test_speed("./"+filename+".exe",1000)

        ret = reopt(filename,".o1","--opt-level=1")
        if ret==0:
            ret=test_functionality("./"+filename+".o1")
        ret==0 and test_speed("./"+filename+".o1",1000)

        ret = reopt(filename,".o2","--opt-level=2")
        if ret==0:
            ret=test_functionality("./"+filename+".o2")
        ret==0 and test_speed("./"+filename+".o2",1000)

        ret = reopt(filename,".o3","--opt-level=3")
        if ret==0:
            ret=test_functionality("./"+filename+".o3")
        ret==0 and test_speed("./"+filename+".o3",1000)
    

def main2():
    reopt("ls",".o3","--opt-level=1")
    test_functionality("./ls.o3")
    test_speed("./ls.o3", 10000)
    test_speed("ls", 10000)


if __name__=="__main__":
    main()
