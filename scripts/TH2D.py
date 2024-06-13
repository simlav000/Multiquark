import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
file_path = 'KLMass.csv'  # Replace with your file path
data = pd.read_csv(file_path)

# Assuming the CSV has two columns: 'x' and 'y'
x = data.iloc[:, 0]
y = data.iloc[:, 1]

print(len(x))

# Create the 2D histogram
plt.hist2d(x, y, bins=30, cmap='Blues')
plt.colorbar(label='Counts')
plt.xlabel('X')
plt.ylabel('Y')
plt.title('2D Histogram')
plt.show()
