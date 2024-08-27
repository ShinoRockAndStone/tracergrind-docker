Building images

Run the following command to build the images:

```bash
docker build -t tracergrind tracergrind
docker build -t texttrace texttrace
```

The basic workflow is:
```bash
docker run -it -v ${PWD}:/home tracergrind -d -d --tool=tracergrind --output=/home/<filename> /home/<filename>.trace
docker run -it -v ${PWD}:/home texttrace home/<filename>.trace home/<filename>.texttrace
```

To create a sample trace with the provided sample file:
```bash
cd sample
gcc -o strcmp strcmp.c
docker run --rm -it -v ${PWD}:/home tracergrind -d -d --tool=tracergrind --output=/home/strcmp.trace /home/strcmp
docker run --rm -it -v ${PWD}:/home texttrace strcmp.trace strcmp.texttrace
```
