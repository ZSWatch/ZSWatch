#/bin/bash

export LS_OPTIONS='-F --color=auto'
alias ls='ls $LS_OPTIONS'

CHECK_FILE=".vscode/.zswatch_initialized"

if [ -f ${CHECK_FILE} ]; then
    echo -e "\e[32mAlready initialized\e[0m"
else
    echo -e "\e[32mPrepare system for first use\e[0m"
    source /tmp/init.sh
fi