#!/usr/bin/env python
import sys, errno
print errno.errorcode[int(sys.argv[1])]
