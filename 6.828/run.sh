#!/usr/bin/env bash

path="$(pwd)/Lab-mmap"

docker run -it -v ${path}/:/xv6-labs-2021 xv6
#docker run -it -v /Users/lucienxian/Study/course/6.828/xv6-labs-2021/:/xv6-labs-2021 xv6