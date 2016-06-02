
FROM ubuntu
RUN apt-get -y update
RUN apt-get install -y git-core scons ssh build-essential g++ doxygen valgrind libboost-dev libboost-program-options-dev libboost-thread-dev uuid-dev libssl-dev libtool libglib2.0-dev unzip
ADD Buildscript /root/Buildscript

RUN chmod +x /root/Buildscript

WORKDIR /root

RUN /root/Buildscript

