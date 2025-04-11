#!/bin/bash

echo ">>> tar package"
echo 'version' >> version
tar czvf lts-cpp-sdk.tar.gz version
