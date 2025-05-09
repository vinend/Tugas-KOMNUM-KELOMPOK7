#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_ROWS 100
#define MAX_LINE_LENGTH 256
#define MAX_YEARS 100
#define MAX_POLYNOMIAL_DEGREE 5

typedef struct {
    int year;
    double internet_percentage;
    long long population;
} DataRecord;

typedef struct {
    int degree;
    double coefficients[MAX_POLYNOMIAL_DEGREE + 1];
    double r_squared;
} PolynomialModel;

int readCSV(const char* filename, DataRecord* data);
void separateData(DataRecord* data, int data_count, DataRecord* population_data, int* pop_count, 
                 DataRecord* internet_data, int* int_count);
void findMissingYears(DataRecord* data, int data_count, int* missing_years, int* missing_count);
PolynomialModel fitPolynomialRegression(DataRecord* data, int count, int col_type, int max_degree);
double calculatePrediction(PolynomialModel model, int year);
double calculateRSquared(DataRecord* data, int count, PolynomialModel model, int col_type);

// New function declarations for GNUplot
void generatePlotData(PolynomialModel pop_model, PolynomialModel int_model, 
                      DataRecord* population_data, int pop_count,
                      DataRecord* internet_data, int int_count,
                      int* missing_years, int missing_count,
                      double* predicted_population, double* predicted_internet);
void createPopulationPlot();
void createInternetPercentagePlot();
void createInternetUsersPlot();
void setupGNUplot();
void cleanupTempFiles();

int main() {
    DataRecord data[MAX_ROWS];
    DataRecord population_data[MAX_ROWS];
    DataRecord internet_data[MAX_ROWS];
    int missing_years[10] = {2005, 2006, 2015, 2016};
    int missing_count = 4;
    int data_count, pop_count = 0, int_count = 0;
    
    data_count = readCSV("data.csv", data);
    printf("Data loaded successfully\n");
    printf("Data count: %d\n", data_count);
    
    printf("\nFirst few rows of the dataset:\n");
    printf("Year,Percentage_Internet_User,Population\n");
    for (int i = 0; i < 5 && i < data_count; i++) {
        printf("%d,%.6f,%lld\n", data[i].year, data[i].internet_percentage, data[i].population);
    }
    
    separateData(data, data_count, population_data, &pop_count, internet_data, &int_count);
    
    printf("\nPopulation data count: %d\n", pop_count);
    printf("Internet data count: %d\n", int_count);
    
    printf("\n1. POPULATION ANALYSIS\n");
    printf("----------------------\n");
    
    PolynomialModel pop_model = fitPolynomialRegression(population_data, pop_count, 2, MAX_POLYNOMIAL_DEGREE);
    
    printf("\nPopulation growth equation:\n");
    printf("y = %.2f", pop_model.coefficients[0]);
    for (int i = 1; i <= pop_model.degree; i++) {
        printf(" + %.6f*x^%d", pop_model.coefficients[i], i);
    }
    printf("\n");
    
    printf("\nPredicted population values for missing years:\n");
    double predicted_population[10];
    for (int i = 0; i < missing_count; i++) {
        predicted_population[i] = calculatePrediction(pop_model, missing_years[i]);
        printf("%d: %.0f\n", missing_years[i], predicted_population[i]);
    }
    
    double predicted_pop_2030 = calculatePrediction(pop_model, 2030);
    printf("\nEstimated population in 2030: %.0f\n", predicted_pop_2030);
    
    printf("\n2. INTERNET USAGE ANALYSIS\n");
    printf("--------------------------\n");
    
    PolynomialModel int_model = fitPolynomialRegression(internet_data, int_count, 1, MAX_POLYNOMIAL_DEGREE);
    
    printf("\nInternet usage percentage equation:\n");
    printf("y = %.2f", int_model.coefficients[0]);
    for (int i = 1; i <= int_model.degree; i++) {
        printf(" + %.6f*x^%d", int_model.coefficients[i], i);
    }
    printf("\n");
    
    printf("\nPredicted internet usage percentage values for missing years:\n");
    double predicted_internet[10];
    for (int i = 0; i < missing_count; i++) {
        predicted_internet[i] = calculatePrediction(int_model, missing_years[i]);
        printf("%d: %.4f%%\n", missing_years[i], predicted_internet[i]);
    }
    
    double predicted_int_2035 = calculatePrediction(int_model, 2035);
    printf("\nEstimated internet usage percentage in 2035: %.4f%%\n", predicted_int_2035);
    
    double predicted_pop_2035 = calculatePrediction(pop_model, 2035);
    double estimated_users_2035 = (predicted_int_2035 / 100.0) * predicted_pop_2035;
    printf("Estimated number of internet users in 2035: %.0f\n", estimated_users_2035);
    
    // Generate plots using GNUplot
    printf("\n3. GENERATING PLOTS\n");
    printf("------------------\n");
    
    generatePlotData(pop_model, int_model, population_data, pop_count, internet_data, int_count,
                     missing_years, missing_count, predicted_population, predicted_internet);
    
    setupGNUplot();
    createPopulationPlot();
    createInternetPercentagePlot();
    createInternetUsersPlot();
    
    printf("\nPlots have been saved to 'Indonesia_Population_Internet_Analysis_C.png'\n");
    
    cleanupTempFiles();
    
    printf("\nAnalysis completed and visualizations created.\n");
    
    return 0;
}

