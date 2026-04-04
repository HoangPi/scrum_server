# Project management using SCRUM framework

Server of project management.

## Test accounts
```json
{
  username: "creator_alpha",
  password: "^Fwnsu^YiVD[Q<|j"
}
```
```json
{
  username: "regular_user",
  password: "kV=z}'~KKTJ0TCmg"
}
```
## Overview

This project is using the following oatpp modules:

- [oatpp](https://github.com/oatpp/oatpp) 
- [oatpp-swagger](https://github.com/oatpp/oatpp-swagger)
- [oatpp-postgresql](https://github.com/oatpp/oatpp-postgresql)
- [jwt-cpp](https://github.com/Thalhammer/jwt-cpp)

### Build and Run
  

#### Using CMake

##### Pre Requirements

- `oatpp` 
- `oatpp-swagger`
- `oatpp-sqlite` with `-DOATPP_SQLITE_AMALGAMATION=ON` cmake flag.

**Note:** 
- You may run `utility/install-oatpp-modules.sh` script to install required oatpp modules.
- You may need to pull git submodule bcrypt-cpp before installing.
- You must install [jwt-cpp](https://github.com/Thalhammer/jwt-cpp) before installing.

##### Build Project

```
$ mkdir build && cd build
$ cmake ..
$ make 
$ ./crud-exe        # - run application.
```

#### In Docker

```
$ docker build -t example-crud .
$ docker run -p 8000:8000 -t example-crud
```
