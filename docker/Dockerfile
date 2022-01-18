FROM ubuntu:20.04

# set timezone
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Taipei
RUN apt-get update && \
        apt-get install -y --no-install-recommends tzdata && \
        rm -rf /var/lib/apt/lists/*

# install gcc 10
RUN apt-get update && \
        apt-get install -y --no-install-recommends build-essential gcc-10 g++-10 && \
        rm -rf /var/lib/apt/lists/* && \
        update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 110 --slave /usr/bin/g++ g++ /usr/bin/g++-10 --slave /usr/bin/gcov gcov /usr/bin/gcov-10

# avoid github certificate verification failed
RUN apt-get update && \
        apt-get install -y --no-install-recommends ca-certificates wget git && \
        rm -rf /var/lib/apt/lists/* && \
        mkdir /usr/local/share/ca-certificates/cacert.org && \
        wget -P /usr/local/share/ca-certificates/cacert.org http://www.cacert.org/certs/root.crt http://www.cacert.org/certs/class3.crt && \
        update-ca-certificates && \
        git config --global http.sslCAinfo /etc/ssl/certs/ca-certificates.crt

# install grpc & protobuf
WORKDIR /tmp
RUN apt-get update && \
        apt-get install -y --no-install-recommends cmake autoconf libtool pkg-config automake unzip curl && \
        rm -rf /var/lib/apt/lists/* && \
        git clone --recurse-submodules -b v1.41.0 https://github.com/grpc/grpc && \
        cd /tmp/grpc && \
        mkdir -p cmake/build && \
        cd cmake/build && \
        cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_SHARED_LIBS=ON ../.. && \
        make -j$(nproc) && \
        make install && \
        rm -rf /tmp/grpc

# install boost
WORKDIR /tmp
RUN git clone --recursive -b boost-1.78.0 https://github.com/boostorg/boost && \
        cd boost && \
        ./bootstrap.sh && \
        ./b2 && \
        ./b2 install && \
        rm -rf /tmp/boost

# install depend
RUN apt-get update && \
        apt-get install -y --no-install-recommends \
                libgoogle-glog-dev \
                libzookeeper-mt-dev \
                libmysql++-dev \
                libcurl4-openssl-dev \
                librdkafka-dev \
                libhiredis-dev \
                wget \
                netbase \
                && \
        rm -rf /var/lib/apt/lists/*

# install mongo-c
WORKDIR /tmp
RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.19.0/mongo-c-driver-1.19.0.tar.gz && \
        tar xzf mongo-c-driver-1.19.0.tar.gz && \
        cd mongo-c-driver-1.19.0 && \
        mkdir cmake-build && \
        cd cmake-build && \
        cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DCMAKE_BUILD_TYPE=Release .. && \
        cmake --build . -j$(nproc) && \
        cmake --build . --target install && \
        rm -rf /tmp/mongo-c-driver-1.19.0 && \
        rm -f /tmp/mongo-c-driver-1.19.0.tar.gz

# install mongo-cxx
WORKDIR /tmp
RUN curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.5/mongo-cxx-driver-r3.6.5.tar.gz && \
        tar -xzf mongo-cxx-driver-r3.6.5.tar.gz && \
        cd mongo-cxx-driver-r3.6.5/build && \
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local && \
        cmake --build . -j$(nproc) && \
        cmake --build . --target install && \
        rm -rf /tmp/mongo-cxx-driver-r3.6.5 && \
        rm -f /tmp/mongo-cxx-driver-r3.6.5.tar.gz

# install tool
RUN apt-get update && \
        apt-get install -y --no-install-recommends \
                jq \
                net-tools \
                vim \
                && \
        rm -rf /var/lib/apt/lists/*

# some setting
RUN echo "PS1='[\[\033[1;31m\]\u\[\033[1;31m\]@\[\033[1;35m\]\A\[\033[1;31m\]@\[\033[1;32m\]\h:\[\033[1;35m\]\w\[\033[0m\]]\[\033[1;31m\]\$\[\033[0m\] '" >> ~/.bashrc && \
        echo "alias cmk='make clean && make'" >> ~/.bashrc && \
        echo "alias ll='ls -alF'" >> ~/.bashrc && \
        echo "alias ls='ls --color=auto'" >> ~/.bashrc

CMD ["/bin/bash"]