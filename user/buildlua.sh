pushd lua-5.1.4/src

rm *.o

for src in lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c; do
	gcc -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-stack-protector -I../../uinc -c $src -o ${src/.c/.o}
done

ar q ../../csoslua.a *.o

popd