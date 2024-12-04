project ("tests")
    kind            ("ConsoleApp")
    language        ("C")
    cdialect        ("C17")
    systemversion   ("latest")
    warnings        ("default")
    -- 
    location        (".")
    targetdir       (OUTPUT_BIN_DIR_PATH)
    objdir          (OUTPUT_OBJ_DIR_PATH)
    -- 
    files {
        "src/**.h",
        "src/**.c",
    }
    -- 
    includedirs {
        PROJECTS_DIR_PATH .. "vanec/include",
        DEPENDENCIES_DIR_PATH,
    }
    -- 
    links {
        "vanec",
    }
    -- 
    debugargs { 
        "--enable-mixed-units",
    }