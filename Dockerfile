FROM gcc:12.2

# Install c++ 23
RUN apt update -y && apt upgrade -y
RUN apt-get install -y build-essential

# Install cmake
RUN apt install python3-pip -y 
RUN pip3 install cmake

# Install HDF5
RUN apt install -y libhdf5-dev
