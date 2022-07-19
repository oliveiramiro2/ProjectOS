## Projeto Sistemas Operacionais
# Gerenciamento de páginas da cache
<div align='center'>
    <p>
        Fifo - Second Chance - NRU - Aging - MFU
    </p>
</div>
<h2>Projeto de implementação de algoritmos de substituição de página na memoria cache 🚀</h2>
<p>Nome: Ramiro Mares de Oliveira</p>
<p>Matrícula: 6312</p>
<hr />
<div align='center'>
    <h1>Lembrando que o código só é compilado em ambiente UNIX de preferência ubuntu(É possível rodar em windows com um terminal ubuntu)</h1>
      <br />
     <h3><bold>Como usar o programa:</bold></h3>
      <h4>Baixe o repositório com o comando:</h4>
     <p>git clone https://github.com/oliveiramiro2/ProjectOS.git.</p>
     <p>Procure sua respectiva pasta e abra um prompt de comando e digite:</p>
     <p> "gcc -Wall vmm.c -o vmm" ou "gcc -Wall main.c -o vmm" e então o progrma será compilado e isso gerará um executável.</p>
     <p>Após realizada a compilação podemos executar o programa com:</p>
     <p> "./vmm random 10 < anomaly.dat".</p>
     <br />
     <div align='start'>
          <ul>
               <legend><h3>Parâmetros iniciando programa:</h3></legend>
               <li>1º  É o nome do arquivo compilado "./main"</li>
               <li>2º  É o algoritmo de substituição de página sendo possível escolher entre ['random' | 'fifo' | 'second_chance' | 'nru' | 'aging' | 'mfu' ]</li>
               <li>3º  Número de solicitações que vai acontecer a cada ciclo de clock. Ex 10 -> serão feitas 10 requisições de endereços em cada ciclo.</li>
               <li>4º  E o ultimo é o arquivo anomaly.dat que entregará ao programa o número de páginas virtuais e reias assim como os endereços virtuais solicitados e tipo de acesso</li>
          </ul>
     </div>
     <br />
     <p>Após o comando executável ser rodado o programa apenas irá processar seus dados e devolverá um número inteiro correspondente a quantidade de page faults(faltas de páginas durante os acessos à memoria).</p>
     <br />
     <div align='start'>
          <ul>
               <legend><h2>Política de substituíção de página dos algoritmos</h2></legend>
               <li>Fifo</li>
               <p>  O algoritmo fifo seguirá o princípio básico de uma fila o primeiro que entra será o primeiro a sair quando a fila se encontrar cheia e chegar um endereço não registrado.</p>
               <li>Segunda chance</li>
               <p>  O algoritmo de segunda chance também conhecido como fifo + bit R, funcionará como o fifo seguindo a ordem da fila porém checará se o bit de referência está como 1, caso esteja, então ele altera esse valor para 0 e então checa o próximo da fila até encontrar o primeiro com o bit R igual a 0.</p>
               <li>NRU</li>
               <p>  O algoritmo NRU vai levar em consideração o bit R e o bit M, com isso, ele checa se a página foi referênciada com o bit R e modificada com o bit M(se houve um instrução de escrita do valor da memória). Então com isso ele divide as páginas mapeadas em 4 classes onde o bit R e bit M respectivamente irão ficar ['00'->classe 1, '01'->classe 2, '10'->classe 3, '11'->classe 4] sendo assim ele remove a classe mais baixa, caso todas as posições de memória estejam com a mesma classe então ele aleotóriamente escolherá um valor para ser removido.</p>
               <li>AGING</li>
               <p>  O Algoritmo aging é o algoritmo LRU modificado e significa envelhecimente por um motivo simples, o LRU incremente um contador no endereço e isso pode gerar um problema caso o endereço seja muito referenciado ele pode acabar não saindo mais, enquanto o aging decrementa o contador de cada um dos valores por meio de bits que ele fará um deslocamento a esquerda de todos os valores com 1 no valor referênciado e 0 nos demais e removerá aquele que tiver o menor contador.</p>
               <li>MFU</li>
               <p>  O Algoritmo MFU também é uma modificação do LRU onde o contador será incrementado e assim como o LRU não será decrementado, entretanto quando solicitado ele removerá aquele endereço que tiver o maior contaodor.</p>
               <li>Random</li>
               <p>  O Algoritmo random é bem simples ele pega uma posiçao mapeada aleatóriamente na memória e então a removerá.</p>
          </ul>
     </div>
     <br/>  
     <h1 style="color: #f00;">Bugs conhecidos e observações importantes</h1>  
     <h4>Uma estrutura de dados para armazenar os bit e move-los durante a execução não foi criado como é proposta do aplicativo ele apenas possui um contador nos seus bits de acesso que sera incrementado quando a posição entra ou e requisitada ocasionando um page hit(acerto de página) e decrementado quando não referenciado. O algoritmo MFU incrementa um contador no mesmo bit(PT_AGING_COUNTER).</h4>
     <hr>
     <p>Sistemas Operacionais</p>
</div>