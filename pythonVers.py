import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.metrics import r2_score

data = pd.read_csv('data.csv')
print("Data loaded successfully")
print(f"Data shape: {data.shape}")

print("\nFirst few rows of the dataset:")
print(data.head())

all_years = set(range(data['Year'].min(), data['Year'].max() + 1))
missing_years = all_years - set(data['Year'])
print(f"\nMissing years in dataset: {sorted(missing_years)}")

population_data = data[['Year', 'Population']].dropna()
internet_data = data[['Year', 'Percentage_Internet_User']].dropna()

internet_data = internet_data[internet_data['Percentage_Internet_User'] > 0]

X_pop = population_data['Year'].values.reshape(-1, 1)
y_pop = population_data['Population'].values

best_degree = 1
best_score = 0

for degree in range(1, 6):
    poly_features = PolynomialFeatures(degree=degree)
    X_poly = poly_features.fit_transform(X_pop)
    
    model = LinearRegression()
    model.fit(X_poly, y_pop)
    
    y_pred = model.predict(X_poly)
    score = r2_score(y_pop, y_pred)
    
    print(f"Population polynomial degree {degree}: R² score = {score}")
    
    if score > best_score:
        best_score = score
        best_degree = degree

print(f"\nBest polynomial degree for population: {best_degree}")
poly_features = PolynomialFeatures(degree=best_degree)
X_poly = poly_features.fit_transform(X_pop)
pop_model = LinearRegression()
pop_model.fit(X_poly, y_pop)

pop_coefficients = pop_model.coef_
pop_intercept = pop_model.intercept_

pop_equation = f"y = {pop_intercept:.2f}"
for i, coef in enumerate(pop_coefficients[1:], 1):
    if i == 1:
        pop_equation += f" + {coef:.6f}x"
    else:
        pop_equation += f" + {coef:.6f}x^{i}"
print("\nPopulation growth equation:")
print(pop_equation)

missing_years_pop = [2005, 2006, 2015, 2016]
missing_years_array = np.array(missing_years_pop).reshape(-1, 1)
missing_years_poly = poly_features.transform(missing_years_array)
predicted_population = pop_model.predict(missing_years_poly)

print("\nPredicted population values for missing years:")
for year, pop in zip(missing_years_pop, predicted_population):
    print(f"{year}: {int(pop)}")

future_year_pop = np.array([[2030]])
future_year_poly_pop = poly_features.transform(future_year_pop)
predicted_pop_2030 = pop_model.predict(future_year_poly_pop)[0]
print(f"\nEstimated population in 2030: {int(predicted_pop_2030)}")

X_int = internet_data['Year'].values.reshape(-1, 1)
y_int = internet_data['Percentage_Internet_User'].values

best_degree_int = 1
best_score_int = 0

for degree in range(1, 6):
    poly_features_int = PolynomialFeatures(degree=degree)
    X_poly_int = poly_features_int.fit_transform(X_int)
    
    model_int = LinearRegression()
    model_int.fit(X_poly_int, y_int)
    
    y_pred_int = model_int.predict(X_poly_int)
    score_int = r2_score(y_int, y_pred_int)
    
    print(f"Internet usage polynomial degree {degree}: R² score = {score_int}")
    
    if score_int > best_score_int:
        best_score_int = score_int
        best_degree_int = degree

print(f"\nBest polynomial degree for internet usage: {best_degree_int}")
poly_features_int = PolynomialFeatures(degree=best_degree_int)
X_poly_int = poly_features_int.fit_transform(X_int)
int_model = LinearRegression()
int_model.fit(X_poly_int, y_int)

int_coefficients = int_model.coef_
int_intercept = int_model.intercept_

int_equation = f"y = {int_intercept:.2f}"
for i, coef in enumerate(int_coefficients[1:], 1):
    if i == 1:
        int_equation += f" + {coef:.6f}x"
    else:
        int_equation += f" + {coef:.6f}x^{i}"
print("\nInternet usage percentage equation:")
print(int_equation)

missing_years_int = [2005, 2006, 2015, 2016]
missing_years_array_int = np.array(missing_years_int).reshape(-1, 1)
missing_years_poly_int = poly_features_int.transform(missing_years_array_int)
predicted_internet = int_model.predict(missing_years_poly_int)

print("\nPredicted internet usage percentage values for missing years:")
for year, internet_pct in zip(missing_years_int, predicted_internet):
    print(f"{year}: {internet_pct:.4f}%")

future_year_int = np.array([[2035]])
future_year_poly_int = poly_features_int.transform(future_year_int)
predicted_int_2035 = int_model.predict(future_year_poly_int)[0]
print(f"\nEstimated internet usage percentage in 2035: {predicted_int_2035:.4f}%")

