#!/bin/bash

################################################################################
#
#
#
#
################################################################################

#
##
#

usage () {
    echo "$(basename ${0}) header.h output.py"
}

help() {
    echo -e ""
    echo -e " header.h\tThe header file to convert"
    echo -e " output.py\tThe Python file in which to put the CFFI declaration"
    echo -e ""
    echo -e " -h, --help\tPrint this help"
    echo -e ""
}

_HEADER=""
_OUTPUT=""


# Parsing arguments
while [ ${#} -gt 0 ]
do
    case ${1} in
        --help|-h)
            usage
            help
            exit 0
            ;;
        *)
            if [ -z "${_HEADER}" ]
            then
                _HEADER=${1}
            else
                _OUTPUT=${1}
            fi
    esac
    shift
done

if [ -z "${_HEADER}" -o -z "${_OUTPUT}" ]
then
    usage >&2
    exit 1
fi

if [ ! -e "${_HEADER}" ]
then
    echo "The given file does not exist! File: '${_HEADER}'" >&2
    exit 2
fi

# Converting the header
cat > "${_OUTPUT}" <<EOF
#!/usr/bin/env python
# -*- coding: utf-8 -*-
###############################################################################
# Author: BXI Automated Conversion Tools
# Contributors:
###############################################################################
# Copyright (C) %%YEAR%%  Bull S. A. S.  -  All rights reserved
# Bull, Rue Jean Jaures, B.P.68, 78340, Les Clayes-sous-Bois
# This is not Free or Open Source software.
# Please contact Bull S. A. S. for details about its license.
###############################################################################

C_DEF = """
EOF

sed "s/%%YEAR%%/$(date +%Y)/" -i "${_OUTPUT}"

gcc -DBXICFFI -E -D__GNUC__=0 -w ${_HEADER} | grep -v '^#' | sed '/^$/d;N' >> "${_OUTPUT}"
#gcc -DBXICFFI -E -D__GNUC__=0 -w ${_HEADER} | sed '/^$/d;N' >> "${_OUTPUT}"
rc=${?}

cat >> "${_OUTPUT}" <<EOF
"""
EOF

# If something gone wrong, delete the generated file
if [ ${rc} -ne 0 ]
then
    rm -f "${_OUTPUT}"
fi


exit ${rc}


#
##
#
