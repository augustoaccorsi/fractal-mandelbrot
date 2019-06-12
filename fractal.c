
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
void compute_image(struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max);
void *execute_ptreads(int image_width, int image_height 	, double xmin, double xmax, double ymin, double ymax, int max, double xcenter, double ycenter, double scale);


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
	printf("-n <threads> Number of threads that will be used to create the Mandelbrot\n");
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
	int number_of_threads = 1;
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
		case 'n':
			number_of_threads = atoi(optarg);
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

	// Display the configuration of the image.
	//printf("fractal: x=%lf y=%lf scale=%lf max=%d outfile=%s\n",xcenter,ycenter,scale,max,outfile);
	int aux = image_height;
	int value = aux / number_of_threads;

	for (int i = 0; i < number_of_threads; i++)
	{
		aux = aux - value;
		if (aux < value)
			value = aux + value;

		printf("\nphtread-%d valor:%d", i + 1, value);

		/* create a second thread which executes inc_x(&x) */
	}

	pthread_t inc_x_thread;
	int x=0;

	if (pthread_create(&inc_x_thread, NULL, execute_ptreads(image_width, image_height, (xcenter - scale), (xcenter + scale), (ycenter - scale), (ycenter + scale), max, xcenter, ycenter, scale), &x))
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	if (pthread_join(inc_x_thread, NULL))
	{

		fprintf(stderr, "Error joining thread\n");
		return 2;
	}

	//execute_ptreads(number_of_threads, image_height, image_width);

	// Create a bitmap of the appropriate size.
	//bitmap *bm = bitmap_create(image_width, image_height);

	// Fill it with a dark blue, for debugging
	//bitmap_reset(bm, MAKE_RGBA(0, 0, 255, 0));

	// Compute the fractalbrot image
	//compute_image(bm, xcenter - scale, xcenter + scale, ycenter - scale, ycenter + scale, max);

	// Save the image in the stated file.

	return 0;
}

/*
Compute an entire fractalbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max)
{
	int i, j;

	int width = bitmap_width(bm);
	int height = bitmap_height(bm);

	// For every pixel in the image...

	for (j = 0; j < height; j++)
	{

		for (i = 0; i < width; i++)
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

void *execute_ptreads(int image_width, int image_height, double xmin, double xmax, double ymin, double ymax, int max, double xcenter, double ycenter, double scale)
{
	struct bitmap *bm = bitmap_create(image_width, image_height);
	printf("\n%d\n%d", image_height, image_width);

	// Fill it with a dark blue, for debugging
	//bitmap_reset(bm, MAKE_RGBA(0, 0, 255, 0));

	// Compute the fractalbrot image
	compute_image(bm, xcenter - scale, xcenter + scale, ycenter - scale, ycenter + scale, max);

	const char *outfile = "fractal.bmp";
	
	if (!bitmap_save(bm, outfile))
	{
		fprintf(stderr, "fractal: couldn't write to %s: %s\n", outfile, strerror(errno));
		return 1;
	}
}
