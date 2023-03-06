FROM gcc:12.2

# Install c++ 23
RUN apt update -y && apt upgrade -y
RUN apt-get install -y build-essential

# Install cmake
RUN apt install python3-pip -y 
RUN pip3 install cmake

# Install Google Benchmark
RUN mkdir /google \
    && cd /google \
    && git clone https://github.com/google/benchmark.git \
    && cd benchmark \
    && git checkout v1.7.1 \
    && cmake -E make_directory "build" \
    && cmake -E chdir "build" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release ../ \
    && cmake --build "build" --config Release --target install

# Install HDF5
RUN apt install -y libhdf5-dev
RUN pip3 install -U cython numpy h5py scikit-learn
