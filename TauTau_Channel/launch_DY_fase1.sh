voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o test test.cpp `root-config --cflags --glibs`

# for i in {1..5}
# do
#   	echo "$i" | ./nanotry_fase1_DY >> cortes_cinematicos_fase1_DY.txt
#         clear
# done

file_path="DY_2018.txt"
echo "Reading files from: $file_path"

while IFS= read -r file_name; do
    echo "Processing file: $file_name"
    echo "$file_name" | ./test #>> cortes_cinematicos_fase1_DY.txt
done < "$file_path"

echo "Processing completed!"