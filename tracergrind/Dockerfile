FROM debian:bullseye

# Evita prompts interativos
ENV DEBIAN_FRONTEND=noninteractive

# Update + install
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    build-essential \
    automake \
    libcapstone-dev \
    libsqlite3-dev \
    wget \
    unzip \
    gcc-multilib && \
    rm -rf /var/lib/apt/lists/*

# Add suporte i386
RUN dpkg --add-architecture i386 && apt-get update

# Copiar arquivos locais (DEVEM estar na mesma pasta do build)
COPY Tracer-master.zip .
COPY valgrind-3.15.0.tar.bz2 .

# Extrair arquivos
RUN unzip Tracer-master.zip
RUN tar xf valgrind-3.15.0.tar.bz2

# Aplicar patch
RUN cp -r Tracer-master/TracerGrind/tracergrind valgrind-3.15.0/
RUN cp Tracer-master/TracerGrind/valgrind-3.15.0.diff .
RUN patch -p0 < valgrind-3.15.0.diff

# Build
WORKDIR /valgrind-3.15.0/
RUN ./autogen.sh && \
    ./configure --prefix=/usr && \
    make && \
    make install

ENTRYPOINT ["valgrind"]
CMD ["--help"]
