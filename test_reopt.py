"""
Tests reopt on binaries stored in a config file
"""
import os, time, subprocess, argparse


def reopt(program, extension, options):
    print("reopt:",program+extension,options)
    ret = os.popen("which "+program.split()[0])
    location = ret.read().strip()
    ret = os.system("reopt "+options+" -o "+program+extension+" "+location+" &> "+program+extension+".out")
    os.system("reopt-explore "+location+" &> "+program+extension+".explore")

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
    return run_time


def test_programs(programs, output):
    for filename in programs: 
        just_prog = filename.split()[0]
        args = ""
        if len(filename.split()) > 1:
            args = filename.split()[1]
        write_file(output, filename+",")

        ret = reopt(just_prog,".exe","")
        write_file(output, str(ret)+",")
        if ret==0: 
            ret=test_functionality("./"+just_prog+".exe "+args)
        write_file(output, str(ret)+",")
        if ret==0:
            speed = 0
            #speed = test_speed("./"+just_prog+".exe "+args,1000)
            write_file(output, str(speed)+",")
        else:
            write_file(output, "N/A,")

        ret = reopt(just_prog,".o1","--opt-level=1")
        write_file(output, str(ret)+",")
        if ret==0: 
            ret=test_functionality("./"+just_prog+".o1 "+args)
        write_file(output, str(ret)+",")
        if ret==0:
            speed = 0
            #speed = test_speed("./"+just_prog+".o1 "+args,1000)
            write_file(output, str(speed)+",")
        else:
            write_file(output, "N/A,")

        ret = reopt(just_prog,".o2","--opt-level=2")
        write_file(output, str(ret)+",")
        if ret==0: 
            ret=test_functionality("./"+just_prog+".o2 "+args)
        write_file(output, str(ret)+",")
        if ret==0:
            speed = 0
            #speed = test_speed("./"+just_prog+".o2 "+args,1000)
            write_file(output, str(speed)+",")
        else:
            write_file(output, "N/A,")

        ret = reopt(just_prog,".o3","--opt-level=3")
        write_file(output, str(ret)+",")
        if ret==0: 
            ret=test_functionality("./"+just_prog+".o3 "+args)
        write_file(output, str(ret)+",")
        if ret==0:
            speed = 0
            #speed = test_speed("./"+just_prog+".o3 "+args,1000)
            write_file(output, str(speed)+",")
        else:
            write_file(output, "N/A,")

        write_file(output, "\n")


def write_file(filename, content):
    with open(filename, 'a') as file:
        file.write(content)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--output",
            type=str,
            help="Filename of output",
            default="output.csv")
    parser.add_argument("-i", "--input",
            type=str,
            help="Input filename containing list of programs to test. Default is programs.txt",
            default="programs.txt")
    args = parser.parse_args()
    write_file(args.output, "program name,lifts, functional,speed,o1 lifts, o1 functional, o1 speed,o2 lifts, o2 functional, o2 speed, o3 lifts, o3 functional, o3 speed\n")
    programs = read_config(args.input)
    test_programs(programs, args.output)
    

if __name__=="__main__":
    main()
