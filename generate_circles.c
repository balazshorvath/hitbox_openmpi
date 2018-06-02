#include <stdio.h>
#include <time.h>
#include <stdlib.h>

double double_rand(double min, double max) {
    double scale = rand() / (double) RAND_MAX;
    return min + scale * ( max - min );
}

int main(){
    int i;
    FILE* fd = fopen("circle.csv", "w");
    srand(time(NULL));
    int count = 100000;
    fprintf(fd, "%d\n", count);
    for(i = 0; i < count; i++){
        double x, y, r;
        x = double_rand(0.0, 10000.0);
        y = double_rand(0.0, 10000.0);
        r = double_rand(1.0, 10.0);
        fprintf(fd, "%d,%.4f,%.4f,%.4f\n", i, x, y, r);
    }
    fclose(fd);
    return 0;
}
