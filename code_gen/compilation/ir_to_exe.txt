clang -fPIC -o hydrogen.o -c hydrogen.ll
clang -o hydrogen hydrogen.o
llc -filetype=obj -relocation-model=pic hydrogen.ll -o hydrogen.o
gcc -o hydrogen hydrogen.o
./hydrogen