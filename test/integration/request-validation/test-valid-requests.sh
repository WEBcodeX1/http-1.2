#!/bin/bash

# test post request
wget --post-data='{"payload": 123}' --header='Content-Type: application/json' http://testapp1.local/backend/ >/dev/null 2>&1

# test get to post translation
wget "http://testapp2.local/backend/endpoint2?param1=test1&param2=test2" >/dev/null 2>&1

# test get index (recursive content)
wget -r "http://testapp1.local/index.html" >/dev/null 2>&1
wget -r "http://testapp2.local/index.html" >/dev/null 2>&1

# validate request results
res1=`cat index.html`
res2=`cat endpoint2\?param1\=test1\&param2\=test2`
index_size1=`ls -la testapp1.local/ | grep index | cut -d " " -f16`
index_size2=`ls -la testapp2.local/ | grep index | cut -d " " -f16`
image_size1=`ls -la testapp1.local/ | grep falcon | cut -d " " -f13`
image_size2=`ls -la testapp2.local/ | grep falcon | cut -d " " -f13`

echo "Result1: ${res1} Result2:${res2} IndexSize1:${index_size1} IndexSize2:${index_size2} ImageSize1:${image_size1} ImageSize2:${image_size2}"

# cleanup
rm index.html
rm endpoint2*
rm -Rf ./testapp1.local
rm -Rf ./testapp2.local

if [ "$res1" != "123" ]; then
    echo "test 1 failed"
    exit 1
fi

if [ "$res2" != "{'param1': 'test1', 'param2': 'test2'}" ]; then
    echo "test 2 failed"
    exit 1
fi

if [ "$index_size1" != "120" ]; then
    echo "test 3 failed"
    exit 1
fi

if [ "$index_size2" != "120" ]; then
    echo "test 4 failed"
    exit 1
fi

if [ "$image_size1" != "118236" ]; then
    echo "test 5 failed"
    exit 1
fi

if [ "$image_size2" != "118236" ]; then
    echo "test 6 failed"
    exit 1
fi

echo "all tests passed!"
exit 0
