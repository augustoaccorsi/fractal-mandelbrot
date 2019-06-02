# Calcular o Fractal de Mandelbrot usando pthreads ou MPI.

## Primeiro, dividir a tela de pequenos quadrados que simbolizarão as tarefas. Cada tarefa tem (Xi; Yi; Xf; Yf). Os processos ou threads executam o ## Fractal de Mandelbrot para uma determinada tarefa e passam para o mestre (ou colocam em um buffer) o resultado. 
## O processo recebe um resultado e coloca na tela. A tela vai se completando de forma aleatória.
