<!-- markdownlint-disable no-inline-html first-line-h1 -->

<div align="center">
  <h1>libsnark Optimization</h1>
</div>

<!-- markdownlint-enable no-inline-html -->

Optimizing libsnark library 


## Installing Dependencies

### MacOS

```bash
brew install pkg-config cmake openjdk@11 openssl gmp libomp 
```

### Linux

```bash
sudo apt-get install build-essential pkg-config cmake libgmp-dev libssl-dev openjdk-11-jdk
```

Installing clang compiler (version 12) :

```bash
wget https://apt.llvm.org/llvm.sh

chmod +x llvm.sh

sudo ./llvm.sh 12

sudo apt-get install libc++-12-dev libc++abi-12-dev libomp-12-dev libunwind-12-dev 
```


## Download and compile libsnark ( MacOS / Linux )

Download :
```bash
git clone https://github.com/snp-labs/libsnark-optimization.git
cd libsnark-optimization
```

Compile :
```bash
make release=yes
```
or 
```bash
make release=yes silent=yes
```
to disable logging and printfs
or 
```bash
make release=no
```
to compile with debug symbols


## Running the sample application
```bash
cd test 
```

```bash
make 
```
to use the debug build of libsnark library , or 
```bash
make release
```
to use the release build of libsnark library


## iOS and Android Sample Apps

### iOS Sample App :

```bash
make ios_sample_app 
```

Open "test_ios_app" folder in Xcode


### Android Sample App :

```bash
make android_sample_app 
```

Open "test_android_app" folder in Android Studio



## API Documentation
See : [Documentation](https://snp-labs.github.io/libsnark-optimization-doc/).