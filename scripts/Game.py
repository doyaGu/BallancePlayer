import os
import ctypes
from ctypes import (
    cdll, c_bool, c_char_p, c_int, c_size_t, c_void_p, Structure, POINTER
)

# Forward declarations (empty structures) so we can use POINTER(...) in function prototypes.
class BpLogger(Structure):
    pass

class BpGameConfig(Structure):
    pass

class BpGamePlayer(Structure):
    pass

# Load the DLL
_game = cdll.LoadLibrary("./Game.dll")  # Adjust path/name as needed

# ------------------------------------------------------------------------------
# 1) Define function prototypes
# ------------------------------------------------------------------------------

# -- Logger
_game.bpGetLogger.argtypes = [c_char_p]
_game.bpGetLogger.restype  = POINTER(BpLogger)

_game.bpGetDefaultLogger.argtypes = []
_game.bpGetDefaultLogger.restype  = POINTER(BpLogger)

_game.bpSetDefaultLogger.argtypes = [POINTER(BpLogger)]
_game.bpSetDefaultLogger.restype  = None

_game.bpLoggerAddRef.argtypes = [POINTER(BpLogger)]
_game.bpLoggerAddRef.restype  = c_int

_game.bpLoggerRelease.argtypes = [POINTER(BpLogger)]
_game.bpLoggerRelease.restype  = c_int

_game.bpLoggerGetName.argtypes = [POINTER(BpLogger)]
_game.bpLoggerGetName.restype  = c_char_p

_game.bpLoggerGetLevel.argtypes = [POINTER(BpLogger)]
_game.bpLoggerGetLevel.restype  = c_int  # BpLogLevel is an int

_game.bpLoggerGetLevelString.argtypes = [POINTER(BpLogger), c_int]
_game.bpLoggerGetLevelString.restype  = c_char_p

_game.bpLoggerSetLevel.argtypes = [POINTER(BpLogger), c_int]
_game.bpLoggerSetLevel.restype  = None

# For vararg log functions (e.g. bpLoggerLog, bpLoggerTrace), 
# we cannot easily bind them with ctypes. Omit them for now.

# -- GameConfig
_game.bpCreateGameConfig.argtypes = [c_char_p]
_game.bpCreateGameConfig.restype  = POINTER(BpGameConfig)

_game.bpDestroyGameConfig.argtypes = [POINTER(BpGameConfig)]
_game.bpDestroyGameConfig.restype  = None

_game.bpGameConfigIsInitialized.argtypes = [POINTER(BpGameConfig)]
_game.bpGameConfigIsInitialized.restype  = c_bool

_game.bpGameConfigInit.argtypes = [POINTER(BpGameConfig)]
_game.bpGameConfigInit.restype  = c_bool

_game.bpGameConfigRelease.argtypes = [POINTER(BpGameConfig)]
_game.bpGameConfigRelease.restype  = None

_game.bpGameConfigRead.argtypes = [POINTER(BpGameConfig), c_char_p, c_size_t]
_game.bpGameConfigRead.restype  = c_bool

_game.bpGameConfigWrite.argtypes = [POINTER(BpGameConfig), c_void_p]
_game.bpGameConfigWrite.restype  = c_char_p

_game.bpGameConfigFree.argtypes = [POINTER(BpGameConfig), c_char_p]
_game.bpGameConfigFree.restype  = None

_game.bpGameConfigLoad.argtypes = [POINTER(BpGameConfig), c_char_p]
_game.bpGameConfigLoad.restype  = c_bool

_game.bpGameConfigSave.argtypes = [POINTER(BpGameConfig), c_char_p]
_game.bpGameConfigSave.restype  = c_bool

_game.bpGetGamePath.argtypes = [POINTER(BpGameConfig), c_int]
_game.bpGetGamePath.restype  = c_char_p

_game.bpSetGamePath.argtypes = [POINTER(BpGameConfig), c_int, c_char_p]
_game.bpSetGamePath.restype  = c_bool

_game.bpHasGamePath.argtypes = [POINTER(BpGameConfig), c_int]
_game.bpHasGamePath.restype  = c_bool

_game.bpResetGamePath.argtypes = [POINTER(BpGameConfig), c_int]
_game.bpResetGamePath.restype  = c_bool

# -- GamePlayer
_game.bpCreateGamePlayer.argtypes = [c_char_p]
_game.bpCreateGamePlayer.restype  = POINTER(BpGamePlayer)

_game.bpDestroyGamePlayer.argtypes = [POINTER(BpGamePlayer)]
_game.bpDestroyGamePlayer.restype  = None

_game.bpGetGamePlayerById.argtypes = [c_int]
_game.bpGetGamePlayerById.restype  = POINTER(BpGamePlayer)

_game.bpGetGamePlayerByName.argtypes = [c_char_p]
_game.bpGetGamePlayerByName.restype  = POINTER(BpGamePlayer)

