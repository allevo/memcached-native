FROM node:0.10.40

RUN apt-get update
RUN apt-get install -y --force-yes libmemcached-dev memcached

RUN mkdir /repos
WORKDIR /repos
ADD . /repos

RUN useradd -ms /bin/bash memcached
RUN chown -R memcached /repos
USER memcached

RUN ls

RUN npm install


