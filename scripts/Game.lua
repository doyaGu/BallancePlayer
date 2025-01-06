local ffi = require("ffi")

--------------------------------------------------------------------------------
-- 1) Define all necessary constants, enums, and structs
--------------------------------------------------------------------------------
ffi.cdef([[
typedef struct BpGamePlayer BpGamePlayer;
typedef struct BpGameConfig BpGameConfig;
typedef struct BpLogger BpLogger;

// Enums
typedef enum BpGamePlayerState {
    BP_PLAYER_INITIAL = 0,
    BP_PLAYER_READY,
    BP_PLAYER_PLAYING,
    BP_PLAYER_PAUSED,
    BP_PLAYER_FOCUS_LOST,
} BpGamePlayerState;

typedef enum BpPathCategory {
    BP_PATH_CONFIG = 0,
    BP_PATH_LOG,
    BP_PATH_CMO,
    BP_PATH_ROOT,
    BP_PATH_PLUGINS,
    BP_PATH_RENDER_ENGINES,
    BP_PATH_MANAGERS,
    BP_PATH_BUILDING_BLOCKS,
    BP_PATH_SOUNDS,
    BP_PATH_TEXTURES,
    BP_PATH_DATA,
    BP_PATH_SCRIPTS,
    BP_PATH_CATEGORY_COUNT,
} BpPathCategory;

typedef enum BpLogMode {
    BP_LOG_MODE_APPEND = 0,
    BP_LOG_MODE_OVERWRITE,
} BpLogMode;

typedef enum BpLogLevel {
    BP_LOG_TRACE = 0,
    BP_LOG_DEBUG,
    BP_LOG_INFO,
    BP_LOG_WARN,
    BP_LOG_ERROR,
    BP_LOG_FATAL,
    BP_LOG_OFF,
} BpLogLevel;

typedef struct BpLogInfo {
    BpLogger *self;
    BpLogLevel level;
    va_list args;
    const char *format;
    struct tm *time;
    void *userdata;
} BpLogInfo;

typedef void (*BpLogCallback)(BpLogInfo *info);
typedef void (*BpLogLockFunction)(bool lock, void *userdata);

// Logger API
BpLogger* bpGetLogger(const char *name);
BpLogger* bpGetDefaultLogger();
void bpSetDefaultLogger(BpLogger *logger);
int bpLoggerAddRef(BpLogger *logger);
int bpLoggerRelease(BpLogger *logger);
const char* bpLoggerGetName(BpLogger *logger);
BpLogLevel bpLoggerGetLevel(BpLogger *logger);
const char* bpLoggerGetLevelString(BpLogger *logger, BpLogLevel level);
void bpLoggerSetLevel(BpLogger *logger, BpLogLevel level);
void bpLoggerSetLock(BpLogger *logger, BpLogLockFunction func, void *userdata);
bool bpLoggerAddCallback(BpLogger *logger, BpLogCallback callback, void *userdata, BpLogLevel level);
void bpLoggerClearCallbacks(BpLogger *logger);
void bpLoggerLog(BpLogger *logger, BpLogLevel level, const char *format, ...);
void bpLoggerTrace(BpLogger *logger, const char *format, ...);
void bpLoggerDebug(BpLogger *logger, const char *format, ...);
void bpLoggerInfo(BpLogger *logger, const char *format, ...);
void bpLoggerWarn(BpLogger *logger, const char *format, ...);
void bpLoggerError(BpLogger *logger, const char *format, ...);
void bpLoggerFatal(BpLogger *logger, const char *format, ...);
void bpLog(BpLogLevel level, const char *format, ...);
void bpLogTrace(const char *format, ...);
void bpLogDebug(const char *format, ...);
void bpLogInfo(const char *format, ...);
void bpLogWarn(const char *format, ...);
void bpLogError(const char *format, ...);
void bpLogFatal(const char *format, ...);

// GameConfig API
BpGameConfig* bpCreateGameConfig(const char *name);
void bpDestroyGameConfig(BpGameConfig *config);
bool bpGameConfigIsInitialized(const BpGameConfig *config);
bool bpGameConfigInit(BpGameConfig *config);
void bpGameConfigRelease(BpGameConfig *config);
bool bpGameConfigRead(BpGameConfig *config, const char *json, size_t size);
char* bpGameConfigWrite(const BpGameConfig *config);
void bpGameConfigFree(const BpGameConfig *config, char *json);
bool bpGameConfigLoad(BpGameConfig *config, const char *filename);
bool bpGameConfigSave(const BpGameConfig *config, const char *filename);
const char* bpGetGamePath(const BpGameConfig *config, BpPathCategory category);
bool bpSetGamePath(BpGameConfig *config, BpPathCategory category, const char *path);
bool bpHasGamePath(const BpGameConfig *config, BpPathCategory category);
bool bpResetGamePath(BpGameConfig *config, BpPathCategory category);

// GamePlayer API
BpGamePlayer* bpCreateGamePlayer(const char *name);
void bpDestroyGamePlayer(BpGamePlayer *player);
BpGamePlayer* bpGetGamePlayerById(int id);
BpGamePlayer* bpGetGamePlayerByName(const char *name);
int bpGamePlayerGetId(const BpGamePlayer *player);
const char* bpGamePlayerGetName(const BpGamePlayer *player);
BpGamePlayerState bpGamePlayerGetState(const BpGamePlayer *player);
BpLogger* bpGamePlayerGetLogger(const BpGamePlayer *player);
BpGameConfig* bpGamePlayerGetConfig(const BpGamePlayer *player);
void* bpGamePlayerGetMainWindow(const BpGamePlayer *player);
void* bpGamePlayerGetRenderWindow(const BpGamePlayer *player);
bool bpGamePlayerInit(BpGamePlayer *player, BpGameConfig *config, void *hInstance);
bool bpGamePlayerLoad(BpGamePlayer *player, const char *filename);
void bpGamePlayerRun(BpGamePlayer *player);
bool bpGamePlayerUpdate(BpGamePlayer *player);
void bpGamePlayerProcess(BpGamePlayer *player);
void bpGamePlayerRender(BpGamePlayer *player);
bool bpGamePlayerShutdown(BpGamePlayer *player);
void bpGamePlayerPlay(BpGamePlayer *player);
void bpGamePlayerPause(BpGamePlayer *player);
void bpGamePlayerReset(BpGamePlayer *player);
]])

