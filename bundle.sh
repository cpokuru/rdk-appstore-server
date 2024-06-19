#!/bin/bash
echo "script to create a bundle if not exist"
echo $1
echo $2
# Check if the number of arguments provided is valid
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <ociurl>"
    exit 1
fi

# Get the ociurl value from the first command line argument
ociurl="$1"
platformname="$2"
# Define paths
source_dir="/home/rdkm/ociimages"
target_dir="/home/rdkm/BundleGen"

# Extract the file name from ociurl
filename=$(basename "$ociurl")
echo "filenam"
echo $filename
echo "source"
echo $source_dir
filename_ext="${filename}.tar"
echo $filename_ext

# Check if the file exists in the source directory
if [ -f "$source_dir/$filename_ext" ]; then
    # Extract the bundle name
    bundle_name=$(basename "$(dirname "$(dirname "$ociurl")")")

    # Define the target bundle directory
    target_bundle_dir="$target_dir/$bundle_name"

    # Check if the target bundle directory exists, if not, create it
    if [ ! -d "$target_bundle_dir" ]; then
        mkdir -p "$target_bundle_dir"
    fi
    # Extract the bundle name from the filename
    b_name="${filename_ext%.*}"
    echo $b_name
    # Create a folder named bundle2 inside the target bundle directory
    bundle2_dir="$target_bundle_dir/$b_name"
    if [ ! -d "$bundle2_dir" ]; then
        mkdir "$bundle2_dir"
    fi

    # Copy the file from source directory to bundle2 directory
    cp "$source_dir/$filename_ext" "$bundle2_dir/"

    # Change to the bundle2 directory
    cd "$bundle2_dir"

    # Extract the file
    tar -xvf "$filename_ext"
    
    cd ..
    echo $b_name
    pwd
    ls 
    # Run bundlegen command
    echo "generate bundle ...."
    export PYTHONPATH=/usr/lib/python3.8/site:$PYTHONPATH
    #bundlegen generate --platform rpi4_reference_dunfell oci:$b_name /home/rdkm/$b_name
    bundlegen generate --platform $platformname oci:$b_name /home/rdkm/$b_name
   # if [ $? -eq 0 ]; then
    #    echo "Script executed successfully"
   # else
    #    echo "Error: Failed to execute bundlegen command"
    #fi
else
    echo "Error: File $filename not found in $source_dir"
fi

