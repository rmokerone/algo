编译
===

# 1. libevnet

```
tar -xvf libevent*.tar.gz
cd libevent*
./configure --prefix=${PWD}/out && make && make install 

生成的文件在libevnet*/out中
```