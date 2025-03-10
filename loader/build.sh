gcc -fPIC -DDEBUG -shared -o rootkit.so rootkit.c -ldl
gcc main.c -o main 

sleep 2
LD_PRELOAD=rootkit.so ./main