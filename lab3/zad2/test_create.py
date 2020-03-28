import numpy as np

if __name__ == "__main__":
    A = 'matrices/0_a.txt'
    B = 'matrices/0_b.txt'
    
    a_mat = []
    b_mat = []

    afile = open(A,'r')
    print("{")
    for line in afile:
        print("{"+", ".join(line.split())+"},")
        a_mat.append([int(x) for x in line.split()])
    print("};")

    bfile = open(B,'r')
    print("{")
    for line in bfile:
        print("{"+", ".join(line.split())+"},")
        b_mat.append([int(x) for x in line.split()])
    print("};")

    a_np = np.array(a_mat)
    print("A", a_np.shape)
    b_np = np.array(b_mat)
    print("B", b_np.shape)
    res = b_np @ a_np
    print("R", res.shape)
    print("{")
    for line in (res):
        print("{"+",".join(list(map(str,list(line))))+"},")
    print("};")