FROM gcc:12.2

# Install
RUN apt update -y && apt upgrade -y && apt install cmake -y