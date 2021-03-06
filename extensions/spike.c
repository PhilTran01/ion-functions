#include <stdio.h>


static inline double double_max(double a, double b)
{
    return (a > b) ? a : b;
}

static inline double double_abs(double a)
{
    return (a > 0) ? a : -a;
}


static signed char window_spike(
            const double *sub_array, 
            size_t window_len, 
            size_t window_index, 
            double N, 
            double ACC) 
{
    size_t j=0;
    double focus;
    double max=0;
    double min=0;
    double mean=0;
    double R=0;
    char initialized=0;
    for(j=0;j<window_len;j++) {
        if(j==window_index) { 
            /*
             * If j is the focus of this window then set it and continue
             */
            focus = sub_array[j];
            continue;
        }
        if(!initialized) {
            /*
             * If we haven't initialized max, min do so now
             */
            max = min = sub_array[j]; 
            initialized = 1;
        }
        if(sub_array[j] > max)
            /*
             * Keep track of the max value in the sub_array
             */
            max = sub_array[j];
        if(sub_array[j] < min)
            /*
             * Keep track of the min as well
             */
            min = sub_array[j];
        /*
         * Sum the elements to make a mean later
         */
        mean+= sub_array[j];
    }
    mean = mean/(window_len-1);
    R = max - min;
    R = double_max(R, ACC);
    if( double_abs(focus - mean) > (N*R)) {
        /*
         * If the deviation of the focus exceeds N*R then it is a spike value
         */
        return 0;
    }
    return 1;
}

/*
 * spike
 *
 * This algorithm generates flags for data values according to whether a single
 * data value deviates significantly from surrounding data values. The purpose
 * of this document is to serve as a reference in order to document which
 * processing steps have been applied to a data product.
 *
 * Arguments:
 * signed char *out  - The output array of flags
 * const double *dat - The data vector
 * size_t len        - Length of the data vector
 * int L             - Window Length
 * double N          - Range multipier
 * double ACC        - accuracy
 */
int spike(signed char *out, const double *dat, size_t len, int L, double N, double ACC) 
{
    size_t i;
    size_t L2 = L/2;
    if(len < L) {
        return 0;
    }
    for(i=0;i<=(len-L);i++) {
        /*
         * Iterate through the main (center) part of the array
         */
        out[i+L2] = window_spike(dat + i, L, L2, N, ACC);
    }
    for(i=0;i<L2;i++) {
        /*
         * Do the beginning
         */
        out[i] = window_spike(dat, L, i, N, ACC);
    }
    for(i=L2;i<L;i++) {
        /*
         * Do the ending
         */
        out[(len-L) + i] = window_spike(dat + (len-L), L, i, N, ACC);
    }
    return 1;
}

