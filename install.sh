#!/bin/bash

# instruction
echo "########################################"
echo "Run install script as"
echo "  sudo ./install [database_password]"
echo "If you don't have mysql, it will install mysql"
echo "for you with user [root] and password [database_passwor]"
echo "If you don't have docker, it will install docker for you"
echo "########################################"

# check command correctness
if [ $# -ne 1 ]; then
	echo "sudo ./install database_password"
	exit 1
fi

# install mysql and create database
echo "mysql-server mysql-server/root_password password $1" | debconf-set-selections
echo "mysql-server mysql-server/root_password_again password $1" | debconf-set-selections
apt-get install -y mysql-server
mysql -u root -p$1 < production.sql

# install docker
apt-get install -y docker.io
systemctl restart docker
docker -v
docker info
groupadd docker
usermod -aG docker $(whoami)


# load trading system
docker load trading_system
docker run --net=host trading_system


