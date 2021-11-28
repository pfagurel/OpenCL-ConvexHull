import numpy as np
import sys

if __name__ == "__main__":
    random_vec1 = np.random.randint(int(sys.argv[1]), size=(int(sys.argv[1])),dtype=np.int64)
    random_vec2 = np.random.randint(int(sys.argv[1]), size=(int(sys.argv[1])),dtype=np.int64)
    unique_random_vec1 = np.unique(random_vec1)
    unique_random_vec2 = np.unique(random_vec2)
    np.random.shuffle(unique_random_vec1)
    np.random.shuffle(unique_random_vec2)
    m_len = min(len(unique_random_vec1),len(unique_random_vec2))
        
    
    f = open(sys.argv[1]+".txt", "w")
    for i in range(m_len):
        f.write(str(unique_random_vec1[i]) + " " + str(unique_random_vec2[i]) + '\n')
    f.close()
