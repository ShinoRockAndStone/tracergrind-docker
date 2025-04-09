FROM debian:bullseye

# Update package manager
RUN apt-get update
RUN apt-get upgrade -y

# Has stuff like gcc, make, and libc6-dev
RUN apt-get install -y build-essential automake libcapstone-dev libsqlite3-dev wget unzip

# Add support for i386 on an amd64 platform
RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get install --yes --no-install-recommends gcc-multilib

# Get Tracer
RUN wget 'https://github.com/SideChannelMarvels/Tracer/archive/refs/heads/master.zip'
RUN unzip master.zip

# Get Valgrind 3.15.0
# RUN wget 'https://sourceware.org/ftp/valgrind/valgrind-3.15.0.tar.bz2'
COPY valgrind-3.15.0.tar.bz2 .
RUN tar xf valgrind-3.15.0.tar.bz2

# Apply the patch
RUN cp -r Tracer-master/TracerGrind/tracergrind valgrind-3.15.0/
RUN cp -r Tracer-master/TracerGrind/valgrind-3.15.0.diff .
RUN patch -p0 < valgrind-3.15.0.diff

# Build and install Valgrind
WORKDIR valgrind-3.15.0/
RUN ./autogen.sh
RUN ./configure --prefix=/usr
RUN make -j8
RUN make install

# Run it as if it were valgrind
ENTRYPOINT ["valgrind"]
CMD ["--help"]
