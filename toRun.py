import subprocess
import math
import os
subprocess.call(["g++", "main.cpp","-o", "main" ,"-std=c++11", "-pthread", "-O3"])
print("Compiled")
configType = str(input("Do you want to open config file(f) or enter from console(c)? f/c "))
os.chdir("./cmake-build-debug")
if configType == "c":
    f = open("./Read.txt", "w")
    infile=str(input("Enter infile, please... "))
    out_by_a=str(input("Enter outfile by a, please... "))
    out_by_n=str(input("Enter outfile by n , please... "))
    threads=str(input("Enter number of threads, please... "))
    block_size=str(input("Enter size of block, please... "))
    f.write('infile="'+infile+'"\n')
    f.write('out_by_a="'+out_by_a +'"\n')
    f.write('out_by_n="'+out_by_n+'"\n')
    f.write('threads='+threads+'"\n')
    f.write('block_size='+block_size)
    f.close()
else:
    f = open("./Read.txt", "r")
    print("It's yours configuration...")
    for line in f:
        print(line)
number = int(input("How many times do you want to run file: "))
while number > 0:
    subprocess.call("../main")
    number -= 1

f= open("./result.txt", "r")
words = []
for line in f:
    words.append([float(n) for n in line.strip().split('  ')])
reading_min = writing_min = counting_min = all_min = math.inf
for n in range(len(words)):
    if words[n][0] < reading_min:
        reading_min = words[n][0]
    if words[n][1] < writing_min:
        writing_min = words[n][1]
    if words[n][2] < counting_min:
        counting_min = words[n][2]
    if words[n][3] < all_min:
        all_min = words[n][3]

print("----- Minimum time at all -----")
print("ONLY reading time: " + str(reading_min))
print("ONLY writing time: " + str(writing_min))
print("ONLY counting time: " + str(counting_min))
print("All time: " + str(all_min))
