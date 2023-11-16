gcc -o program program.c -lpthread
./program 2 1 1 &
sleep 1
kill -SIGINT $!
