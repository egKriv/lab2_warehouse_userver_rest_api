#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
BASE_URL="http://localhost:8082"
PASSED=0
FAILED=0
TOKEN=""

test_endpoint() {
    local name="$1"; local method="$2"; local endpoint="$3"
    local data="$4"; local expected_code="$5"; local auth="$6"
    echo -n "Testing: $name ... "
    local cmd="curl -s -o /tmp/resp.txt -w '%{http_code}' -X $method"
    [ -n "$data" ] && cmd="$cmd -H 'Content-Type: application/json' -d '$data'"
    [ "$auth" = "true" ] && [ -n "$TOKEN" ] && cmd="$cmd -H 'Authorization: Bearer $TOKEN'"
    cmd="$cmd $BASE_URL$endpoint"
    local http_code=$(eval $cmd)
    local response=$(cat /tmp/resp.txt)
    if [ "$http_code" = "$expected_code" ]; then
        echo -e "${GREEN}PASSED${NC} (HTTP $http_code)"; ((PASSED++)); return 0
    else
        echo -e "${RED}FAILED${NC} (Expected $expected_code, got $http_code)"; echo "Response: $response"; ((FAILED++)); return 1
    fi
}

echo "=== Inventory API Tests ==="
# 1. Ping
test_endpoint "Ping" "GET" "/ping" "" "200" "false"

# 2. Register user
USER="test_$(date +%s)_$$"
test_endpoint "Register" "POST" "/api/auth/register" \
  "{\"login\":\"$USER\",\"password\":\"test123\",\"first_name\":\"Test\",\"last_name\":\"User\",\"email\":\"${USER}@test.com\"}" "201" "false"

# 3. Register duplicate
test_endpoint "Register duplicate" "POST" "/api/auth/register" \
  "{\"login\":\"$USER\",\"password\":\"test123\",\"first_name\":\"Test\",\"last_name\":\"User\",\"email\":\"${USER}@test.com\"}" "409" "false"

# 4. Login
echo -n "Testing: Login ... "
HTTP=$(curl -s -w '%{http_code}' -o /tmp/login.json -X POST "$BASE_URL/api/auth/login" -H "Content-Type: application/json" -d "{\"login\":\"$USER\",\"password\":\"test123\"}")
if [ "$HTTP" = "200" ]; then
    echo -e "${GREEN}PASSED${NC} (HTTP 200)"
    ((PASSED++))
    TOKEN=$(jq -r '.access_token' /tmp/login.json)
else
    echo -e "${RED}FAILED${NC} ($HTTP)"; ((FAILED++))
fi

# 5. Login wrong password
test_endpoint "Login wrong password" "POST" "/api/auth/login" "{\"login\":\"$USER\",\"password\":\"wrong\"}" "401" "false"

# 6. Search user by login (requires auth)
test_endpoint "Search user by login" "GET" "/api/users/by-login/$USER" "" "200" "true"

# 7. Search user by name mask
test_endpoint "Search user by mask" "GET" "/api/users/search?name_mask=Test" "" "200" "true"

# 8. Add product
test_endpoint "Add product" "POST" "/api/products" "{\"name\":\"Test Product\",\"initial_quantity\":100}" "201" "true"
PRODUCT_ID=$(cat /tmp/resp.txt | jq -r '.id')

# 9. Search product by name
test_endpoint "Search product" "GET" "/api/products/search?name=Product" "" "200" "false"

# 10. Get stock
test_endpoint "Get stock" "GET" "/api/products/stock" "" "200" "false"

# 11. Create receipt
test_endpoint "Create receipt" "POST" "/api/receipts" "{\"product_id\":\"$PRODUCT_ID\",\"quantity\":50,\"supplier\":\"Supplier Inc\"}" "201" "true"

# 12. Get receipt history
test_endpoint "Receipt history" "GET" "/api/receipts?product_id=$PRODUCT_ID" "" "200" "true"

# 13. Write-off valid
test_endpoint "Write-off" "POST" "/api/inventory/write-off" "{\"product_id\":\"$PRODUCT_ID\",\"quantity\":30}" "200" "true"

# 14. Write-off more than stock
test_endpoint "Write-off insufficient" "POST" "/api/inventory/write-off" "{\"product_id\":\"$PRODUCT_ID\",\"quantity\":999}" "409" "true"

# 15. Unauthorized access
test_endpoint "Add product no token" "POST" "/api/products" "{\"name\":\"x\",\"initial_quantity\":1}" "401" "false"

echo ""
echo "Results: Passed: $PASSED, Failed: $FAILED, Total: $((PASSED+FAILED))"
[ $FAILED -eq 0 ] && exit 0 || exit 1