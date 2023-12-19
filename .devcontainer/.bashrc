export LS_OPTIONS='-F --color=auto'
alias ls='ls $LS_OPTIONS'

if [ ! -f "/tmp/initialized" ]; then
	echo -e "\e[32mPrepare system for first use\e[0m"
	source /tmp/init.sh
fi