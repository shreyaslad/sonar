FROM amd64/ubuntu:latest

RUN apt-get update
RUN apt-get install -y \
    clang \
    lld \
    nasm \
    gcc \
    g++ \
    make \
    git \
    xorriso

ADD . /sonar
WORKDIR /sonar

ENV DOCKER_BUILD=1

CMD ["make", "sonar"]
