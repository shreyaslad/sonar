FROM amd64/ubuntu:latest

RUN apt update -y && apt install -y \
    clang \
    nasm \
    gcc \
    g++ \
    make \
    lld \
    git

COPY . /sonar
WORKDIR /sonar

CMD ["make"]
