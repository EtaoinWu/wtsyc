codef=build/judge.S
outf=build/judge.out
exef=build/judge
prefix=./wtsyc/
for sysy in $2/*.sy; do
  basename=${sysy##*/}
  ansf=${sysy%.sy}.out
  inf=${sysy%.sy}.in
  echo ============================ $basename ==============================
  ./build/compiler -S $sysy -o $codef
  sudo docker exec -i $1 riscv32-unknown-linux-gnu-gcc $prefix$codef -o $prefix$exef -L/root -lsysy -static
  if [ -f $inf ]; then
    sudo docker exec -i $1 qemu-riscv32-static $prefix$exef > $outf < $inf
    ret=$?
    [[ $(tail -c1 $outf) && -f $outf ]]&&echo ''>>$outf
    echo $ret >> $outf
    [[ $(tail -c1 $outf) && -f $outf ]]&&echo ''>>$outf
  else
    sudo docker exec -i $1 qemu-riscv32-static $prefix$exef > $outf
    ret=$?
    [[ $(tail -c1 $outf) && -f $outf ]]&&echo ''>>$outf
    echo $ret >> $outf
    [[ $(tail -c1 $outf) && -f $outf ]]&&echo ''>>$outf
  fi
  [[ $(tail -c1 $ansf) && -f $ansf ]]&&echo ''>>$ansf
  diff --strip-trailing-cr $outf $ansf
  if [ $? -ne 0 ]; then
    echo "Different!!!"
    echo ============================ out ==============================
    cat $outf
    echo ============================ ans ==============================
    cat $ansf
    echo ============================ '***' ==============================
    echo $sysy
    break
  fi
done