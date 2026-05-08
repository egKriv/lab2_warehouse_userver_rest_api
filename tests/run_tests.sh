#!/bin/bash
echo "========================================="
echo "Running Inventory API Tests"
echo "========================================="
echo ""
echo "Checking if service is running..."
if curl -s http://localhost:8082/ping > /dev/null 2>&1; then
    echo "Service is running. Starting tests..."
    echo ""
    ./test_api.sh
    EXIT_CODE=$?
    echo ""
    if [ $EXIT_CODE -eq 0 ]; then
        echo "All tests completed successfully!"
    else
        echo "Some tests failed."
    fi
    exit $EXIT_CODE
else
    echo "ERROR: Service is not running on http://localhost:8080"
    exit 1
fi