## Trabalho Prático Individual 01: Simulação Uber em C

**Resumo:**

Este projeto envolve a criação de uma aplicação cliente-servidor em C que simula o funcionamento básico do Uber, utilizando sockets TCP/IP para comunicação. O programa permite que um "cliente" (usuário) solicite uma corrida e um "servidor" (motorista) aceite ou recuse a solicitação. Se aceito, o servidor envia atualizações de distância ao cliente até que o "motorista" chegue ao destino.

**Requisitos:**

* **Linguagem:** C
* **Tipo de conexão:** TCP
* **Plataforma:** Linux
* **IPv4 e IPv6:** O programa deve funcionar com ambos os protocolos.

**Funcionalidades:**

* **Servidor:**
    * Aguarda conexão do cliente.
    * Aceita ou recusa a solicitação de corrida.
    * Se aceito, calcula a distância até o cliente e envia atualizações de distância a cada 2 segundos, simulando a aproximação do motorista.
    * Envia mensagem de chegada ao cliente quando a distância chega a zero.
* **Cliente:**
    * Permite ao usuário solicitar uma corrida.
    * Envia suas coordenadas ao servidor.
    * Recebe e exibe atualizações de distância do servidor.
    * Exibe mensagem de chegada do motorista.

**Entrega:**

* Código fonte em C (arquivos separados para cliente e servidor)
* Makefile para compilação
* Documentação em PDF (pelo menos 3 páginas) contendo:
    * Nome completo e matrícula do aluno
    * Explicação detalhada do código (com foco nas partes de conexão)
    * Prints do programa em funcionamento (com descrições)

**Instruções de compilação:**

Execute o comando `make` na raiz do projeto. Isso irá gerar os binários `client` e `server` dentro da pasta `bin`.

**Execução:**

* **Servidor:** `./bin/server [ipv4|ipv6] [porta]`
* **Cliente:** `./bin/client [ipv4|ipv6] [ip_servidor] [porta]`

**Observações:**

* Utilize a fórmula de Haversine para calcular a distância entre coordenadas.
* Comente todas as linhas de código relacionadas à conexão.
* Utilize a metodologia de divisão e conquista para simplificar o desenvolvimento.

**Pontuação:**