int readCSV(const char* filename, DataRecord* data) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int count = 0;
    
    fgets(line, MAX_LINE_LENGTH, file);
    
    while (fgets(line, MAX_LINE_LENGTH, file) && count < MAX_ROWS) {
        char* token = strtok(line, ",");
        data[count].year = atoi(token);
        
        token = strtok(NULL, ",");
        data[count].internet_percentage = atof(token);
        
        token = strtok(NULL, ",\n");
        data[count].population = atoll(token);
        
        count++;
    }
    
    fclose(file);
    return count;
}

void separateData(DataRecord* data, int data_count, DataRecord* population_data, int* pop_count, 
                 DataRecord* internet_data, int* int_count) {
    *pop_count = 0;
    *int_count = 0;
    
    for (int i = 0; i < data_count; i++) {
        if (data[i].population > 0) {
            population_data[*pop_count] = data[i];
            (*pop_count)++;
        }
        
        if (data[i].internet_percentage > 0) {
            internet_data[*int_count] = data[i];
            (*int_count)++;
        }
    }
}

void findMissingYears(DataRecord* data, int data_count, int* missing_years, int* missing_count) {
    int min_year = data[0].year;
    int max_year = data[0].year;
    
    for (int i = 1; i < data_count; i++) {
        if (data[i].year < min_year) min_year = data[i].year;
        if (data[i].year > max_year) max_year = data[i].year;
    }
    
    *missing_count = 0;
    for (int year = min_year; year <= max_year; year++) {
        int found = 0;
        for (int i = 0; i < data_count; i++) {
            if (data[i].year == year) {
                found = 1;
                break;
            }
        }
        
        if (!found && *missing_count < 10) {
            missing_years[*missing_count] = year;
            (*missing_count)++;
        }
    }
}