--------------------------------------------------------------------------------
-- 2) Load the Game.dll shared library
--------------------------------------------------------------------------------
local ok, game = pcall(ffi.load, "Game.dll")
if not ok then
    error("Failed to load Game.dll: " .. tostring(game))
end

--------------------------------------------------------------------------------
-- 3) Define Logger class
--------------------------------------------------------------------------------
local Logger = {}
Logger.__index = Logger

-- Constructor for a named logger
function Logger:new(name)
    local loggerPtr = game.bpGetLogger(name)
    if loggerPtr == nil then
        error("Failed to create logger with name: " .. tostring(name))
    end
    local obj = {
        _logger = loggerPtr
    }
    return setmetatable(obj, Logger)
end

-- Alternative constructor for the default logger
function Logger.getDefault()
    local loggerPtr = game.bpGetDefaultLogger()
    if loggerPtr == nil then
        error("Failed to get the default logger.")
    end
    return setmetatable({ _logger = loggerPtr }, Logger)
end

-- (Optional) Set the passed-in logger as the default
function Logger.setDefault(logger)
    if logger and logger._logger then
        game.bpSetDefaultLogger(logger._logger)
    else
        error("Invalid logger supplied to setDefault.")
    end
end

function Logger:addRef()
    return game.bpLoggerAddRef(self._logger)
end

function Logger:release()
    return game.bpLoggerRelease(self._logger)
end

function Logger:getName()
    return ffi.string(game.bpLoggerGetName(self._logger))
end

function Logger:getLevel()
    return game.bpLoggerGetLevel(self._logger)
end

function Logger:getLevelString(level)
    return ffi.string(game.bpLoggerGetLevelString(self._logger, level))
end

function Logger:setLevel(level)
    game.bpLoggerSetLevel(self._logger, level)
end

