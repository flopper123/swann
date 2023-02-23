FROM gcc:12.2

# Install
RUN apt update -y && apt upgrade -y
RUN apt-get install build-essential

RUN wget https://cmake.org/files/v3.25/cmake-3.25.0.tar.gz
RUN tar xf cmake-3.25.0.tar.gz
RUN cd cmake-3.25.0 && ./configure && make && make install
