#!/bin/sh
readonly SKYWALKING_HOME="/data"

readonly SKYWALKING_CONF="/etc/skywalking.cnf"


toConsole() {
	echo "$(date +"%H:%M:%S")" "$@"
}

createDirIfNotExistAndSetRights() {
	local dir="${1}"
	local rights="${2}"
	if [ ! -d "${dir}" ]; then
		if ! mkdir -p "${dir}"; then
			toConsole "Cannot create ${dir} directory."
		fi		
	fi
	
	if ! chmod "${rights}" "${dir}"; then
		toConsole "Cannot change permisions of ${dir} directory to ${rights}."
	fi

}

main() {
	toConsole "Checking root privileges..."
    local MY_UID="$(id -u)"
    if [ "${MY_UID}" != "0" ];then
        toConsole "no privileges"
    fi
    createDirIfNotExistAndSetRights "${SKYWALKING_HOME}" 755
    touch ${SKYWALKING_CONF}
    toConsole "download from skywalking.." 
    wget  http://10.92.194.148/skyagentsimple -P ${SKYWALKING_HOME} && cd ${SKYWALKING_HOME} &&  tar -xf skyagentsimple
	rm -f  /etc/skywalking.cnf
	echo "[skywalking_client]" >> /etc/skywalking.cnf
    echo "SW_AGENT_PATH="-javaagent:${SKYWALKING_HOME}/apache-skywalking-apm-bin/agent/skywalking-agent.jar >> /etc/skywalking.cnf
	echo "[skywalking_server]" >> /etc/skywalking.cnf
	echo "SW_AGENT_COLLECTOR_BACKEND_SERVICES=10.92.193.115:9999" >> /etc/skywalking.cnf
	echo "SW_GRPC_LOG_SERVER_HOST=10.92.193.115" >> /etc/skywalking.cnf
	echo "SW_GRPC_LOG_SERVER_PORT=9999" >> /etc/skywalking.cnf
    toConsole "init success";
}

main "$@"