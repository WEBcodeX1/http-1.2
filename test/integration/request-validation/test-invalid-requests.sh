#!/bin/bash

# test post request non-existing backend
wget --post-data='{"payload": 536}' --header='Content-Type: application/json' http://testapp1.local/backend/xyz

# test post request non-existing vhost
# wget --post-data='{"payload": 812}' --header='Content-Type: application/json' http://testapp2.local/back/xyz

# test get2post invalid parameters
wget "http://testapp2.local/backend/endpoint2?param3=test1&param4=test2"

# test get2post invalid backend endpoint
# wget "http://testapp2.local/backend/endp1?par1=test1&par2=test2"

# test get index (recursive content)
wget -r "http://testapp1.local/nonexistent.html" >/dev/null 2>&1
wget -r "http://testapp2.local/nonexistent.html" >/dev/null 2>&1