PolynomialModel fitPolynomialRegression(DataRecord* data, int count, int col_type, int max_degree) {
    PolynomialModel best_model = {1, {0}, 0.0};
    
    for (int degree = 1; degree <= max_degree; degree++) {
        PolynomialModel current_model = {degree, {0}, 0.0};
        
        double** X = (double**)malloc((count) * sizeof(double*));
        for (int i = 0; i < count; i++) {
            X[i] = (double*)malloc((degree + 1) * sizeof(double));
            X[i][0] = 1.0;
            for (int j = 1; j <= degree; j++) {
                X[i][j] = pow(data[i].year, j);
            }
        }
        
        double* y = (double*)malloc(count * sizeof(double));
        for (int i = 0; i < count; i++) {
            if (col_type == 1) {
                y[i] = data[i].internet_percentage;
            } else {
                y[i] = (double)data[i].population;
            }
        }
        
        double** XTX = (double**)malloc((degree + 1) * sizeof(double*));
        double** XTy = (double**)malloc((degree + 1) * sizeof(double*));
        
        for (int i = 0; i <= degree; i++) {
            XTX[i] = (double*)malloc((degree + 1) * sizeof(double));
            XTy[i] = (double*)malloc(sizeof(double));
            XTy[i][0] = 0.0;
            
            for (int j = 0; j <= degree; j++) {
                XTX[i][j] = 0.0;
                for (int k = 0; k < count; k++) {
                    XTX[i][j] += X[k][i] * X[k][j];
                }
            }
            
            for (int k = 0; k < count; k++) {
                XTy[i][0] += X[k][i] * y[k];
            }
        }
        
        for (int i = 0; i <= degree; i++) {
            double diag = XTX[i][i];
            for (int j = 0; j <= degree; j++) {
                XTX[i][j] /= diag;
            }
            XTy[i][0] /= diag;
            
            for (int k = 0; k <= degree; k++) {
                if (k != i) {
                    double factor = XTX[k][i];
                    for (int j = 0; j <= degree; j++) {
                        XTX[k][j] -= factor * XTX[i][j];
                    }
                    XTy[k][0] -= factor * XTy[i][0];
                }
            }
        }
        
        for (int i = 0; i <= degree; i++) {
            current_model.coefficients[i] = XTy[i][0];
        }
        
        current_model.r_squared = calculateRSquared(data, count, current_model, col_type);
        
        printf("%s polynomial degree %d: R² score = %.4f\n", 
               (col_type == 1 ? "Internet usage" : "Population"), 
               degree, current_model.r_squared);
        
        if (current_model.r_squared > best_model.r_squared) {
            best_model = current_model;
        }
        
        for (int i = 0; i < count; i++) {
            free(X[i]);
        }
        free(X);
        free(y);
        
        for (int i = 0; i <= degree; i++) {
            free(XTX[i]);
            free(XTy[i]);
        }
        free(XTX);
        free(XTy);
    }
    
    printf("\nBest polynomial degree for %s: %d\n", 
           (col_type == 1 ? "internet usage" : "population"), 
           best_model.degree);
    
    return best_model;
}

double calculatePrediction(PolynomialModel model, int year) {
    double prediction = model.coefficients[0];
    
    for (int i = 1; i <= model.degree; i++) {
        prediction += model.coefficients[i] * pow(year, i);
    }
    
    return prediction;
}

double calculateRSquared(DataRecord* data, int count, PolynomialModel model, int col_type) {
    double mean = 0.0;
    double ss_total = 0.0;
    double ss_residual = 0.0;
    
    for (int i = 0; i < count; i++) {
        if (col_type == 1) {
            mean += data[i].internet_percentage;
        } else {
            mean += (double)data[i].population;
        }
    }
    mean /= count;
    
    for (int i = 0; i < count; i++) {
        double actual;
        if (col_type == 1) {
            actual = data[i].internet_percentage;
        } else {
            actual = (double)data[i].population;
        }
        
        double predicted = calculatePrediction(model, data[i].year);
        
        ss_total += pow(actual - mean, 2);
        ss_residual += pow(actual - predicted, 2);
    }
    
    if (ss_total == 0) {
        return 0.0;
    }
    
    return 1.0 - (ss_residual / ss_total);
}

