## Building das imagens

Baixe e instale [Docker](https://www.docker.com/)

Comandos para building das imagens:

```bash
docker build -f tracergrind/Dockerfile -t tracergrind .
```
```bash
docker build -f texttrace/Dockerfile -t texttrace .
```
## Uso

Como usar:
```bash
make SRC=nome-arquivo.c
```