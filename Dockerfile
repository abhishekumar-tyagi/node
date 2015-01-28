FROM ubuntu:14.04.1
MAINTAINER Sam Liu <ontouchstart@gmail.com>

RUN apt-get update
RUN apt-get -y upgrade

RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN locale

RUN apt-get install -y build-essential python-software-properties man
ADD . /io.js
RUN cd /io.js && ./configure && make && make test && make doc && make install
RUN iojs -v
ENV HOME /home
WORKDIR /home
