# Breakout
Breakout game in C/SDL2.

# Projeto de 2017/1 de Computação para Informática
Professor: Adriano Joaquim de Oliveira Cruz
19 de maio de 2017

# 1.Informações Gerais
Neste trabalho, os alunos terão de desenvolver um jogo em linguagem C, utilizando obrigatoriamente a biblioteca gra ́fica SDL2. O jogo poderá ser desenvolvido em grupos de até três alunos.
Além do executável e dos fontes do programa, os alunos deverão elaborar e entregar um manual do usuário, contendo informações sobre como instalar e executar o jogo, bem como informações sobre o jogo em si.
Há várias versões deste jogo conhecido como breakout.
O site http://www.bgames.com/breakout-games/ contem várias versões diferentes deste jogo.

# 2. Avaliação
O jogo será avaliado como um todo, ou seja, os requisitos não receberão pontuações individualmente. Dessa forma, a falta de um ou mais requisitos acarretará na perda de pontos, que poderá ser compensada (não totalmente, claro) através de outros componentes bem desenvolvidos.
Componentes adicionais serão muito bem vistos, desde que implementados de maneira racional. Lembre-se de usar o bom senso para não transformar criatividade em bagunça.

# 3. O Jogo
O jogo é composto pelos seguintes elementos:
• Uma area de jogo (retangulo menor do que a tela), delimitada por paredes (as arestas do retangulo).
• Uma bolinha, que se movimenta com uma certa velocidade e é rebatida pelas paredes da area de jogo.
• Uma plataforma, localizada na parte inferior da area de jogo, com movimento horizontal e controlado pelo jogador. A plataforma tambem rebate a bolinha.
• Uma certa quantidade de tijolos, localizados na parte superior da area de jogo, imoveis e que tambem rebatem a bolinha.
O objetivo do jogo e destruir todos os tijolos da  ́area de jogo, utilizando, para isto, a bolinha. Um tijolo é destruído após rebater a bolinha. Para cada tijolo destruído, o jogador recebe 100 pontos. Alem disso, o tijolo destruído deve deixar de aparecer na area de jogo.
A bolinha, no entanto, nao deve encostar na parede inferior da area de jogo. Para evitar que isto aconteça, o jogador deve controlar a plataforma e fazer com que a bolinha rebata na plataforma e volte para a parte superior da area de jogo.
Caso o jogador nao seja bem sucedido e a bolinha entre em contato com a parede inferior, o numero de vidas restantes do jogador é decrementado. Quando este numero atinge o valor zero, o jogo termina.
Quando todos os tijolos sao destruídos, um novo nível (com os tijolos novamente dispostos na parte superior da  ́area de jogo) é iniciado e o jogador recebe 1000 pontos.
A cada 10000 pontos, o numero de vidas restantes do jogador é incrementado.

# 4. Requisitos
A seguir, há alguns requisitos do jogo a ser implementado.

# 4.1 Tela Inicial
O jogo deve possuir uma tela inicial, que deve ser apresentada ao jogador antes da tela com o jogo em si, bem como ao final do jogo.
Esta tela deve conter o nome do jogo, os nomes dos integrantes do grupo e poss ́ıveis op ̧c ̃oes para o jogo.
# 4.2 Plataforma
A plataforma deve ser ser controlada pelo jogador atraves das teclas direcionais (esquerda e direita). O movimento da plataforma deve estar limitado as paredes da area de jogo, de modo que aquela fique sempre no interior desta.
# 4.3 Bolinha
Como mencionado, a bolinha deve ter uma determinada velocidade, que pode ser de- composta em suas componentes horizontal (vx) e vertical (vy), de modo que estas sejam tratadas de forma independente.
Caso a componente vx seja positiva, a bolinha ira para a direita; caso seja negativa, a bolinha ira para a esquerda. Caso a componente vy seja positiva, a bolinha ira para baixo; caso seja negativa, a bolinha ira para cima.
Assim, caso a bolinha colida com uma das paredes laterais (esquerda ou direita), devemos apenas trocar o sinal da componente vx. Analogamente, caso a bolinha colida com a parede superior (ou, se for o caso, com a parede inferior), trocamos o sinal da componente vy.
Do mesmo modo, caso a bolinha colida com a plataforma ou com um tijolo, devemos trocar o sinal da componente em questao.
Alem disso, a colisao com a plataforma deve influenciar nao so o sinal da componente vx (se for o caso), mas tambem o seu modulo, dependendo do local em que a bolinha tocar a plataforma (metade esquerda ou direita). A tabela 1 mostra como essa componente da velocidade é influenciada. Repare que o fator f, que multiplica ou divide vx, deve ser fixo.
# 4.4 Quadro de Pontuação
Na tela, ao lado da area de jogo, deve ser apresentado um quadro de pontuação, onde estarao o nível atual, a pontuação e o numero de vidas restantes do jogador.

# 5. Componentes Adicionais
O grupo que seguir e implementar os requisitos de maneira correta recebera uma nota razoavel. Os grupos estao livres para adicionar outros componentes ao jogo para que recebam notas melhores. Por ́em, antes de fazer altera ̧c ̃oes radicais em relaçao ao que foi pedido, leia a seção 2.
Alguns possíveis componentes adicionais:
• O fator f que modifica a componente vx pode variar de acordo com o ponto em que a bolinha toca a (metade da) plataforma. Assim, f pode ser menor quando a bolinha tocar um ponto mais proximo do meio da plataforma e maior quando a bolinha tocar um ponto mais proximo da ponta da plataforma.
• Os tijolos podem possuir diferentes resistencias, que indicam quantas vezes a bolinha deve colidir com o tijolo para que este seja destruído. As diferentes resistencias podem ser representadas por diferentes cores de tijolos. Caso tenhamos tijolos com diferentes resistencias, podemos ter níveis com dificuldades diferentes, onde níveis mais difíceis teriam tijolos mais resistentes que níveis mais faceis.
• Os tijolos podem ter, durante intervalos de tempo específicos, propriedades especiais, de modo que, quando destruídos dentro deste intervalo de tempo, dêem algum benefício ao jogador.
• As paredes da area de jogo podem ter fendas, o que transformaria a area de jogo num cilindro e faria a bolinha desaparecer de um lado e reaparecer do outro, ao inves de ser rebatida, caso entrasse em contato com uma dessas fendas.
• Opção para dois jogadores. Neste modo de jogo, poder ́ıamos ter, por exemplo, tijolos sendo adicionados a area de um jogador a medida que o outro jogador destroi os seus.
