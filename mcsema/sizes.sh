#!/bin/bash

main () {
	echo filename,original size,lifted size,cfg,lift,recompile >> output.csv
	for file in *
	do
		if [ "`file $file | awk '{print $2}'`" == "ELF" ] && ! grep -q $file output.csv; then
			size $file
		fi
	done

	mkdir log ll lifted
	rm *.bc
	rm *.cfg

	mv *.lifted ./lifted/
	mv *.ll ./ll/
	mv *.log ./log/

	segfault
}


size () {
	mcsema-disass-3.8 \
	        --disassembler "${IDA_PATH}/idat64" \
	        --arch amd64 \
	        --os linux \
	        --entrypoint main \
	        --binary ./$1 \
	        --output ./$1.cfg \
	        --log_file ./$1.log

	if [ $? != 0 ]; then
	        echo $1,`du -b $1 | awk '{print $1}'`,Failed,0,0,0 >> output.csv
	        return
	fi

	mcsema-lift-${version}.0 \
	        --arch amd64 \
	        --os linux \
	        --cfg ./$1.cfg \
	        --output ./$1.bc \
	        --merge_segments \
	        --explicit_args \
	        --check_for_lowmem_xrefs \
	        --name_lifted_sections
	
	if [ $? != 0 ]; then
	        echo $1,`du -b $1 | awk '{print $1}'`,Failed,1,0,0 >> output.csv
	        return
	fi

	llvm-dis-${version} $1.bc
	if [ $apt == "true" ]; then
		for i in `ldd $1 | awk '!/^\t\//' | awk '!/^\tlinux-vdso/' | awk '{split($1,q,/\.so/); print(q[1]);}' | sed 's/$/-dev/'`; do yes | sudo apt-get install $i ; done
	fi
	remill-clang-${version} -o $1.lifted ./$1.ll -lmcsema_rt64-${version}.0 -lm -v \
        	`ldd $1 | awk '!/^\t\//' | awk '!/^\tlinux-vdso/' | awk '{split($1,q,/\.so/); print(q[1]);}' | sed 's/lib/-l/'`

	if [ $? != 0 ]; then
	        echo $1,`du -b $1 | awk '{print $1}'`,Failed,1,1,0 >> output.csv
        	return
	fi

	#output
	echo $1,`du -b $1 | awk '{print $1}'`,`du -b $1.lifted | awk '{print $1}'`,1,1,1 >> output.csv

}


segfault () {
	pushd lifted
	for file in *.lifted
	do
		./$file --help
		echo $file,$? >> segfault.csv
	done
	popd
}


apt="false"
version="9"
for arg in $@ 
do
	if [ ${arg:0:1} == "-" ]; then
		if [[ "$arg" =~ "a" ]]; then
			apt="true"
		elif [[ "$arg" =~ "v" ]]; then
			version=""
		fi
	fi
done

main
