import subprocess
import os
from datetime import datetime

# Define the source and destination paths
source_path = "slavoie@lxplus.cern.ch:/afs/cern.ch/user/s/slavoie/private/MultiQuark/run/submitDir/data-ANALYSIS/dataset.root"
home = str(os.getenv("HOME"))
destination_path = home + "/McGill/Multiquark/data/"

# Get the current datetime in DDMMYYYY_HHMM format
current_datetime = datetime.now().strftime("%Y%m%d_%H%M")

# Define the original and new file names
old_file = os.path.join(destination_path, "dataset.root")
new_file = os.path.join(destination_path, f"{current_datetime}dataset.root")

# Rename the old file by prepending date and time to it
try:
    os.rename(old_file, new_file)
    print(f"File renamed to: {new_file}")
except OSError as e:
    print(f"Error renaming file: {e}")
    exit(1)

# Run the scp command to get new dataset.root
scp_command = ["scp", source_path, destination_path]
try:
    result = subprocess.run(scp_command, check=True)
except subprocess.CalledProcessError as e:
    print(f"Error during SCP transfer: {e}")
    exit(1)