future_year_pop_2035 = np.array([[2035]])
future_year_poly_pop_2035 = poly_features.transform(future_year_pop_2035)
predicted_pop_2035 = pop_model.predict(future_year_poly_pop_2035)[0]
estimated_users_2035 = (predicted_int_2035 / 100) * predicted_pop_2035
print(f"Estimated number of internet users in 2035: {int(estimated_users_2035)}")

plt.figure(figsize=(18, 12))

plt.subplot(2, 2, 1)
plt.scatter(population_data['Year'], population_data['Population'] / 1e6, color='blue', label='Actual Data')

years_range = np.arange(1960, 2040).reshape(-1, 1)
years_poly = poly_features.transform(years_range)
pop_predicted = pop_model.predict(years_poly)

plt.plot(years_range, pop_predicted / 1e6, color='red', label='Polynomial Fit')

plt.scatter(missing_years_pop, predicted_population / 1e6, color='green', marker='*', s=100, 
            label='Predicted Missing Values')

plt.scatter(2030, predicted_pop_2030 / 1e6, color='purple', marker='D', s=100, 
            label='2030 Prediction')
plt.scatter(2035, predicted_pop_2035 / 1e6, color='orange', marker='D', s=100, 
            label='2035 Prediction')

plt.title('Population Growth in Indonesia (1960-2035)')
plt.xlabel('Year')
plt.ylabel('Population (millions)')
plt.grid(True)
plt.legend()

plt.subplot(2, 2, 2)
plt.scatter(internet_data['Year'], internet_data['Percentage_Internet_User'], color='blue', label='Actual Data')

int_years_range = np.arange(1994, 2040).reshape(-1, 1)
int_years_poly = poly_features_int.transform(int_years_range)
int_predicted = int_model.predict(int_years_poly)

plt.plot(int_years_range, int_predicted, color='red', label='Polynomial Fit')

plt.scatter(missing_years_int, predicted_internet, color='green', marker='*', s=100, 
            label='Predicted Missing Values')

plt.scatter(2035, predicted_int_2035, color='orange', marker='D', s=100, 
            label='2035 Prediction')

plt.title('Internet Usage Percentage in Indonesia (1994-2035)')
plt.xlabel('Year')
plt.ylabel('Percentage of Population (%)')
plt.grid(True)
plt.legend()

plt.subplot(2, 1, 2)

internet_users_actual = []
years_with_both = []

for year in sorted(set(internet_data['Year']) & set(population_data['Year'])):
    pop_val = population_data[population_data['Year'] == year]['Population'].values[0]
    int_val = internet_data[internet_data['Year'] == year]['Percentage_Internet_User'].values[0]
    internet_users_actual.append((int_val / 100) * pop_val)
    years_with_both.append(year)

plt.scatter(years_with_both, np.array(internet_users_actual) / 1e6, color='blue', label='Actual Internet Users')

internet_users_predicted = []
for year in int_years_range.flatten():
    year_poly_pop = poly_features.transform(np.array([[year]]))
    year_poly_int = poly_features_int.transform(np.array([[year]]))
    
    pop_pred = pop_model.predict(year_poly_pop)[0]
    int_pred = int_model.predict(year_poly_int)[0]
    
    if int_pred > 0 and int_pred <= 100:
        internet_users_predicted.append((int_pred / 100) * pop_pred)
    else:
        internet_users_predicted.append(None)

valid_indices = [i for i, val in enumerate(internet_users_predicted) if val is not None]
valid_years = int_years_range.flatten()[valid_indices]
valid_users = np.array([internet_users_predicted[i] for i in valid_indices]) / 1e6

plt.plot(valid_years, valid_users, color='red', label='Predicted Internet Users')

missing_internet_users = []
for i, year in enumerate(missing_years_int):
    pop_val = predicted_population[i]
    int_val = predicted_internet[i]
    missing_internet_users.append((int_val / 100) * pop_val)

plt.scatter(missing_years_int, np.array(missing_internet_users) / 1e6, color='green', marker='*', s=100, 
            label='Predicted Missing Values')

plt.scatter(2035, estimated_users_2035 / 1e6, color='orange', marker='D', s=100, 
            label='2035 Prediction')

plt.title('Number of Internet Users in Indonesia (1994-2035)')
plt.xlabel('Year')
plt.ylabel('Internet Users (millions)')
plt.grid(True)
plt.legend()

plt.tight_layout()
plt.savefig('Indonesia_Population_Internet_Analysis.png')
plt.show()

print("\nAnalysis completed and visualizations created.")