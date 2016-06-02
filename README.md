# IoTivityDocker
IoTivity build environment based on Ubuntu.


How to use this Dockerfile.

    git clone https://github.com/jspark311/IoTivityDocker.git
    docker build -t='my_name' IoTivityDocker/
    docker run -v <your root source path>:/src_root my_name
