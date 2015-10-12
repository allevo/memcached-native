FROM node:0.10.40

RUN apt-get update
RUN apt-get install -y --force-yes libmemcached-dev memcached telnet

WORKDIR /repos



