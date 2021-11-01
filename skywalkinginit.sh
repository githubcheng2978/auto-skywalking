#!/bin/sh
. /etc/init.d/functions
. /etc/rc.d/init.d/functions
readonly SKYWALKING_HOME="/data/skywalkingoneagent"
readonly SKYWALKING_CONF="/etc/skywalking.cnf"
readonly AGENT="libprocessjava.so"
readonly DOWNLOADURL="download"
readonly SW_AGENT_COLLECTOR_BACKEND_SERVICES="skywalking:9999"
readonly SW_GRPC_LOG_SERVER_HOST="skywalking"
readonly SW_GRPC_LOG_SERVER_PORT="9999"

toConsole() {
	echo "$(date +"%H:%M:%S")" "$@"
}

toActionConsole() {
	action "$(date +"%H:%M:%S") $@" /bin/true
}

restoreSELinuxContext() {
	local file="${1}"
	if [ ! -e "${file}" ]; then
		return
	fi

	if ! which restorecon >/dev/null 2>&1; then
		return
	fi

	toConsole "Restoring default SELinux security context for ${file}"
	if ! restorecon -R ${file} >/dev/null 2>&1; then
		toConsole "Failed to restore default SELinux security context for ${file}"
	fi
}
restoreSELinuxContexts() {
	restoreSELinuxContext /usr/local/lib64/${AGENT}
	restoreSELinuxContext /etc/ld.so.preload
}

createDirIfNotExistAndSetRights() {
	local dir="${1}"
	local rights="${2}"
	if [ ! -d "${dir}" ]; then
		if ! mkdir -p "${dir}"; then
			toConsole "Cannot create ${dir} directory."
		fi
	else
		rm -rf "${dir}"
	fi
	if ! chmod "${rights}" "${dir}"; then
		toConsole "Cannot change permisions of ${dir} directory to ${rights}."
	fi

}

installPreloadFile() {
	toConsole "installPreloadFile /etc/ld.so.preload..."
	if [ -e /etc/ld.so.preload ]; then
		sed -i '/${AGENT}/d' /etc/ld.so.preload
		echo /usr/local/lib64/${AGENT} >>/etc/ld.so.preload
	else
		echo /usr/local/lib64/${AGENT} >/etc/ld.so.preload
	fi
}

main() {
	toConsole "Checking root privileges..."
	local MY_UID="$(id -u)"
	if [ "${MY_UID}" != "0" ]; then
		toConsole "no privileges"
		return 1
	fi
	createDirIfNotExistAndSetRights "${SKYWALKING_HOME}" 755
	touch ${SKYWALKING_CONF}
	toConsole "download from skywalking.."
	wget ${DOWNLOADURL}/skyagentsimple -P ${SKYWALKING_HOME} && cd ${SKYWALKING_HOME} && tar -xf skyagentsimple
	if [ $? -eq 0 ]; then
		toActionConsole "download skywalkingonagent successfully!"
	else
		echo "download skywalkingonagent from ${DOWNLOADURL}/skyagentsimple failed"
		return 1
	fi
	rm -f /etc/skywalking.cnf
	echo "[skywalking_client]" >>/etc/skywalking.cnf
	echo "SW_AGENT_PATH="-javaagent:${SKYWALKING_HOME}/apache-skywalking-apm-bin/agent/skywalking-agent.jar >>/etc/skywalking.cnf
	echo "[skywalking_server]" >>/etc/skywalking.cnf
	echo "SW_AGENT_COLLECTOR_BACKEND_SERVICES=${SW_AGENT_COLLECTOR_BACKEND_SERVICES}" >>/etc/skywalking.cnf
	echo "SW_GRPC_LOG_SERVER_HOST=${SW_GRPC_LOG_SERVER_HOST}" >>/etc/skywalking.cnf
	echo "SW_GRPC_LOG_SERVER_PORT=${SW_GRPC_LOG_SERVER_PORT}" >>/etc/skywalking.cnf

	toActionConsole "init skywalkingonagent successfully!"

	installPreloadFile
	wget ${DOWNLOADURL}/${AGENT} -P ${SKYWALKING_HOME} && mv ${SKYWALKING_HOME}/${AGENT} /usr/local/lib64
	if [ $? -eq 0 ]; then
		toActionConsole "download ${AGENT} successfully!"
	fi
	restoreSELinuxContexts
	# reload systemctl
	systemctl daemon-reexec
	toActionConsole "install libprocessjava successfully!"
}

main "$@"
