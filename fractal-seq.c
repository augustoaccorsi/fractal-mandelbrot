#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <time.h>

int iteration_to_color(int i, int max);
int iterations_at_point(double x, double y, int max);
void compute_image(struct bitmap *bm, double xmax, double xmin, double ymax, double ymin, double max);

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
	printf("fractal-seq -x -0.5 -y 0 -s 2 -m 1000 -W 1000 -H 1000 -o fractal-seq.bmp");
}

int main(int argc, char *argv[])
{
	clock_t start, end;
	start = clock();

	char c;

	const char *outfile = "fractal-seq.bmp";
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

	struct bitmap *bm = bitmap_create(image_width, image_height);

	compute_image(bm, (xcenter + scale), (xcenter - scale), (ycenter + scale), (ycenter - scale), max);

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

void compute_image(struct bitmap *bm, double xmax, double xmin, double ymax, double ymin, double max)
{
	int i, j;

	int width = bitmap_width(bm);
	int height = bitmap_height(bm);

	for (j = 0; j < height; j++)
	{

		for (i = 0; i < width; i++)
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
