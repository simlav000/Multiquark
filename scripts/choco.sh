#!/bin/bash

# Local directory where histograms are stored
local_directory="/home/simlav000/McGill/Multiquark/Histograms/Ready/"

# Remote SSH details
remote_username="slavoie"
remote_host="choco.physics.mcgill.ca"
remote_path="/homes/anquetil/slavoie/plots/"

# Function to check if a file exists remotely
remote_file_exists() {
    sshpass -p "$remote_password" ssh "$remote_username@$remote_host" "[ -f '$1' ]"
}

# Iterate over each file in the local directory
for file_path in "$local_directory"/*; do
    if [ -f "$file_path" ]; then
        filename=$(basename "$file_path")
        remote_file="$remote_path/$filename"
        
        # Check if the file already exists on the remote server
        if remote_file_exists "$remote_file"; then
            echo "$filename already exists on $remote_host"
        else
            # File does not exist on remote, so upload it
            echo "Uploading $filename to $remote_host:$remote_file"
            sshpass -p "$remote_password" scp "$file_path" "$remote_username@$remote_host:$remote_file"
        fi
    fi
done
