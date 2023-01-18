FROM gcc:12.2

# Install
RUN apt update -y && apt upgrade -y && apt install cmake -y

# Setup
VOLUME /swann/build

# Compile
WORKDIR /swann/src

COPY ./src /swann/src

RUN cd /swann/src && cmake -S . -B build && cmake --build build

CMD  cp /swann/bin/swann /swann/build && /swann/build/swann


# WORKDIR /swann/src
# CMD /swann/build/swann