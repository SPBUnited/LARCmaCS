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


#!amd64: MATLAB_DIR = 'C:/Program Files (x86)/MATLAB/R2015b/extern'
!amd64: MATLAB_DIR = 'C:\Program Files (x86)\MATLAB\R2013b\extern'
!UNIX: MATLAB_DIR = '/usr/local/MATLAB/R2021b/extern/'

message("Path to matlab: $${MATLAB_DIR}")

#VCPKG_DIR = 'C:/vcpkg'
MSYS_DIR = 'C:\msys64'

