FROM	debian:latest

RUN	apt-get	update
RUN	apt-get	install build-essential xorriso nasm -y

WORKDIR	/kfs