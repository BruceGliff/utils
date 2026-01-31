# Use Ubuntu 24.04 as base image
FROM ubuntu:24.04

# Install required packages
RUN apt-get update && \
    apt-get install -y \
      build-essential \
      clang-tidy \
      clang-format \
      cppcheck \
      cmake \
      python3 \
      pipx \
      vim \
      lcov \
    && rm -rf /var/lib/apt/lists/*

USER ubuntu

# Install Conan
RUN pipx install conan
ENV PATH="$PATH:/home/ubuntu/.local/bin"
RUN conan profile detect
RUN pipx install codespell
RUN pipx install gcovr

