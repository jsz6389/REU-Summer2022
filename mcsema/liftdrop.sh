mcsema-disass-3.8 \
	--disassembler "${IDA_PATH}/idat64" \
	--arch amd64 \
	--os linux \
	--entrypoint main \
	--binary ./$1 \
	--output ./$1.cfg \
	--log_file ./$1.log

if [ $? != 0 ]; then
	exit 1
fi

mcsema-lift-11.0 \
	--arch amd64 \
	--os linux \
	--cfg ./$1.cfg \
	--output ./$1.bc \
	--merge_segments \
	--check_for_lowmem_xrefs \
	--libc_constructor init \
	--libc_destructor fini \
	--abi_libraries /home/jakobz/repos/mcsema/build/mcsema/OS/Linux/X86/ABI_exceptions_amd64.bc,/home/jakobz/repos/mcsema/build/mcsema/OS/Linux/X86/ABI_libc_amd64_ABI_libc.c.bc \
	--name_lifted_sections

if [ $? != 0 ]; then
	exit 1
fi

llvm-dis-11 $1.bc
remill-clang-11 -o $1.lifted ./$1.ll -lmcsema_rt64-11.0 -lm -v \
	`ldd $1 | awk '{split($1,a,".so"); print(a[1]);}' | sed 's/lib/-l/' | awk '(NR>1)' | sed '$d'`

if [ $? != 0 ]; then
	exit 1
fi
