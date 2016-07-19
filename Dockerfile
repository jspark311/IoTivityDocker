FROM ubuntu
RUN apt-get -y update
RUN apt-get install -y git-core scons ssh build-essential g++ doxygen valgrind libboost-dev libboost-program-options-dev libboost-thread-dev uuid-dev libssl-dev libtool libglib2.0-dev unzip curl 
COPY Buildscript /root/Buildscript
COPY facepalm.patch /root/facepalm.patch


COPY Thermostat.cpp /root/Thermostat.cpp
COPY thermostat_db.json /root/thermostat_db.json
COPY Makefile /root/Makefile
COPY runThermostat.sh /root/runThermostat.sh

RUN chmod +x /root/Buildscript
RUN chmod +x /root/runThermostat.sh

WORKDIR /root

RUN /root/Buildscript