void generatePlotData(PolynomialModel pop_model, PolynomialModel int_model, 
                      DataRecord* population_data, int pop_count,
                      DataRecord* internet_data, int int_count,
                      int* missing_years, int missing_count,
                      double* predicted_population, double* predicted_internet) {
    
    FILE* pop_actual_file = fopen("pop_actual.dat", "w");
    for (int i = 0; i < pop_count; i++) {
        fprintf(pop_actual_file, "%d %lf\n", population_data[i].year, 
                (double)population_data[i].population / 1000000.0); 
    }
    fclose(pop_actual_file);
    
    FILE* pop_pred_file = fopen("pop_pred.dat", "w");
    for (int year = 1960; year <= 2040; year++) {
        double predicted = calculatePrediction(pop_model, year);
        fprintf(pop_pred_file, "%d %lf\n", year, predicted / 1000000.0); 
    }
    fclose(pop_pred_file);
    
    FILE* pop_missing_file = fopen("pop_missing.dat", "w");
    for (int i = 0; i < missing_count; i++) {
        fprintf(pop_missing_file, "%d %lf\n", missing_years[i], 
                predicted_population[i] / 1000000.0); 
    }
    fclose(pop_missing_file);
    
    FILE* pop_future_file = fopen("pop_future.dat", "w");
    double predicted_pop_2030 = calculatePrediction(pop_model, 2030);
    double predicted_pop_2035 = calculatePrediction(pop_model, 2035);
    fprintf(pop_future_file, "%d %lf\n", 2030, predicted_pop_2030 / 1000000.0);
    fprintf(pop_future_file, "%d %lf\n", 2035, predicted_pop_2035 / 1000000.0);
    fclose(pop_future_file);
    
    FILE* int_actual_file = fopen("int_actual.dat", "w");
    for (int i = 0; i < int_count; i++) {
        fprintf(int_actual_file, "%d %lf\n", internet_data[i].year, 
                internet_data[i].internet_percentage);
    }
    fclose(int_actual_file);
    
    FILE* int_pred_file = fopen("int_pred.dat", "w");
    for (int year = 1994; year <= 2040; year++) {
        double predicted = calculatePrediction(int_model, year);
        fprintf(int_pred_file, "%d %lf\n", year, predicted);
    }
    fclose(int_pred_file);
    
    FILE* int_missing_file = fopen("int_missing.dat", "w");
    for (int i = 0; i < missing_count; i++) {
        fprintf(int_missing_file, "%d %lf\n", missing_years[i], predicted_internet[i]);
    }
    fclose(int_missing_file);
    
    FILE* int_future_file = fopen("int_future.dat", "w");
    double predicted_int_2035 = calculatePrediction(int_model, 2035);
    fprintf(int_future_file, "%d %lf\n", 2035, predicted_int_2035);
    fclose(int_future_file);
    
    FILE* users_actual_file = fopen("users_actual.dat", "w");
    for (int i = 0; i < int_count; i++) {
        long long pop = 0;
        for (int j = 0; j < pop_count; j++) {
            if (population_data[j].year == internet_data[i].year) {
                pop = population_data[j].population;
                break;
            }
        }
        if (pop > 0) {
            double users = (internet_data[i].internet_percentage / 100.0) * pop;
            fprintf(users_actual_file, "%d %lf\n", internet_data[i].year, users / 1000000.0);
        }
    }
    fclose(users_actual_file);
    
    FILE* users_pred_file = fopen("users_pred.dat", "w");
    for (int year = 1994; year <= 2040; year++) {
        double int_pred = calculatePrediction(int_model, year);
        double pop_pred = calculatePrediction(pop_model, year);
        
        if (int_pred > 0 && int_pred <= 100) {
            double users = (int_pred / 100.0) * pop_pred;
            fprintf(users_pred_file, "%d %lf\n", year, users / 1000000.0); 
        }
    }
    fclose(users_pred_file);
    
    FILE* users_missing_file = fopen("users_missing.dat", "w");
    for (int i = 0; i < missing_count; i++) {
        double users = (predicted_internet[i] / 100.0) * predicted_population[i];
        fprintf(users_missing_file, "%d %lf\n", missing_years[i], users / 1000000.0); 
    }
    fclose(users_missing_file);
    
    FILE* users_future_file = fopen("users_future.dat", "w");
    double estimated_users_2035 = (predicted_int_2035 / 100.0) * predicted_pop_2035;
    fprintf(users_future_file, "%d %lf\n", 2035, estimated_users_2035 / 1000000.0); 
    fclose(users_future_file);
}

void setupGNUplot() {
    FILE* gp_script = fopen("plot_script.gnu", "w");
    
    fprintf(gp_script, "set terminal pngcairo size 1800,1200 enhanced font 'Arial,12'\n");
    fprintf(gp_script, "set output 'Indonesia_Population_Internet_Analysis_C.png'\n");
    fprintf(gp_script, "set multiplot layout 2,2\n");
    fprintf(gp_script, "set grid\n");
    
    fclose(gp_script);
}

