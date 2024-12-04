workspace ("vane-compiler")
    location (ROOT_DIR_PATH)
    -- configurations
    configurations {
        "debug",
        "release",
    }
    -- platforms
    platforms {
        "x32",
        "x64",
    }
    -- filters
    filter ("configurations:debug")
        runtime("debug")
        symbols("on")
        optimize("off")
    
    filter ("configurations:release")
        runtime("release")
        symbols("off")
        optimize("on")
        
    filter ("platforms:x32")
        architecture("x32")

    filter ("platforms:x64")
        architecture("x64")
    
    -- projects
    include(PROJECTS_DIR_PATH .. "vanec")
    include(PROJECTS_DIR_PATH .. "tests")
    include(PROJECTS_DIR_PATH .. "testbed")