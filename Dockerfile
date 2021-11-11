#printf '#include <unistd.h>\nvoid main(){write(1, "Hello World !\\n", 14);}' | gcc -x c - -o sample
FROM debian:10

RUN apt-get update && apt-get install -y build-essential nasm openssh-server rsync cmake lldb git file make gcc g++ gdb
RUN git clone https://github.com/radareorg/radare2 /tmp/radare2
RUN /tmp/radare2/sys/install.sh
RUN yes password | adduser user
RUN ssh-keygen -A
RUN service ssh start
CMD /usr/sbin/sshd -D

EXPOSE 22