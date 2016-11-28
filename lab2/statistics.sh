test -z "$1" && echo "Please specify trace.txt.out path" && exit 1

for i in {0..4}; do
  for j in {0..4}; do
    echo "$i $j `cat $1 | grep "$i $j" | wc -l`"
  done
done
