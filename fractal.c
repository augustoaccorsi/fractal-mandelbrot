#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

int iteration_to_color(int i, int max);
int iterations_at_point(double x, double y, int max);
void *compute_image(void *param_thread);
void show_help()
{
	printf("-m Número máximo de iterações por ponto (default=1000)\n");
	printf("-x Coordenada X do centro da imagem (default=0)\n");
	printf("-x Coordenada y do centro da imagem (default=0)\n");
	printf("-s Escala da imagem (default=4)\n");
	printf("-W Largura da imagem em pixels (default=500)\n");
	printf("-H Alura da imagem em pixels (default=500)\n");
	printf("-o Arquivo de saída (default=fractal.bmp)\n");
	printf("-h Ajuda\n");
	printf("\nExemplo:\n");
	printf("fractal -x -0.5 -y 0 -s 2 -m 1000 -W 1000 -H 1000 -o fractal.bmp");
}
struct bitmap *bm;

struct param_fractal
{
	double xmax;
	double xmin;
	double ymax;
	double ymin;
	double max;
	double w_min;
	double w_max;
	double h_min;
	double h_max;
	struct bitmap *bm;
};

int main(int argc, char *argv[])
{
	clock_t start, end;
	start = clock();
	char c;

	const char *outfile = "fractal.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int image_width = 500;
	int image_height = 500;
	int max = 1000;

	while ((c = getopt(argc, argv, "x:y:s:W:H:m:n:o:h")) != -1)
	{
		switch (c)
		{
		case 'x':
			xcenter = atof(optarg);
			break;
		case 'y':
			ycenter = atof(optarg);
			break;
		case 's':
			scale = atof(optarg);
			break;
		case 'W':
			image_width = atoi(optarg);
			break;
		case 'H':
			image_height = atoi(optarg);
			break;
		case 'm':
			max = atoi(optarg);
			break;
		case 'o':
			outfile = optarg;
			break;
		case 'h':
			show_help();
			exit(1);
			break;
		}
	}
	pthread_t up_left_thread, up_right_thread, down_left_thread, down_right_thread;

	struct param_fractal param1, param2, param3, param4;
	bm = bitmap_create(image_width, image_height);

	param1.xmin = (xcenter - scale);
	param1.xmax = (xcenter + scale);
	param1.ymin = (ycenter - scale);
	param1.ymax = (ycenter + scale);
	param1.h_min = 0.5;
	param1.h_max = 1;
	param1.w_min = 0;
	param1.w_max = 0.5;
	param1.max = max;

	param2.xmin = (xcenter - scale);
	param2.xmax = (xcenter + scale);
	param2.ymin = (ycenter - scale);
	param2.ymax = (ycenter + scale);
	param2.h_min = 0.5;
	param2.h_max = 1;
	param2.w_min = 0.5;
	param2.w_max = 1;
	;
	param2.max = max;

	param3.xmin = (xcenter - scale);
	param3.xmax = (xcenter + scale);
	param3.ymin = (ycenter - scale);
	param3.ymax = (ycenter + scale);
	param3.h_min = 0;
	param3.h_max = 0.5;
	param3.w_min = 0;
	param3.w_max = 0.5;
	;
	param3.max = max;

	param4.xmin = (xcenter - scale);
	param4.xmax = (xcenter + scale);
	param4.ymin = (ycenter - scale);
	param4.ymax = (ycenter + scale);
	param4.h_min = 0;
	param4.h_max = 0.5;
	param4.w_min = 0.5;
	param4.w_max = 1;
	;
	param4.max = max;

	if (pthread_create(&up_left_thread, NULL, compute_image, &param1))
	{
		printf("Error creating thread\n");
		return 1;
	}
	else
	{
		printf("Thread 1 up and running\n");
	}
	if (pthread_create(&up_right_thread, NULL, compute_image, &param2))
	{
		printf("Error creating thread\n");
		return 1;
	}
	else
	{
		printf("Thread 2 up and running\n");
	}
	if (pthread_create(&down_left_thread, NULL, compute_image, &param3))
	{
		printf("Error creating thread\n");
		return 1;
	}
	else
	{
		printf("Thread 3 up and running\n");
	}
	if (pthread_create(&down_right_thread, NULL, compute_image, &param4))
	{
		printf("Error creating thread\n");
		return 1;
	}
	else
	{
		printf("Thread 4 up and running\n");
	}

	printf("\n");

	if (pthread_join(up_left_thread, NULL))
	{
		printf("Error joining thread\n");
		return 2;
	}
	else
	{
		printf("Thread 1 executed successfully\n");
	}
	if (pthread_join(up_right_thread, NULL))
	{
		printf("Error joining thread\n");
		return 2;
	}
	else
	{
		printf("Thread 2 executed successfully\n");
	}
	if (pthread_join(down_left_thread, NULL))
	{
		printf("Error joining thread\n");
		return 2;
	}
	else
	{
		printf("Thread 3 executed successfully\n");
	}
	if (pthread_join(down_right_thread, NULL))
	{
		printf("Error joining thread\n");
		return 2;
	}
	else
	{
		printf("Thread 4 executed successfully\n");
	}

	printf("\n");

	if (!bitmap_save(bm, outfile))
	{
		printf("fractal: couldn't write to %s\n", outfile);
		return 1;
	}
	else
	{
		printf("Fractal Mandelbrot bitmap created as %s", outfile);
	}

	end = clock();
	double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

	printf("\nTempo de execucao: %fs", cpu_time_used);
	return 0;
}

void *compute_image(void *param_thread)
{
	struct param_fractal *param = (struct param_fractal *)param_thread;

	int i, j;

	int width = bitmap_width(bm);
	int height = bitmap_height(bm);

	double h_max = param->h_max;
	double h_min = param->h_min;
	double w_max = param->w_max;
	double w_min = param->w_min;
	double xmax = param->xmax;
	double xmin = param->xmin;
	double ymax = param->ymax;
	double ymin = param->ymin;
	double max = param->max;

	for (j = (height * h_min); j < (height * h_max); j++)
	{
		for (i = (width * w_min); i < (width * w_max); i++)
		{
			double x = xmin + i * (xmax - xmin) / width;
			double y = ymin + j * (ymax - ymin) / height;

			int iters = iterations_at_point(x, y, max);

			bitmap_set(bm, i, j, iters);
		}
	}
}

int iterations_at_point(double x, double y, int max)
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while ((x * x + y * y <= 4) && iter < max)
	{

		double xt = x * x - y * y + x0;
		double yt = 2 * x * y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter, max);
}

int iteration_to_color(int i, int max)
{
	int gray = 255 * i / max;
	return MAKE_RGBA(gray, gray, gray, 0);
}
