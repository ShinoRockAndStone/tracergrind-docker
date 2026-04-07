FROM debian:bullseye

ENV DEBIAN_FRONTEND=noninteractive

# Add suporte i386 + install
RUN dpkg --add-architecture i386 && apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
    build-essential \
    automake \
    libcapstone-dev \
    libsqlite3-dev \
    wget \
    unzip \
    gcc-multilib && \
    rm -rf /var/lib/apt/lists/*

# Copiar arquivos (devem estar no diretório do build)
COPY Tracer-master.zip .

# Extrair
RUN unzip Tracer-master.zip

# Build
WORKDIR /Tracer-master/TracerGrind/texttrace/
RUN make && make install

WORKDIR /home

ENTRYPOINT ["texttrace"]
CMD ["--help"]
