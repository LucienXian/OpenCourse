FROM ubuntu:20.04

ENV TZ=Asia/Shanghai \
    DEBIAN_FRONTEND=noninteractive
# RUN mv source.list /etc/apt/sources.list

RUN apt-get update 
RUN apt-get upgrade
RUN apt-get install -fy --fix-missing git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu

RUN apt-get -y install zsh python3
RUN ln -s /usr/bin/python3 /usr/bin/python
CMD ["zsh"]