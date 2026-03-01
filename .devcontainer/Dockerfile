FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

# Install MySQL Client and Development Libraries
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libmysqlcppconn-dev \
    default-mysql-client \
    build-essential \
    cmake

# Clean up
RUN apt-get clean && rm -rf /var/lib/apt/lists/*