function Logger:setLock(lockFunction, userdata)
    game.bpLoggerSetLock(self._logger, lockFunction, userdata)
end

function Logger:addCallback(callback, userdata, level)
    return game.bpLoggerAddCallback(self._logger, callback, userdata, level)
end

function Logger:clearCallbacks()
    game.bpLoggerClearCallbacks(self._logger)
end

-- Generic log call with a specified level
function Logger:log(level, format, ...)
    game.bpLoggerLog(self._logger, level, format, ...)
end

function Logger:trace(format, ...)
    game.bpLoggerTrace(self._logger, format, ...)
end

function Logger:debug(format, ...)
    game.bpLoggerDebug(self._logger, format, ...)
end

function Logger:info(format, ...)
    game.bpLoggerInfo(self._logger, format, ...)
end

function Logger:warn(format, ...)
    game.bpLoggerWarn(self._logger, format, ...)
end

function Logger:error(format, ...)
    game.bpLoggerError(self._logger, format, ...)
end

function Logger:fatal(format, ...)
    game.bpLoggerFatal(self._logger, format, ...)
end

--------------------------------------------------------------------------------
-- 4) Define GameConfig class
--------------------------------------------------------------------------------
local GameConfig = {}
GameConfig.__index = GameConfig

function GameConfig:new(name)
    local configPtr = game.bpCreateGameConfig(name)
    if configPtr == nil then
        error("Failed to create game config: " .. tostring(name))
    end
    local obj = {
        _config = configPtr
    }
    return setmetatable(obj, GameConfig)
end

function GameConfig:destroy()
    game.bpDestroyGameConfig(self._config)
    self._config = nil
end

function GameConfig:isInitialized()
    return game.bpGameConfigIsInitialized(self._config)
end

function GameConfig:init()
    return game.bpGameConfigInit(self._config)
end

function GameConfig:release()
    game.bpGameConfigRelease(self._config)
end

function GameConfig:read(jsonStr, size)
    return game.bpGameConfigRead(self._config, jsonStr, size)
end

function GameConfig:write()
    local jsonPtr = game.bpGameConfigWrite(self._config)
    if jsonPtr == nil then
        error("Failed to write game config to JSON.")
    end
    local jsonString = ffi.string(jsonPtr)
    -- Must free the JSON string buffer
    game.bpGameConfigFree(self._config, jsonPtr)
    return jsonString
end

function GameConfig:load(filename)
    return game.bpGameConfigLoad(self._config, filename)
end

function GameConfig:save(filename)
    return game.bpGameConfigSave(self._config, filename)
end

function GameConfig:getPath(category)
    local pathPtr = game.bpGetGamePath(self._config, category)
    if pathPtr == nil then
        return nil
    end
    return ffi.string(pathPtr)
end

function GameConfig:setPath(category, path)
    return game.bpSetGamePath(self._config, category, path)
end

function GameConfig:hasPath(category)
    return game.bpHasGamePath(self._config, category)
end

function GameConfig:resetPath(category)
    return game.bpResetGamePath(self._config, category)
end

--------------------------------------------------------------------------------
-- 5) Define GamePlayer class
--------------------------------------------------------------------------------
local GamePlayer = {}
GamePlayer.__index = GamePlayer

function GamePlayer:new(name)
    local playerPtr = game.bpCreateGamePlayer(name)
    if playerPtr == nil then
        error("Failed to create game player with name: " .. tostring(name))
    end
    local obj = {
        _player = playerPtr
    }
    return setmetatable(obj, GamePlayer)
end

function GamePlayer:destroy()
    game.bpDestroyGamePlayer(self._player)
    self._player = nil
end

-- Static lookup by ID
function GamePlayer.getById(id)
    local playerPtr = game.bpGetGamePlayerById(id)
    if playerPtr == nil then
        error("Game player with ID " .. id .. " not found.")
    end
    return setmetatable({ _player = playerPtr }, GamePlayer)
end

-- Static lookup by name
function GamePlayer.getByName(name)
    local playerPtr = game.bpGetGamePlayerByName(name)
    if playerPtr == nil then
        error("Game player with name '" .. name .. "' not found.")
    end
    return setmetatable({ _player = playerPtr }, GamePlayer)
