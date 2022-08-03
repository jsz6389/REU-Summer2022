for file in ./*
do
	filename=`echo $file | awk '{split($0,a,"."); print a[2]}'`
	echo $filename
	#insert external function declaration
	sed -i.bak '5i\
		declare i64 @printf(...) nounwind\
		' ./$filename.ll
	#substitute the original function call
	sed -i 's/%14 = call i64 (...) @__printf_chk(i64 %4, i64 %5, i64 %6, i64 %7, i64 %8, i64 %9, i64 %11, i64 %13)/%14 = call i64 (...) @printf(i64 %5, i64 %6, i64 %7, i64 %8, i64 %9, i64 %11, i64 %13)/' ./$filename.ll
	remill-clang-11 -o ./$filename.lifted ./$filename.ll -lmcsema_rt64-11.0 -lm -v \
		`ldd ../$filename | awk '!/^\t\//' | awk '!/^\tlinux-vdso/' | awk '{split($1,q,/\.so/); print(q[1]);}' | sed 's/lib/-l/'`
	if [ $? != 0 ]; then
		echo "$filename,0" >> output.csv
	else
		echo "$filename,1" >> output.csv
	fi
done

#cleanup
mkdir backup
rm *.sh.bak
mv *.bak ./backup/

mkdir lifted
mv *.lifted ./lifted/
