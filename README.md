# SRLE
Stream Run Length Encoding

I wrote this for some rocket club experiments, so dont expect this to work or anything (although the tests say it does...)

Does whats on the tin. Datastream goes in, run length encoded datastream goes out. 
Should be O(n).

Additional Data needed (for the lib alone) should be below 10 bytes or so (counter + last byte + function pointer + stack)

