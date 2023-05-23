## SSL support 
use 
```
cmake -DHTTPS=true -DWSS=true 
```
or one of the above to enable https/ wss 

Also copy `.env.example` to `.env` and set the certificate paths.

## depenancies 
Apart from the conan libraries which are installed with 
```
mkdir build
cd build
conan install .. --build=missing
```
You should manually install the following libraries: 
- https://github.com/laserpants/dotenv-cpp
- https://github.com/kava-i/JanGeschenk
