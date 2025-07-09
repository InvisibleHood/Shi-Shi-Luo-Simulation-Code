import random
import numpy as np
import time

fw_file = "simulation_results_2_strategies_donation_game.txt"


def assign_initial_strategies(m, n):  
    groups = [random.randint(0, n) for _ in range(m)]
    urns = [0] * (n + 1)
    for i in groups:
        urns[i] += 1.0
    return urns

def frac(urns, i):
    if i < 0 or i >= len(urns):
        raise ValueError(f"Index {i} is out of bounds for urns of length {len(urns)}.")
    return urns[i] / sum(urns)

def draw_time_poisson(leaving_rate_matrix):
    lambd = np.sum(leaving_rate_matrix)
    return np.random.poisson(lambd)

def draw_random_coord(matrix):
    prob_matrix = matrix / np.sum(matrix)
    if np.sum(matrix) < 0:
        raise ValueError(f"Negative sum of matrix: ", np.sum(matrix))
    if np.any(prob_matrix < 0):
        print("Matrix is: ", matrix)
        raise ValueError(f"Negative probabilities in draw_prob: {prob_matrix}")
    
    return np.random.choice(len(prob_matrix), p=prob_matrix)  


def get_leaving_rates(urns, m, n, s, r, w):
    sum = 0
    group_level_urns = np.zeros(n + 1)
    for i in range(n + 1):
        group_level_urns[i] = frac(urns, i) * w * (1 + r * (i / n))
        sum += group_level_urns[i]

    leaving_rate_matrix = np.zeros(n + 1)
    leaving_rate_matrix[0] = (sum - group_level_urns[0]) * m * frac(urns, 0)
    for i in range(1, n):
        leaving_rate_matrix[i] = m * frac(urns, i) * (n - i) * (i / n) * (2 + s) + m * frac(urns, i) * (sum - group_level_urns[i])
    
    leaving_rate_matrix[n] = (sum - group_level_urns[n]) * m * frac(urns, n)
    return leaving_rate_matrix

def get_destination_rates(I1, n, urns, s, r, w):
    destination_rate_matrix = np.zeros(n + 1)
    for i in range(n + 1):
        if i == I1:
            continue
        if i == I1 - 1 and I1 < n:
            destination_rate_matrix[i] = (n - I1) * (1 + s) * I1 / n + frac(urns, I1 - 1) * (1 + r * (I1 - 1) / n) * w
        elif i == I1 + 1 and I1 > 0:
            destination_rate_matrix[i] = (I1) * (1 - I1 / n) + frac(urns, I1 + 1) * (1 + r * (I1 + 1) / n) * w
        else:
            destination_rate_matrix[i] = frac(urns, i) * (1 + r * i / n) * w
    
    return destination_rate_matrix


def main():
    # Parameters
    m = 50              # number of groups
    n = 50              # number of individuals in a group
    s = 0.5
    r = 0.5
    w = 10

    coop = 0
    defect = 0
    
    simulation_times = 100
    start_time = time.perf_counter()

    for counter in range(simulation_times):
        # Initialize distribution
        print("\033[1;32mCounter: \033[0m", counter)
        urns = assign_initial_strategies(m, n)
        #print("\033[1;32mInitial distribution of strategies (urns):\033[0m", urns)
        event_time = 0

        while urns[0] < m and urns[n] < m:
            leaving_rate_matrix = get_leaving_rates(urns, m, n, s, r, w)
            
            if np.any(leaving_rate_matrix < -1e-8):
                raise ValueError(f"\033[31mNegative values in leaving rates (leaving_rate_matrix)\033[0m\n: {leaving_rate_matrix}")

            if np.sum(leaving_rate_matrix) == 0:
                raise ValueError("\033[31mSum of leaving rates (L) is zero, unable to draw random numbers.\033[0m")
            # print("\033[1;32mLeaving rate matrix:\033[0m")
            # print(leaving_rate_matrix)
            tau = draw_time_poisson(leaving_rate_matrix)
            I1 = draw_random_coord(leaving_rate_matrix) #the coordinate of the individual who will leave the group (i)
            
            destination_rate_matrix = get_destination_rates(I1, n, urns, s, r, w)

            if np.any(destination_rate_matrix < -1e-8):
                raise ValueError(f"\033[31mNegative values in incoming rates (destination_rate_matrix)\033[0m\n: {destination_rate_matrix}")
                
            if np.sum(destination_rate_matrix) == 0:
                raise ValueError("\033[31mSum of incoming rates (L) is zero, unable to draw random numbers.\033[0m")
            

            I2 = draw_random_coord(destination_rate_matrix)
            #print(f"\033[1;32mI1: {I1}, I2: {I2}, tau: {tau}\033[0m")

            urns[I1] -= 1
            urns[I2] += 1
            event_time += tau

            if np.any(np.array(urns) < 0):
                raise ValueError(f"\033[31mNegative values in urns after iteration:\033[0m\n{urns}")
            #print(f"\033[1;32murns: {urns}\033[0m")
            
        if urns[0] == m:
            defect += 1
        elif urns[n] == m:
            coop += 1
        else:
            raise ValueError("\033[31mUnexpected final distribution of strategies.\033[0m")
        
        # print("\033[1;32mFinal distribution of strategies (urns):\033[0m", urns)

    end_time = time.perf_counter()
    elapsed_time = end_time - start_time
    

    print("\033[1;32mSimulation is done.\033[0m") 
    print("\033[1;32mCooperators:\033[0m", coop)
    print("\033[1;32mDefectors:\033[0m", defect)
    print(f"\033[1;32mThe whole simulation took {elapsed_time:.6f} seconds to execute.\033[0m")

    


if __name__ == "__main__":
    main()