end

function GamePlayer:getId()
    return game.bpGamePlayerGetId(self._player)
end

function GamePlayer:getName()
    local namePtr = game.bpGamePlayerGetName(self._player)
    return namePtr and ffi.string(namePtr) or nil
end

function GamePlayer:getState()
    return game.bpGamePlayerGetState(self._player)
end

function GamePlayer:getLogger()
    local loggerPtr = game.bpGamePlayerGetLogger(self._player)
    if loggerPtr == nil then
        return nil
    end
    -- Wrap the returned C pointer in a Logger instance
    return setmetatable({ _logger = loggerPtr }, Logger)
end

function GamePlayer:getConfig()
    local configPtr = game.bpGamePlayerGetConfig(self._player)
    if configPtr == nil then
        return nil
    end
    -- Wrap the returned C pointer in a GameConfig instance
    return setmetatable({ _config = configPtr }, GameConfig)
end

function GamePlayer:getMainWindow()
    return game.bpGamePlayerGetMainWindow(self._player)
end

function GamePlayer:getRenderWindow()
    return game.bpGamePlayerGetRenderWindow(self._player)
end

function GamePlayer:init(gameConfig, hInstance)
    -- Pass the raw config pointer to the C function
    return game.bpGamePlayerInit(self._player, gameConfig._config, hInstance)
end

function GamePlayer:load(filename)
    return game.bpGamePlayerLoad(self._player, filename)
end

function GamePlayer:run()
    game.bpGamePlayerRun(self._player)
end

function GamePlayer:update()
    return game.bpGamePlayerUpdate(self._player)
end

function GamePlayer:process()
    game.bpGamePlayerProcess(self._player)
end

function GamePlayer:render()
    game.bpGamePlayerRender(self._player)
end

function GamePlayer:shutdown()
    return game.bpGamePlayerShutdown(self._player)
end

function GamePlayer:play()
    game.bpGamePlayerPlay(self._player)
end

function GamePlayer:pause()
    game.bpGamePlayerPause(self._player)
end

function GamePlayer:reset()
    game.bpGamePlayerReset(self._player)
end

--------------------------------------------------------------------------------
-- 6) Provide convenience constants/enums + global log wrappers if needed
--------------------------------------------------------------------------------
local M = {}

-- Classes
M.Logger      = Logger
M.GameConfig  = GameConfig
M.GamePlayer  = GamePlayer

-- Enumerations
M.GamePlayerState = {
    INITIAL    = 0,
    READY      = 1,
    PLAYING    = 2,
    PAUSED     = 3,
    FOCUS_LOST = 4,
}

M.PathCategory = {
    CONFIG           = 0,
    LOG              = 1,
    CMO              = 2,
    ROOT             = 3,
    PLUGINS          = 4,
    RENDER_ENGINES   = 5,
    MANAGERS         = 6,
    BUILDING_BLOCKS  = 7,
    SOUNDS           = 8,
    TEXTURES         = 9,
    DATA             = 10,
    SCRIPTS          = 11,
}

M.LogMode = {
    APPEND    = 0,
    OVERWRITE = 1,
}

M.LogLevel = {
    TRACE = 0,
    DEBUG = 1,
    INFO  = 2,
    WARN  = 3,
    ERROR = 4,
    FATAL = 5,
    OFF   = 6,
}

-- Global log methods (optional convenience wrappers)
function M.log(level, format, ...)
    game.bpLog(level, format, ...)
end

function M.logTrace(format, ...)
    game.bpLogTrace(format, ...)
end

function M.logDebug(format, ...)
    game.bpLogDebug(format, ...)
end

function M.logInfo(format, ...)
    game.bpLogInfo(format, ...)
end

function M.logWarn(format, ...)
    game.bpLogWarn(format, ...)
end

function M.logError(format, ...)
    game.bpLogError(format, ...)
end

function M.logFatal(format, ...)
    game.bpLogFatal(format, ...)
end

--------------------------------------------------------------------------------
-- 7) Return the module table
--------------------------------------------------------------------------------
return M
