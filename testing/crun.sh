#!/bin/bash

# Start time measurement
start=$(date +%s%N)
start=${start:0:13} 

# Run four instances of ./client simultaneously in the background
./test_scalability 127.0.0.1 &
./test_scalability 127.0.0.1 &
./test_scalability 127.0.0.1 &
./test_scalability 127.0.0.1 &
./test_scalability 127.0.0.1 &
./test_scalability 127.0.0.1 &
./test_scalability 127.0.0.1 &



# Wait for all background jobs to finish
wait

# End time measurement
end=$(date +%s%N)
end=${end:0:13}

# Calculate the duration
duration=$((end - start))

echo "Total execution time: $duration milliseconds"
