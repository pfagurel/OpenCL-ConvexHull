import numpy as np
import sys

if __name__ == "__main__":
    random_vec1 = np.arange(int(sys.argv[1]))
    random_vec2 = np.arange(int(sys.argv[1]))
    np.random.shuffle(random_vec1)
    np.random.shuffle(random_vec2)
    m_len = len(random_vec1)
        
    
    f = open(sys.argv[1]+".txt", "w")
    for i in range(m_len):
        f.write(str(random_vec1[i]) + " " + str(random_vec2[i]) + '\n')
    f.close()
