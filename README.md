Relatório de Implementação: Algoritmo Genético para Ajuste Linear
1. Introdução e Objetivos
Este projeto foi desenvolvido como parte da disciplina de Algoritmos e Estruturas de Dados I. O objetivo central é aplicar conceitos de manipulação de vetores, estruturas dinâmicas e lógica iterativa para resolver um problema de ajuste de função linear. A meta-heurística utilizada é o Algoritmo Genético (AG), que busca otimizar os parâmetros de uma reta para que ela se ajuste da melhor forma possível a um conjunto de pontos fixos no plano cartesiano.


3. Especificação Técnica e Estruturas de Dados
O algoritmo trabalha sobre um conjunto de dados D composto por n observações (x,y) fornecidas via arquivo de entrada. As principais estruturas utilizadas no código C++ são:


Representação dos Pontos: Uma estrutura Point que armazena as coordenadas reais de cada amostra.


População: A população de tamanho m é representada por um conjunto de indivíduos, onde cada Individual codifica dois genes: o coeficiente angular (a) e o coeficiente linear (b).



Vetores Auxiliares: Foram utilizados vetores dinâmicos para gerenciar a pontuação de fitness e o erro acumulado de cada solução candidata.

3. Lógica do Processo Evolutivo
O ciclo de evolução implementado segue rigorosamente as etapas de um AG clássico:


Avaliação e Fitness: Para cada indivíduo, o programa calcula o Erro Quadrático Médio (MSE), que mede a distância vertical média entre a reta estimada ( 
y
^
​
 =ax+b) e os pontos reais. O fitness é definido de forma inversamente proporcional ao erro: quanto menor o MSE, maior a aptidão do indivíduo.



Seleção e Reprodução: A cada geração, os dois indivíduos com as maiores pontuações de fitness são selecionados para a reprodução.


Operadores Genéticos: O Crossover combina os parâmetros dos pais para gerar um novo descendente (ex: combinando o gene a do primeiro pai com o gene b do segundo). A Mutação introduz diversidade ao aplicar uma pequena variação controlada (δ) em um dos genes, evitando que o algoritmo fique preso em soluções locais.



Atualização da População: Utiliza-se uma estratégia de substituição onde o novo indivíduo gerado toma o lugar do espécime com menor fitness na população atual.

4. Instruções de Compilação e Execução (Ambiente Linux)
Seguindo as orientações de execução obrigatória em ambiente Linux (como Ubuntu ou Zorin), o projeto inclui um Makefile para automatizar o processo.

Compilação: No terminal, basta digitar o comando make. Isso invocará o compilador g++ com as flags de otimização necessárias.


Execução: O programa lê os parâmetros n (pontos), m (população) e G (gerações) do arquivo input.dat. Para rodar, utilize ./programa.


Saída: Os resultados de cada geração, incluindo o melhor fitness, o erro associado e os valores finais de a e b, são registrados automaticamente no arquivo output.dat.
