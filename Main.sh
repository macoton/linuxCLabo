gcc -o program program.c -lpthread
./program 0 &
sleep 2
kill -SIGINT $!
