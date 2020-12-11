#!/usr/bin/python
import Tkinter as tk
from tkFileDialog import asksaveasfilename
import datetime
import sys

import threading
from time import sleep

start = datetime.datetime.now()

def mandelbrot(z, c):
    return z**2 + c

def julia(constant):
    def julia_function(z, c):
        return z*z + constant
    return julia_function

class MandelbrotWindow(tk.Tk):
    """ A window object which creates an image and draws a fractal. """

    def __init__(self, iter_func=mandelbrot, width=640, height=480, x_range=(-2.0, 1.0), 
            max_iter=250, threads=8, *args, **kwargs):

        self.max_iter = 8 * 30
        print("Number of Threads: "+str(threads))

        tk.Tk.__init__(self, *args, **kwargs)

        # Iteration function for the fractal
        self.iter_func = iter_func

        # Store the mapped range in the complex plane
        self.x_range = x_range
        y_len = (x_range[1] - x_range[0]) * float(height)/width
        self.y_range = (-y_len/2, y_len/2)
        
        self.width = width
        self.height = height

        self.title("Fractal Mandelbrot Set "+str(width)+"x"+str(height))

        # Create the image
        self.canvas = tk.Canvas(self, width=width, height=height, bg= '#000000')
        self.img = tk.PhotoImage(width=width, height=height)
        self.canvas.create_image((0, 0), image=self.img, state='normal', anchor=tk.NW)

        self.points = ( (kx, ky) for kx in range(width) for ky in range(height))
        self.max_iter = max_iter

        # Create a lock and rendering threads for the image
        self.lock = threading.Lock()
        self.render_threads = [ComputationThread(self) for x in range(threads)]
    
    def run(self):
        self.canvas.pack()
        # Create a notifier thread
        render_notifier = NotificationThread("Rendering",
                threads=self.render_threads)

        # Begin rendering
        for t in self.render_threads:
            t.start()
        render_notifier.start()

class NotificationThread(threading.Thread):
    """ 
    Object for notifying the user that some task run by 
    some other worker threads is completed. 
    """

    def __init__(self, task, threads=None):
        super(NotificationThread, self).__init__()

        self.task = task
        self.threads = threads
        self.daemon = True

    def run(self):
        working = [t for t in self.threads if t.is_alive()]
        while working:
            working = [t for t in self.threads if t.is_alive()]
           # sleep(2)
        self.notify()
    
    def notify(self):
        notification_msg = "{} is complete".format(self.task)
        print(notification_msg)
        
        end = datetime.datetime.now()    
        print("Rendering Time: "+str(end - start))

        # Create a widget that pops up
        top = tk.Toplevel()
        top.title("Notification!")

        msg = tk.Message(top, text=notification_msg)
        msg.pack()

        button = tk.Button(top, text="Dismiss", command=top.destroy)
        button.pack()

class ComputationThread(threading.Thread):
    """ Threading object that does all of the iterative analysis. """

    def __init__(self, fractalwindow):
        super(ComputationThread, self).__init__()
        self.parent = fractalwindow
        self.daemon = True

    def run(self):
        while True:
            try:
                self.parent.lock.acquire()
                point = self.parent.points.next()
                self.compute(point)

                self.parent.lock.release()
            except StopIteration:
                self.parent.lock.release()
                break

    def compute(self, point):
        """ Iterates the iteration function over a point to check for escape. """
        c = self.convert_to_complex(point)
        z = complex(0.0, 0.0)

        for i in range(self.parent.max_iter):
            z = self.parent.iter_func(z, c)
            if abs(z) >= 6.0:
                break
        self.parent.img.put(self.get_color(i), point) 

    def convert_to_complex(self, point):
        """ Converts a position in the image to its equivalent complex number. """
        xa, xb = self.parent.x_range
        ya, yb = self.parent.y_range

        xc = xa + (xb - xa) * point[0] / self.parent.width 
        yc = ya + (yb - ya) * point[1] / self.parent.height
        return complex(xc, yc)

    def get_color(self, i):
        """ Returns the color given the number of iterations at that point."""
        t = float(i)/self.parent.max_iter

        r = hex(int((9 * (1-t) * t**3) * 255))[2:].zfill(2)
        g = hex(int((15 * ((1-t)*t)**2) * 255))[2:].zfill(2)
        b = hex(int((8.5 * (1-t)**3 * t) * 255))[2:].zfill(2)

        return '#' + r + g + b

def main(threads, width, height):
    # Draw a 1440 x 900 image with 1000 iterations
    win = MandelbrotWindow(width=width, height=height, max_iter=500,\
            threads=threads)
    win.run()
    win.mainloop()

if __name__ == '__main__':
    
    threads = 15
    width = 350
    height = 350

    if len(sys.argv) == 4:
        if sys.argv[1] != "":
            threads = int(sys.argv[1])
        
        if sys.argv[2] != "":
            width = int(sys.argv[2])
        
        if sys.argv[3] != "":
            height = int(sys.argv[3])

    main(threads, width, height)
    
    