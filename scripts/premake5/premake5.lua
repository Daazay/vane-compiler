-- Makes a path relative to the working dir path
WORK_DIR_REL_PATH = function(path)
    return string.format( "%s/%s/", _WORKING_DIR, path )
end

-- global vars
ROOT_DIR_PATH           = _WORKING_DIR
PROJECTS_DIR_PATH       = WORK_DIR_REL_PATH("projects")
DEPENDENCIES_DIR_PATH   = WORK_DIR_REL_PATH("dependencies")
OUTPUT_DIR_PATH         = WORK_DIR_REL_PATH("output")
OUTPUT_BIN_DIR_PATH     = (OUTPUT_DIR_PATH .. "bin/%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}")
OUTPUT_OBJ_DIR_PATH     = (OUTPUT_DIR_PATH .. "obj/%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}")

-- includes
include("workspace.lua")
include("actions.lua")