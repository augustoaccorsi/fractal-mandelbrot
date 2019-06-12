# Cáculo de Fractal de Mandelbrot

## Processamento de Alto Desempenho (PAD) - Unisinos - Ciência da Computação

* O trabalho consiste em executar o calculo da Fractal de Mandelbrot através de ptreads

* Trabalho relizado na plataforma Windows, utilizando a o package [MinGW da Chocolatey](https://chocolatey.org/packages/mingw)

* O algoritimo irá criar quatro phtreads para formar a seguinte imagem de Fractal Mandelbrot

![alt text](https://raw.githubusercontent.com/augustoaccorsi/fractal-mandelbrot/master/fractal.bmp)

### Execução do programa

`make`
`fractal`

* É possível enviar parametros para que tenhamos uma Fractal de Mandelbrot melhor

|          Parametros                       |
--------------------------------------------|
| -m Número máximo de iterações por ponto   |
| -x Coordenada X do centro da imagem       |
| -x Coordenada y do centro da imagem       |                
| -s Escala da imagem                       |
| -W Largura da imagem em pixels            |
| -H Alura da imagem em pixels              |
| -o Arquivo de saída                       |
| -h Ajuda                                  |

* Exemplo de execução com os parametros

`fractal -x -0.5 -y 0 -s 2 -m 1000 -W 1000 -H 1000 -o fractal.bmp` 


