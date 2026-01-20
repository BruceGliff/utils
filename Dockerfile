# Use Ubuntu 24.04 as base image
FROM ubuntu:24.04

# Install required packages
RUN apt-get update && \
    apt-get install -y \
      gcc-14 \
      g++-14 \
      clang-tidy \
      cppcheck \
      cmake \
      python3 \
      pipx \
      vim \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

USER ubuntu

# Install Conan
RUN pipx install conan
ENV PATH="$PATH:/home/ubuntu/.local/bin"
RUN conan profile detect

