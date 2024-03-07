FROM rust:1.75.0-slim-bookworm

# Define a build argument to specify the target architecture (defaulting to "x86_64-unknown-linux-gnu")
ARG ARCH_NAME=x86_64
ARG TARGET_ARCH=x86_64-unknown-linux-gnu

# Fetch package list
RUN apt update

# Install cross-compilation tools
RUN apt install -y gcc gcc-aarch64-linux-gnu

# Install packaging tools
RUN apt install -y build-essential ruby ruby-dev rubygems rpm zstd libarchive-tools

# Install fpm
RUN gem install fpm

# Install rustup components
RUN rustup component add clippy
RUN rustup component add rustfmt

##### Commands #####

WORKDIR /workspace

# Build and package the project
CMD rustup target add $TARGET_ARCH && \
    cargo build --release --target $TARGET_ARCH && \
    cp target/$TARGET_ARCH/release/vangelis . && \
    fpm -f -t deb -p vangelis-debian-$ARCH_NAME.deb -a $ARCH_NAME && \
    fpm -f -t rpm -p vangelis-redhat-$ARCH_NAME.rpm -a $ARCH_NAME && \
    fpm -f -t pacman -p vangelis-archlinux-$ARCH_NAME.pkg.tar.zst -a $ARCH_NAME