import matplotlib.pyplot as plt
import numpy as np
import os
import re

from pymoo.util.nds.non_dominated_sorting import NonDominatedSorting
from pymoo.visualization.scatter import Scatter

def main():
    # Directory name
    directory_name = 'results/'

    # Loop over the all the files
    for file in os.listdir(directory_name):
        file_path = os.path.join(directory_name, file)
        #file_path = 'results/pla33810-n169045.txt'

        travel_time, profit_list = [], []

        # Open the file to read
        with open(file_path, 'r') as file:
            
            # Read the file
            str  = file.read()

            # Delimiter characters
            delimiters = r'[ \n]+'
            split_text = re.split(delimiters, str)

            # Pop the last random value in the list
            split_text.pop()
            index = 0

            # Append the values to the lists
            while index < len(split_text):
                travel_time.append(float(split_text[index]))
                profit_list.append(-float(split_text[index+1]))
                index = index + 2

            # Make np array with tuple of travel time and (negative of) profit list.
            solutions = np.array([(travel_time[i], profit_list[i]) for i in range(len(travel_time))])
            
            # Perform non-dominated sorting
            nds = NonDominatedSorting()
            front = nds.do(solutions)

            # Extract the Pareto front (first front)
            pareto_front = solutions[front[0]]

            # Plot the pareto front
            plot = Scatter(title = file_path, legend = True, labels = ["Travel time", "Negative Profit"])
            plot.add(solutions, s=30, facecolors='none', edgecolors='b', alpha=0.7, label="Solutions")
            plot.add(pareto_front, s=30, color='r', label="Pareto Front")
            plot.show()
            
            pareto_front_unique = np.unique(pareto_front, axis=0)
            print(file_path, " : ", pareto_front_unique)

        # with open(file_path, 'a') as file:
        #     file.write("Pareto front values: \n")
        #     np.savetxt(file, solutions[front[0]], fmt='%8.5f', delimiter=' ')
        #     np.savetxt(file, solutions[front[1]], fmt='%8.5f', delimiter=' ')
        #     np.savetxt(file, solutions[front[2]], fmt='%8.5f', delimiter=' ')

        # Plot the results
        # plt.scatter(travel_time, profit_list)
        # plt.xlabel('Travel time')
        # plt.ylabel('Profit')
        # plt.title(file.name)
        # plt.show()


if __name__ == "__main__":
    main()