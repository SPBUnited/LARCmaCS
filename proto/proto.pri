# Copyright 2019 Irina Khonakhbeeva

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


#define path to directories with protobuf headers and libs
CONFIG += protobuf

mingw {
  PROTO_DIR = $${MSYS_DIR}/mingw$${BIT}
  LIBS += -lprotobuf
  PROTOC_DIR = $${PROTO_DIR}/bin/
  DLLS += $$MSYS_DIR/mingw$${BIT}/bin/libprotobuf.dll
}
msvc:!amd64: PROTO_DIR = $${VCPKG_DIR}/installed/x86-windows
msvc:amd64:  PROTO_DIR = $${VCPKG_DIR}/installed/x64-windows
msvc {
  LIBS += -L$${PROTO_DIR}/$${PREFIX_STR}lib/ -llibprotobuf$${SUFFIX_STR}
  PROTOC_DIR = $${PROTO_DIR}/tools/protobuf/
  DLLS += $$PROTO_DIR/$${PREFIX_STR}bin/libprotobuf$${SUFFIX_STR}.dll
}
unix {
  LIBS += -lprotobuf
}

INCLUDEPATH += $${PROTO_DIR}/include
PROTO_GENERATED_DIR = $$PWD/generated/
INCLUDEPATH += $$PROTO_GENERATED_DIR

old_ssl: PROTOS = $$files($$PWD/ssl-2009/*.proto)
!old_ssl: PROTOS = $$files($$PWD/ssl-2018/*.proto)
!old_ssl: PROTOS += $$files($$PWD/simulator/*.proto)
PROTOS += $$files($$PWD/referee/*.proto)