_game.bpGamePlayerGetId.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerGetId.restype  = c_int

_game.bpGamePlayerGetName.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerGetName.restype  = c_char_p

_game.bpGamePlayerGetState.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerGetState.restype  = c_int  # BpGamePlayerState is int

_game.bpGamePlayerGetLogger.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerGetLogger.restype  = POINTER(BpLogger)

_game.bpGamePlayerGetConfig.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerGetConfig.restype  = POINTER(BpGameConfig)

_game.bpGamePlayerGetMainWindow.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerGetMainWindow.restype  = c_void_p

_game.bpGamePlayerGetRenderWindow.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerGetRenderWindow.restype  = c_void_p

_game.bpGamePlayerInit.argtypes = [POINTER(BpGamePlayer), POINTER(BpGameConfig), c_void_p]
_game.bpGamePlayerInit.restype  = c_bool

_game.bpGamePlayerLoad.argtypes = [POINTER(BpGamePlayer), c_char_p]
_game.bpGamePlayerLoad.restype  = c_bool

_game.bpGamePlayerRun.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerRun.restype  = None

_game.bpGamePlayerUpdate.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerUpdate.restype  = c_bool

_game.bpGamePlayerProcess.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerProcess.restype  = None

_game.bpGamePlayerRender.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerRender.restype  = None

_game.bpGamePlayerShutdown.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerShutdown.restype  = c_bool

_game.bpGamePlayerPlay.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerPlay.restype  = None

_game.bpGamePlayerPause.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerPause.restype  = None

_game.bpGamePlayerReset.argtypes = [POINTER(BpGamePlayer)]
_game.bpGamePlayerReset.restype  = None

# ------------------------------------------------------------------------------
# 2) Enumerations (as Python constants or IntEnum)
# ------------------------------------------------------------------------------
class GamePlayerState:
    INITIAL    = 0
    READY      = 1
    PLAYING    = 2
    PAUSED     = 3
    FOCUS_LOST = 4

class PathCategory:
    CONFIG          = 0
    LOG             = 1
    CMO             = 2
    ROOT            = 3
    PLUGINS         = 4
    RENDER_ENGINES  = 5
    MANAGERS        = 6
    BUILDING_BLOCKS = 7
    SOUNDS          = 8
    TEXTURES        = 9
    DATA            = 10
    SCRIPTS         = 11

class LogMode:
    APPEND    = 0
    OVERWRITE = 1

class LogLevel:
    TRACE = 0
    DEBUG = 1
    INFO  = 2
    WARN  = 3
    ERROR = 4
    FATAL = 5
    OFF   = 6

# ------------------------------------------------------------------------------
# 3) Define OO-style Python wrappers
# ------------------------------------------------------------------------------

# -- Logger
class Logger:
    def __init__(self, logger_ptr):
        """Wrap an existing BpLogger* pointer."""
        self._logger = logger_ptr
        if not self._logger:
            raise ValueError("Received a null BpLogger pointer.")

    @classmethod
    def get_default(cls):
        """Return the default logger as a Logger instance."""
        ptr = _game.bpGetDefaultLogger()
        if not ptr:
            raise RuntimeError("Failed to get the default logger.")
        return cls(ptr)

    @classmethod
    def get_named(cls, name: str):
        """Create or retrieve a named logger."""
        name_bytes = name.encode("utf-8") if name else None
        ptr = _game.bpGetLogger(name_bytes)
        if not ptr:
            raise RuntimeError(f"Failed to get logger with name: {name}")
        return cls(ptr)

    def set_as_default(self):
        _game.bpSetDefaultLogger(self._logger)

    def add_ref(self) -> int:
        return _game.bpLoggerAddRef(self._logger)

    def release(self) -> int:
        return _game.bpLoggerRelease(self._logger)

    def get_name(self) -> str:
        c_str = _game.bpLoggerGetName(self._logger)
        return c_str.decode("utf-8") if c_str else ""

    def get_level(self) -> int:
        return _game.bpLoggerGetLevel(self._logger)

    def get_level_string(self, level: int) -> str:
        c_str = _game.bpLoggerGetLevelString(self._logger, level)
        return c_str.decode("utf-8") if c_str else ""

    def set_level(self, level: int):
        _game.bpLoggerSetLevel(self._logger, level)

    # For vararg log calls, not directly supported by ctypes.

