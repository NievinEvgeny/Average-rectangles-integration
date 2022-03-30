#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <sys/time.h>

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

double func(double x)
{
    return log(1 + x) / x; // формула (можно менять)
}

void serial()
{
    const double eps = 1E-5;
    const double a = 0.1;
    const double b = 1;
    const int n0 = 100000000;
    int n = n0, k;

    double sq[2], delta = 1;
    double t = wtime();
    for (k = 0; delta > eps; n *= 2, k ^= 1)
    {
        double h = (b - a) / n;
        double s = 0.0;
        for (int i = 0; i < n; i++)
            s += func(a + h * (i + 0.5));
        sq[k] = s * h;
        if (n > n0)
            delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
    }
    printf("Result Pi: %.12f; Runge rule: EPS %e, n %d\n", sq[k] * sq[k], eps, n / 2);
    t = wtime() - t;
    printf("Elapsed time (sec.): %.6f\n", t);
}

void parallel()
{
    const double eps = 1E-5; // заданная точность
    const double a = 0.1;
    const double b = 1;
    const int n0 = 100000000;

    double sq[2];
    double t = omp_get_wtime();
#pragma omp parallel num_threads(8) // задаём кол-во потоков
    {
        int n = n0, k;
        double delta = 1;
        for (k = 0; delta > eps; n *= 2, k ^= 1) // n*=2, т.к. берётся всё больше и больше точек пока не будет достигнута нужная точность (Пр. Рунге).
        {
            double h = (b - a) / n;
            double s = 0.0;
            sq[k] = 0;
#pragma omp barrier // ждём пока все потоки обнулят sq[k].
#pragma omp for nowait
            for (int i = 0; i < n; i++)
                s += func(a + h * (i + 0.5));
#pragma omp atomic // Гарантируем корректную работу с общей переменной, стоящей в левой части оператора присваивания.
            sq[k] += s * h;
#pragma omp barrier
            if (n > n0)
                delta = fabs(sq[k] - sq[k ^ 1]) / 3.0; // /3.0, т.к. формула прямоугольников (Пр. Рунге); sq[k^1], т.к. если k=0, то k^1=1 и наоборот.
        }
#pragma omp master
        printf("Result Pi: %.12f; Runge rule: EPS %e, n %d\n", sq[k] * sq[k], eps, n / 2);
    }
    t = omp_get_wtime() - t;
    printf("Elapsed time (sec.): %.6f\n", t);
}

int main()
{
    // serial();
    parallel();
}