-- This is the property file for the zergling

-- Configurations

--
-- [COMMON]
--
VERSION="0.4.0"

HEARTBEAT_INTERVAL=15
SERVER_HOST="111.13.47.154"
SERVER_PORT=31201

TDFI_PORT=31200
TCP_CONN_MAX=5

-- Retrieve the mac addr
function get_mac()
	local file = io.popen('ifconfig br0|grep HWaddr')
	if file == nil then return "" end

	local outline = file:read()
	file:close()
	if outline == nil then return "" end

	return outline:match("%w%w:%w%w:%w%w:%w%w:%w%w:%w%w"):gsub(':', ''):upper()
end
MAC_ADDR=get_mac()

-- SPAWN_NGINX_SCRIPT="spawn.sh"

--
-- [BUNDLE]
--
SMART_RSYNC_SCRIPT="rsync.sh"
RSYNC_DEST="/opt/share/www"

--
-- [LED]
--
LED_DEVICE="/dev/ttyUSB6"

--
-- [ERROR code]
--
errors={
[0] = "Success",
[1] = "Syntax or usage error",
[2] = "Protocol incompatibility",
[3] = "Errors selecting input/output files, dirs",
[4] = "Requested action not supported",
[5] = "Error starting client-server protocol",
[6] = "Daemon unable to append to log-file",
[10] = "Error in socket I/O",
[11] = "Error in file I/O",
[12] = "Error in rsync protocol data stream",
[13] = "Errors with program diagnostics",
[14] = "Error in IPC code",
[20] = "Received SIGUSR1 or SIGINT",
[21] = "Some error returned by waitpid()",
[22] = "Error allocating core memory buffers",
[23] = "Partial transfer due to error",
[24] = "Partial transfer due to vanished source files",
[25] = "The --max-delete limit stopped deletions",
[30] = "Timeout in data send/receive",
[35] = "Timeout waiting for daemon connection",
[36] = "rsync binary not exists",
[255] = "General error",
}
-- Retrieve the error info for the given error code
function strerror(err_code)
	return errors[err_code]
end

--
-- [Methods]
--
-- Decode the request
function decode (req) 
	local params = {}	
	valid, req_type = validate(req)
	if not valid then
		return params, valid
	end

	params["REQ"] = req_type
	for name, value in string.gmatch(req, "([^&=%s]+)=([^&=%s]+)") do 
    	params[name] = value 
  	end 
	return params, valid
end 

-- Validate the request
function validate(req)
	local pattern1="^id=%w+&action=get&target=bundle$"
--	local pattern2="^id=%w+&action=set&target=bundle&url=.*&manner=%u+&version=[%w_%-%.]+$"
	local pattern2="^id=%w+&action=set&target=bundle&url=rsync://.*$"
--	local pattern3="^id=%w+&action=set&target=led&msg=.*$"

	if string.match(req, pattern1) ~= nil then
		return true, "GET_BUNDLE_VER"
	elseif string.match(req, pattern2) ~= nil then
		return true, "SET_BUNDLE"
	elseif string.match(req, pattern3) ~= nil then
		return true, "SET_LED"
	else
		return false, "INVALID_REQUEST"
	end
end
