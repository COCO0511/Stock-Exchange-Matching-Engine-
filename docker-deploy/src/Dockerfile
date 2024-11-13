FROM ubuntu:20.04

RUN mkdir /src
ADD . /src/
WORKDIR /src
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update && \
    apt-get install -y libpqxx-dev postgresql libxerces-c-dev make build-essential && \
    rm -rf /var/lib/apt/lists/*