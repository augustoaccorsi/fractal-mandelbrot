
#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int iteration_to_color(int i, int max);
int iterations_at_point(double x, double y, int max);
void *compute_image(struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max, double height_min_divider, double height_max_divider, double width_min_divider, double width_max_divider);

void show_help()
{
	printf("Use: fractal [options]\n");
	printf("Where options are:\n");
	printf("-m <max>     The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>   X coordinate of image center point. (default=0)\n");
	printf("-y <coord>   Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>   Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels>  Width of the image in pixels. (default=500)\n");
	printf("-H <pixels>  Height of the image in pixels. (default=500)\n");
	printf("-o <file>    Set output file. (default=fractal.bmp)\n");
	printf("-h           Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("fractal -x -0.5 -y -0.5 -s 0.2\n");
	printf("fractal -x -.38 -y -.665 -s .05 -m 100\n");
	printf("fractal -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main(int argc, char *argv[])
{
	char c;
	// These are the default configuration values used
	// if no command line arguments are given.

	const char *outfile = "fractal.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int image_width = 500;
	int image_height = 500;
	int max = 1000;

	// For each command line argument given,
	// override the appropriate configuration value.

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

	pthread_t up_left_thread, up_right_thread, down_left_thread, down_right_thread;
	int x = 0, y = 0, z = 0, w = 0;

	if (pthread_create(&up_left_thread, NULL, compute_image(bm, (xcenter - scale), (xcenter + scale), (ycenter - scale), (ycenter + scale), max, 0.5, 1, 1, 0.5), &x))
	{
		printf("Error creating thread\n");
		return 1;
	}
	else
	{
		printf("Thread 1 up and running\n");
	}
	if (pthread_create(&up_right_thread, NULL, compute_image(bm, (xcenter - scale), (xcenter + scale), (ycenter - scale), (ycenter + scale), max, 0.5, 1, 0.5, 1), &y))
	{
		printf("Error creating thread\n");
		return 1;
	}
	else
	{
		printf("Thread 2 up and running\n");
	}
	if (pthread_create(&down_left_thread, NULL, compute_image(bm, (xcenter - scale), (xcenter + scale), (ycenter - scale), (ycenter + scale), max, 1, 0.5, 1, 0.5), &z))
	{
		printf("Error creating thread\n");
		return 1;
	}
	else
	{
		printf("Thread 3 up and running\n");
	}
	if (pthread_create(&down_right_thread, NULL, compute_image(bm, (xcenter - scale), (xcenter + scale), (ycenter - scale), (ycenter + scale), max, 1, 0.5, 0.5, 1), &w))
	{
		printf("Error creating thread\n");
		return 1;
	}
	else
	{
		printf("Thread 4 up and running\n");
	}

	printf("\n\n");

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

	printf("\n\n");

	if (!bitmap_save(bm, outfile))
	{
		printf("fractal: couldn't write to %s\n", outfile);
		return 1;
	}
	else
	{
		printf("Fractal Mandelbrot bitmap created as %s", outfile);
	}

	return 0;
}

/*
Compute an entire fractalbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void *compute_image(struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max, double height_min_divider, double height_max_divider, double width_min_divider, double width_max_divider)
{
	int i, j;

	int width = bitmap_width(bm);
	int height = bitmap_height(bm);

	// For every pixel in the image...

	for (j = (height * height_min_divider); j < (height * height_max_divider); j++)
	{

		for (i = (width * width_min_divider); i < (width * width_max_divider); i++)
		{

			// Determine the point in x,y space for that pixel.
			double x = xmin + i * (xmax - xmin) / width;
			double y = ymin + j * (ymax - ymin) / height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x, y, max);

			// Set the pixel in the bitmap.
			bitmap_set(bm, i, j, iters);
		}
	}
}

/*
Return the number of iterations at point x, y
in the fractalbrot space, up to a maximum of max.
*/

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

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color(int i, int max)
{
	int gray = 255 * i / max;
	return MAKE_RGBA(gray, gray, gray, 0);
}
