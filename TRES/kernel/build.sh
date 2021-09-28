make compile
if [ $? -eq 0 ]
then
  echo "Successfully compile"
else
  echo "\033[1;31m Could not compile \033[1;31m" >&2
  exit
fi
cp ./build/KERNEL.BIN ../tools/

