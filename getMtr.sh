#!/bin/bash

# Check if an argument is passed
if [ $# -ne 1 ]; then
  echo "Usage: $0 <search_string>"
  exit 1
fi

# Get the input string from the argument
SEARCH_STRING=$1

# Database file (change this to your actual database path)
DATABASE="/home/ubuntu/dac/rdk-appstore-metadata/maintainer_data.db"

# Run the SQL query and print the result
sqlite3 "$DATABASE" <<EOF
SELECT a.maintainer_code
FROM apps a
WHERE a.header LIKE '%$SEARCH_STRING%';
EOF

