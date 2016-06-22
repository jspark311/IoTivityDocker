# IoTivityDocker
Secure IoTivity build environment based on Ubuntu.
Sample thermostat device included.

How to use this Dockerfile.

    git clone https://github.com/datasnap-io/IoTivityDocker.git
    docker build -t='my_name' IoTivityDocker/
    docker run -iv <your root source path>:/src_root my_name

Or if you are only interested in invoking Thermostat...

    docker run -i my_name /root/runThermostat.sh

The last command should drop you into a shell where you can execute your build.