# -- GameConfig
class GameConfig:
    def __init__(self, name: str = ""):
        """Creates a new game config. If name is empty, pass None or b''."""
        name_bytes = name.encode("utf-8") if name else None
        ptr = _game.bpCreateGameConfig(name_bytes)
        if not ptr:
            raise RuntimeError("Failed to create game config.")
        self._config = ptr

    def destroy(self):
        if self._config:
            _game.bpDestroyGameConfig(self._config)
            self._config = None

    def is_initialized(self) -> bool:
        return bool(_game.bpGameConfigIsInitialized(self._config))

    def init(self) -> bool:
        return bool(_game.bpGameConfigInit(self._config))

    def release(self):
        _game.bpGameConfigRelease(self._config)

    def read(self, json_str: str) -> bool:
        """Read config from JSON string."""
        data = json_str.encode("utf-8")
        return bool(_game.bpGameConfigRead(self._config, data, len(data)))

    def write(self) -> str:
        """Write config to JSON string. Must free the returned buffer."""
        c_str = _game.bpGameConfigWrite(self._config, None)
        if not c_str:
            raise RuntimeError("Failed to write GameConfig to JSON.")
        result = ctypes.string_at(c_str).decode("utf-8")
        _game.bpGameConfigFree(self._config, c_str)
        return result

    def load(self, filename: str) -> bool:
        return bool(_game.bpGameConfigLoad(self._config, filename.encode("utf-8")))

    def save(self, filename: str) -> bool:
        return bool(_game.bpGameConfigSave(self._config, filename.encode("utf-8")))

    def get_path(self, category: int) -> str:
        c_str = _game.bpGetGamePath(self._config, category)
        return c_str.decode("utf-8") if c_str else ""

    def set_path(self, category: int, path: str) -> bool:
        return bool(_game.bpSetGamePath(self._config, category, path.encode("utf-8")))

    def has_path(self, category: int) -> bool:
        return bool(_game.bpHasGamePath(self._config, category))

    def reset_path(self, category: int) -> bool:
        return bool(_game.bpResetGamePath(self._config, category))

# -- GamePlayer
class GamePlayer:
    def __init__(self, name: str = ""):
        """Creates a new game player. If name is empty, pass None or b''."""
        name_bytes = name.encode("utf-8") if name else None
        ptr = _game.bpCreateGamePlayer(name_bytes)
        if not ptr:
            raise RuntimeError("Failed to create game player.")
        self._player = ptr

    def destroy(self):
        if self._player:
            _game.bpDestroyGamePlayer(self._player)
            self._player = None

    @classmethod
    def get_by_id(cls, player_id: int):
        ptr = _game.bpGetGamePlayerById(player_id)
        if not ptr:
            raise RuntimeError(f"No game player with ID {player_id} found.")
        return cls._wrap(ptr)

    @classmethod
    def get_by_name(cls, player_name: str):
        name_bytes = player_name.encode("utf-8") if player_name else None
        ptr = _game.bpGetGamePlayerByName(name_bytes)
        if not ptr:
            raise RuntimeError(f"No game player with name {player_name} found.")
        return cls._wrap(ptr)

    @classmethod
    def _wrap(cls, ptr):
        """Internal helper to wrap an existing BpGamePlayer pointer."""
        obj = cls.__new__(cls)
        obj._player = ptr
        return obj

    def get_id(self) -> int:
        return _game.bpGamePlayerGetId(self._player)

    def get_name(self) -> str:
        c_str = _game.bpGamePlayerGetName(self._player)
        return c_str.decode("utf-8") if c_str else ""

    def get_state(self) -> int:
        return _game.bpGamePlayerGetState(self._player)

    def get_logger(self):
        logger_ptr = _game.bpGamePlayerGetLogger(self._player)
        if logger_ptr:
            return Logger(logger_ptr)
        return None

    def get_config(self):
        config_ptr = _game.bpGamePlayerGetConfig(self._player)
        if config_ptr:
            cfg_obj = GameConfig.__new__(GameConfig)
            cfg_obj._config = config_ptr
            return cfg_obj
        return None

    def init(self, config: GameConfig, hInstance=None) -> bool:
        # hInstance can be passed as an int/None; from Python it's typically None
        return bool(_game.bpGamePlayerInit(self._player, config._config, hInstance))

    def load(self, filename: str or None) -> bool:
        if filename is None:
            return bool(_game.bpGamePlayerLoad(self._player, None))
        return bool(_game.bpGamePlayerLoad(self._player, filename.encode("utf-8")))

    def run(self):
        _game.bpGamePlayerRun(self._player)

    def update(self) -> bool:
        return bool(_game.bpGamePlayerUpdate(self._player))

    def process(self):
        _game.bpGamePlayerProcess(self._player)

    def render(self):
        _game.bpGamePlayerRender(self._player)

    def shutdown(self) -> bool:
        return bool(_game.bpGamePlayerShutdown(self._player))

    def play(self):
        _game.bpGamePlayerPlay(self._player)

    def pause(self):
        _game.bpGamePlayerPause(self._player)

    def reset(self):
        _game.bpGamePlayerReset(self._player)

# ------------------------------------------------------------------------------
# Export items
# ------------------------------------------------------------------------------
__all__ = [
    "Logger",
    "GameConfig",
    "GamePlayer",
    "GamePlayerState",
    "PathCategory",
    "LogMode",
    "LogLevel",
]
