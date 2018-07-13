# Base image
FROM ubuntu:14.04

# Install boost library
RUN apt-get update && apt-get install -y libboost-all-dev

# Install cppnet library
RUN apt-get update && apt-get install -y libcppnetlib-dev

# Install mysqlcpp library
RUN apt-get update && apt-get install -y libmysqlcppconn-dev

# Create trading system directory
RUN mkdir /trading_system

# Copy main binary
COPY trading_system /trading_system/

# Copy system config
COPY system_config_test /trading_system/

# Copy run script
COPY run.sh /trading_system/

RUN mkdir -p /trading_system/log/yin_he_ctp_future
RUN mkdir -p /trading_system/data
RUN mkdir -p /trading_system/signal
COPY html /trading_system/html

# Create ctp directory
RUN mkdir /usr/include/ctp

# Copy ctp headers
COPY ctp_api/include/error.dtd /usr/include/ctp/
COPY ctp_api/include/error.xml /usr/include/ctp/
COPY ctp_api/include/ThostFtdcMdApi.h /usr/include/ctp/
COPY ctp_api/include/ThostFtdcTraderApi.h /usr/include/ctp/
COPY ctp_api/include/ThostFtdcUserApiDataType.h /usr/include/ctp/
COPY ctp_api/include/ThostFtdcUserApiStruct.h /usr/include/ctp/

# Copy ctp libraries
COPY ctp_api/lib/libthostmduserapi.so /usr/lib/
COPY ctp_api/lib/libthosttraderapi.so /usr/lib/

# Go to work directory
WORKDIR "/trading_system"

# Verify set up
RUN pwd
RUN ls /trading_system

# Run trading system
CMD ["/trading_system/run.sh"]

EXPOSE 10000
