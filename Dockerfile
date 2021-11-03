FROM alpine:3.12

RUN apk add --update --no-cache openssh gcc g++ rsync cmake make gdb coreutils nasm valgrind lldb
RUN yes password | adduser user
RUN ssh-keygen -A

EXPOSE 22
CMD /usr/sbin/sshd -D