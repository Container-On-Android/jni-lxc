# jni-lxc
Java bindings for liblxc on Android

# LICENSE
## Multi-license Notice for This Project  
This software project is distributed under multiple open source licenses. Different parts of the code are governed by different licenses as specified below.
﻿
### 1. The MIT License (MIT) - Applies to the Majority of This Project
Unless otherwise explicitly stated for specific files (see Section 2), **all original source code and content in this repository** is licensed under the permissive MIT License.  

MIT License:
```
Copyright (c) 2025 Container On Android

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 2. GNU Lesser General Public License v2.1 (LGPLv2.1) - Applies to Specific Modified Files
The following source files are derived from, and remain licensed under, the GNU Lesser General Public License version 2.1 (LGPLv2.1), including the special exception clause present in the original library. This applies only to these files and any modifications made to them by the contributor of this project.  

List of LGPLv2.1 Licensed Files:

### lxc [link](https://github.com/Container-On-Android/lxc)
- app/src/main/jni/deps/arm64-v8a/liblxc.a
- app/src/main/jni/src/include/lxc/attach_options.h
- app/src/main/jni/src/include/lxc/lxccontainer.h
- app/src/main/jni/src/include/lxc/version.h
### go-lxc [link](https://github.com/lxc/go-lxc)
- app/src/main/jni/src/lxc-binding.c
- app/src/main/jni/src/lxc-binding.h

For these files, the terms and conditions of the LGPLv2.1 fully apply. Notably, if you distribute a binary that incorporates these files (statically or dynamically linked), you must comply with the requirements of LGPLv2.1 regarding the provision of corresponding source code for these specific files.  

## LGPLv2.1 Obligations Summary for These Files:  

If you distribute this software (in source or binary form), you must provide the complete corresponding source code for the modified versions of the files listed above.  

You must retain all copyright notices and the full LGPLv2.1 license text for these files.
﻿
You must allow recipients to reverse engineer the binary solely to debug modifications made to these LGPL-covered files.
