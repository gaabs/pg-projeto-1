P2-11: Gráfico da Curvatura da Curva de Bézier Interativo:
 
Descrição: o usuário entra via mouse com os pontos de controle de uma curva de Bézier.
O número de pontos de controle é arbitrário, sem limite.
O sistema desenha a curva correspondente, e, numa outra janela, desenha o gráfico da
curvatura da curva (normalizada para caber na janela). O usuário pode modificar o
posicionamento dos pontos, deletar e inserir pontos, e o sistema responder em tempo real
de forma adequada, reconstruindo a curva correspondente. O usuário poderá suprimir os
pontos de controle, a poligonal de controle, e os pontos da curva. O usuário também
poderá determinar o número de avaliações que deverá ser usado para então o sistema
calcular os correspondentes pontos da curva e ligá-los por retas. As avaliações deverão
ser feitas obrigatoriamente com o Algoritmo de de Casteljau, tanto para a curva quanto
para o cálculo das derivadas.