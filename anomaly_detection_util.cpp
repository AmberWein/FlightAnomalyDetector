// Amber Weiner 208783522

#include "anomaly_detection_util.h"
#include <math.h>

/**
* This function finds the average value of a given array of float numbers.
 * @param x - a given array
 * @param size - x's size
 * @return the average of x.
 */
float avg(float* x, int size){
    float sum_of_x = 0;
    // a for loop to sum all the numbers in X
    for (int i = 0; i < size ; ++i) {
        sum_of_x += x[i];
    }
    return sum_of_x / size;
}

/**
* This function finds the variance of a given array of float numbers and return it.
 * @param x - a given array
 * @param size - x's size
 * @return the variance of x.
 */
float var(float* x, int size){
    float sum_of_squared_sum = 0;
    float average_of_squared_sum, average_of_x;
    // a for loop to sum the squared x's values
    for(int i = 0 ; i < size ; i++) {
        sum_of_squared_sum += pow(x[i], 2);
    }
    // compute two different averages
    average_of_x = avg(x, size);
    average_of_squared_sum = sum_of_squared_sum / size;

    return (average_of_squared_sum - pow(average_of_x, 2));
}

/**
* This function finds the covariance of x and y and return it.
 * @param x - a given array
 * @param y - a given array
 * @param size - x's and y's size
 * @return the covariance of x and y.
 */
float cov(float* x, float* y, int size){
    float total_sum = 0;
    float average_of_x = avg(x, size);
    float average_of_y = avg(y, size);
    // compute the sum of the upper side of the equation
    for(int i = 0 ; i < size ; i++) {
        total_sum += (x[i] - average_of_x) * (y[i] - average_of_y);
    }
    return total_sum / size;
}

/**
* This function finds pearson correlation coefficient of x and y and return it.
 * @param x - a given array
 * @param y - a given array
 * @param size - x's and y's size
 * @return the pearson correlation coefficient of x and y.
 */
float pearson(float* x, float* y, int size){
    float cov_of_x_and_y = cov(x, y, size);
    float var_of_x = var(x, size);
    float var_of_y = var(y, size);
    return cov_of_x_and_y / (sqrt(var_of_x) * sqrt(var_of_y));
}

/**
* This function performs a linear regression and returns the line equation.
 * @param points - a given array of pointers to points
 * @param size - points's size
 * @return the line equation of a linear regression.
 */
Line linear_reg(Point** points, int size){
    float x[size], y[size];
    // a for loop to go the points to fill the x and y arrays
    for (int i = 0; i < size; i++) {
        x[i] = points[i]->x;
        y[i] = points[i]->y;
    }
    float a = cov(x, y, size) / var(x, size);
    float avg_of_x = avg(x, size);
    float avg_of_y = avg(y, size);
    float b = avg_of_y - (a * avg_of_x);
    Line line = Line(a, b);
    return line;
}

/**
 * @param number - a given value
 * @return the absolute value of a given value.
 */
float absolute(float number) {
if (number >= 0)
    return number;
return (-1) * number;
}

/**
 * @param points - an array of pointers to points
 * @param p - a point
 * @param size - points' size
 * @return the deviation between point p and the line equation of the points.
 */
float dev(Point p,Point** points, int size) {
    Line line = linear_reg(points, size);
    float f_of_x = line.a * p.x + line.b;
    return absolute(f_of_x - p.y);
}

/**
 * @param p - a point
 * @param l - a line
 * @return the deviation between point p and the line.
 */
float dev(Point p,Line l){
    float f_of_x = l.a * p.x + l.b;
    return absolute(f_of_x - p.y);
}