void createPopulationPlot() {
    FILE* gp_script = fopen("plot_script.gnu", "a");
    
    fprintf(gp_script, "# Population Growth Plot\n");
    fprintf(gp_script, "set title 'Population Growth in Indonesia (1960-2035)' font 'Arial,14'\n");
    fprintf(gp_script, "set xlabel 'Year'\n");
    fprintf(gp_script, "set ylabel 'Population (millions)'\n");
    fprintf(gp_script, "set key outside\n");
    fprintf(gp_script, "plot 'pop_actual.dat' using 1:2 with points pt 7 ps 1 lc rgb 'blue' title 'Actual Data', \\\n");
    fprintf(gp_script, "     'pop_pred.dat' using 1:2 with lines lw 2 lc rgb 'red' title 'Polynomial Fit', \\\n");
    fprintf(gp_script, "     'pop_missing.dat' using 1:2 with points pt 2 ps 2 lc rgb 'green' title 'Predicted Missing Values', \\\n");
    fprintf(gp_script, "     'pop_future.dat' using 1:2 with points pt 6 ps 2 lc rgb 'orange' title '2030/2035 Predictions'\n");
    
    fclose(gp_script);
}

void createInternetPercentagePlot() {
    FILE* gp_script = fopen("plot_script.gnu", "a");
    
    fprintf(gp_script, "# Internet Usage Percentage Plot\n");
    fprintf(gp_script, "set title 'Internet Usage Percentage in Indonesia (1994-2035)' font 'Arial,14'\n");
    fprintf(gp_script, "set xlabel 'Year'\n");
    fprintf(gp_script, "set ylabel 'Percentage of Population (%)'\n");
    fprintf(gp_script, "set key outside\n");
    fprintf(gp_script, "plot 'int_actual.dat' using 1:2 with points pt 7 ps 1 lc rgb 'blue' title 'Actual Data', \\\n");
    fprintf(gp_script, "     'int_pred.dat' using 1:2 with lines lw 2 lc rgb 'red' title 'Polynomial Fit', \\\n");
    fprintf(gp_script, "     'int_missing.dat' using 1:2 with points pt 2 ps 2 lc rgb 'green' title 'Predicted Missing Values', \\\n");
    fprintf(gp_script, "     'int_future.dat' using 1:2 with points pt 6 ps 2 lc rgb 'orange' title '2035 Prediction'\n");
    
    fclose(gp_script);
}

void createInternetUsersPlot() {
    FILE* gp_script = fopen("plot_script.gnu", "a");
    
    fprintf(gp_script, "# Number of Internet Users Plot\n");
    fprintf(gp_script, "set title 'Number of Internet Users in Indonesia (1994-2035)' font 'Arial,14'\n");
    fprintf(gp_script, "set xlabel 'Year'\n");
    fprintf(gp_script, "set ylabel 'Internet Users (millions)'\n");
    fprintf(gp_script, "set key outside\n");
    fprintf(gp_script, "plot 'users_actual.dat' using 1:2 with points pt 7 ps 1 lc rgb 'blue' title 'Actual Internet Users', \\\n");
    fprintf(gp_script, "     'users_pred.dat' using 1:2 with lines lw 2 lc rgb 'red' title 'Predicted Internet Users', \\\n");
    fprintf(gp_script, "     'users_missing.dat' using 1:2 with points pt 2 ps 2 lc rgb 'green' title 'Predicted Missing Values', \\\n");
    fprintf(gp_script, "     'users_future.dat' using 1:2 with points pt 6 ps 2 lc rgb 'orange' title '2035 Prediction'\n");
    
    fprintf(gp_script, "unset multiplot\n");
    fclose(gp_script);
    
    system("gnuplot plot_script.gnu");
}

void cleanupTempFiles() {
    remove("pop_actual.dat");
    remove("pop_pred.dat");
    remove("pop_missing.dat");
    remove("pop_future.dat");
    remove("int_actual.dat");
    remove("int_pred.dat");
    remove("int_missing.dat");
    remove("int_future.dat");
    remove("users_actual.dat");
    remove("users_pred.dat");
    remove("users_missing.dat");
    remove("users_future.dat");
    remove("plot_script.gnu");
}