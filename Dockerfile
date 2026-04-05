# Stage 1: Build
FROM ubuntu:22.04 AS builder

# Install build tools and dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    git \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source code
COPY . .

# Run oatpp modules installer script before building
RUN chmod +x utility/install-oatpp-modules.sh && \
    bash ./utility/install-oatpp-modules.sh Release

# Install jwt-cpp from source
RUN git clone https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp && \
    cd /tmp/jwt-cpp && \
    mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    make install

# Configure and build in Release mode
RUN rm -rf build && mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

# Stage 2: Runtime
FROM ubuntu:22.04

# Install only runtime dependencies (adjust as needed)
RUN apt-get update && apt-get install -y \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy built executable from builder stage
COPY --from=builder /app/build/crud-exe .

# Copy the swagger icon because it is vital to the project
COPY --from=builder /usr/local/include/oatpp-1.4.0/bin/oatpp-swagger/res /usr/local/include/oatpp-1.4.0/bin/oatpp-swagger/res
RUN mkdir /usr/local/include/oatpp-1.4.0/oatpp-swagger

# Expose application port (adjust if your app uses a different one)
EXPOSE 8000

CMD ["/app/crud-exe"]
