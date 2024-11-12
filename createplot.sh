#!/usr/bin/env bash

# Function to display usage information
function usage() {
    echo "Usage: $0 -s <size> -f <filename>"
    echo "Options:"
    echo "  -s <size>      The size of the array to sort."
    echo "  -f <filename>  The plot file name (without extension)."
    echo "  -h             Display help."
    exit 1
}

# Check if no arguments were provided
if [ $# -eq 0 ]; then
    usage
fi

# Parse command-line options
while getopts "hs:f:" arg; do
    case $arg in
        s) # The size of the array to sort
            size=${OPTARG}
            ;;
        f) # The plot file name
            name=${OPTARG}
            ;;
        h | *) # Display help
            usage
            ;;
    esac
done

# Ensure required parameters are provided
if [[ -z "$size" || -z "$name" ]]; then
    echo "Error: Both -s <size> and -f <filename> options are required."
    usage
fi

# Check if the compiled program exists
if [[ ! -x "./myprogram" ]]; then
    echo "Error: myprogram is not present in the build directory. Did you compile your code?"
    exit 1
fi

# Remove old data file if it exists
data_file="data.dat"
if [[ -f $data_file ]]; then
    rm -f "$data_file"
fi

# Generate data by running the program with different thread counts
echo "Running myprogram to generate data"
echo "#Threads Time(ms)" > "$data_file"
for n in {1..32}; do
    echo -ne "Running with $n thread(s) \r"
    # Capture both the thread count and time from myprogram output
    result=$(./myprogram "$size" "$n")
    echo "$result" >> "$data_file"
done
echo -e "\nData generation complete."

# Generate the plot using gnuplot
plot_file="${name}.png"
gnuplot -e "filename='$plot_file'" graph.plt

if [[ -f "$plot_file" ]]; then
    echo "Created plot $plot_file from $data_file"
else
    echo "Error: Plot generation failed. Check if gnuplot and graph.plt are correctly configured."
fi