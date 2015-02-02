# To change this license header, choose License Headers in Project Properties.
# To change this template file, choose Tools | Templates
# and open the template in the editor.

from random import *
import sys

__author__="dtrilla"
__date__ ="$Dec 12, 2014 10:20:12 AM$"

def outputforC(nstructs,nbanks,nconflicts,penalty,capacities,costs,sizes,conflicts,file):
    file.write(str(penalty)+"\n")
    file.write(str(nstructs)+"\n")
    
    for i in sizes:
        file.write(str(i)+" ")
    file.write("\n")
    
    for i in costs:
        file.write(str(i)+" ")
    file.write("\n")
    
    file.write(str(nbanks)+"\n")
    
    for i in capacities:
        file.write(str(i)+" ")
    file.write("\n")
    
    file.write(str(nconflicts)+"\n")
    for i in conflicts:
        file.write(str(i[0]-1)+" ")
    file.write("\n")
    for i in conflicts:
        file.write(str(i[1]-1)+" ")
    file.write("\n")
    for i in conflicts:
        file.write(str(i[2])+" ")
    file.write("\n")
    return
    
def outputforCPLEX(nstructs,nbanks,nconflicts,penalty,capacities,costs,sizes,conflicts,file):
    file.write("Nn="+str(nstructs)+";\n")
    file.write("Nm="+str(nbanks)+";\n")
    file.write("No="+str(nconflicts)+";\n")
    file.write("p="+str(penalty)+";\n")
    
    file.write("s=[")
    for i in sizes:
        file.write(str(i)+" ")
    file.write("];\n")
    
    file.write("e=[")
    for i in costs:
        file.write(str(i)+" ")
    file.write("];\n")
    
    file.write("c=[")
    for i in capacities:
        file.write(str(i)+" ")
    file.write("];\n")
    
    file.write("conf=[\n")
    for i in conflicts:
        file.write("["+str(i[2])+" "+str(i[0])+" "+str(i[1])+"]\n")
    file.write("];")
    return


if __name__ == "__main__":
    CPLEX_name = sys.argv[1]+'CPLEX'
    C_name = sys.argv[1]+'C++'
    
    nstructs = int(sys.argv[2])
    nbanks = int(sys.argv[3])
    nconflicts = int(sys.argv[4])
    
    penalty = randint(1,20)
    
    capacities = []
    for i in range(nbanks):
        capacities += [randint(1000,3000)]
    
    costs = []
    sizes = []
    for i in range(nstructs):
        sizes += [randint(1,200)] 
        costs += [randint(10,50)]
        

    conflicts = []
    for i in range(nconflicts):
        conflicts += [[randint(1,nstructs)] + [randint(1,nstructs)] + [randint(10,400)]]
        
    CPLEXfile = open(CPLEX_name,"w+")
    Cfile = open(C_name,"w+")
    outputforCPLEX(nstructs,nbanks,nconflicts,penalty,capacities,costs,sizes,conflicts,CPLEXfile)
    outputforC(nstructs,nbanks,nconflicts,penalty,capacities,costs,sizes,conflicts,Cfile)
