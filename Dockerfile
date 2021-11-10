#printf '#include <unistd.h>\nvoid main(){write(1, "Hello World !\\n", 14);}' | gcc -x c - -o sample
FROM alpine:3.12

RUN apk add --update --no-cache openssh gcc g++ rsync cmake make gdb coreutils nasm lldb linux-headers git patch
RUN git clone https://github.com/radareorg/radare2 /tmp/radare2
RUN /tmp/radare2/sys/install.sh
RUN yes password | adduser user
RUN ssh-keygen -A

EXPOSE 22
CMD /usr/sbin/sshd -D