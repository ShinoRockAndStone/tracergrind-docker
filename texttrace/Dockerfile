FROM debian:stable

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

# Build and install texttrace
WORKDIR Tracer-master/TracerGrind/texttrace/
RUN make
RUN make install

WORKDIR /home
ENTRYPOINT ["texttrace"]
