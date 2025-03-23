
#!/bin/bash


file_path="DY_2018.txt"
echo "Reading file: $file_path using while loop"
echo "---------------------------------"

## Using while loop to read the file line by line
line_number=1
while IFS= read -r line || [[ -n "$line" ]]; do
    echo "Line $line_number: $line"
    line_number=$((line_number + 1))
done < "$file_path"

echo "---------------------------------"
echo "File reading completed!"
