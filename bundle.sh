#!/bin/bash
##########################################################################                                                                        
# If not stated otherwise in this file or this component's Licenses.txt                                                                           
# file the following copyright and licenses apply:                                                                                                
#                                                                                                                                                 
# Copyright 2015 RDK Management                                                                                                                   
#                                                                                                                                                 
# Licensed under the Apache License, Version 2.0 (the "License");                                                                                 
# you may not use this file except in compliance with the License.                                                                                
# You may obtain a copy of the License at                                                                                                         
#                                                                                                                                                 
# http://www.apache.org/licenses/LICENSE-2.0                                                                                                      
#                                                                                                                                                 
# Unless required by applicable law or agreed to in writing, software                                                                             
# distributed under the License is distributed on an "AS IS" BASIS,                                                                               
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.                                                                        
# See the License for the specific language governing permissions and                                                                             
# limitations under the License.                                                                                                                  
########################################################################## 
echo "script to create a bundle if not exist"

# Check if the number of arguments provided is valid
if [ "$#" -ne 1 ]; then
	echo "Usage: $0 <ociurl>"
	exit 1
fi

# Get the ociurl value from the first command line argument
ociurl="$1"

# Define paths
source_dir="/home/rdkm/ociimages"
target_dir="/home/rdkm/bundle/BundleGen"

# Extract the file name from ociurl
filename=$(basename "$ociurl")

# Check if the file exists in the source directory
if [ -f "$source_dir/$filename" ]; then
	# Extract the bundle name
	bundle_name=$(basename "$(dirname "$(dirname "$ociurl")")")

	# Define the target bundle directory
	target_bundle_dir="$target_dir/$bundle_name"

	# Check if the target bundle directory exists, if not, create it
	if [ ! -d "$target_bundle_dir" ]; then
    	mkdir -p "$target_bundle_dir"
	fi
	# Extract the bundle name from the filename
	b_name="${filename%.*}"
	echo $b_name
	# Create a folder named bundle2 inside the target bundle directory
	bundle2_dir="$target_bundle_dir/$b_name"
	if [ ! -d "$bundle2_dir" ]; then
    	mkdir "$bundle2_dir"
	fi

	# Copy the file from source directory to bundle2 directory
	cp "$source_dir/$filename" "$bundle2_dir/"

	# Change to the bundle2 directory
	cd "$bundle2_dir"

	# Extract the file
	tar -xvf "$filename"
    
	cd ..
	echo $b_name
	pwd
	ls
	# Run bundlegen command
	echo "generate bundle ...."
	export PYTHONPATH=/usr/lib/python3.8/site:$PYTHONPATH
	bundlegen generate --platform rpi4_reference_dunfell oci:$b_name /home/rdkm/$b_name
   # if [ $? -eq 0 ]; then
	#	echo "Script executed successfully"
   # else
	#	echo "Error: Failed to execute bundlegen command"
	#fi
else
	echo "Error: File $filename not found in $source_dir"
fi

